/****************************************************************
 
 ASMacView.m
 
 =============================================================
 
 Copyright 1996-2024 Tom Barbalet. All rights reserved.
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 
 This software is a continuing work of Tom Barbalet, begun on
 13 June 1996. No apes or cats were harmed in the writing of
 this software.
 
 ****************************************************************/

#import "ASMacView.h"

#import "ASShared.h"
#import "AppDelegate.h"

#include <stdio.h>
#include <stdlib.h>

@interface ASMacView()


@end

@implementation ASMacView


- (void) quitProcedure
{
    [self.shared close];
    
    exit(0);
}

- (void) drawRect:(NSRect)rect
{
    size_t      dim_y = (size_t)rect.size.height;
    size_t      dim_x = (size_t)rect.size.width;

    [self.shared cycle];
    
    if ([self.shared cycleQuit])
    {
        NSLog(@"Quit procedure initiated");
        [self quitProcedure];
    }
    
    if ([self.shared cycleNewApes])
    {
        NSLog(@"New apes neede to continue simulation");
        [self.shared newAgents];
    }
            
    /* Sonoma fix */
    if (@available(macOS 14, *)) {
        dim_y = dim_y - 28;
    }
    [self blitCode:dim_x width:dim_y];
}

- (void) blitCode:(size_t)dim_x width:(size_t)dim_y
{
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    CGContextSaveGState(context);
    
    n_byte * byteImage = shared_draw([self.shared sharedId], dim_x, dim_y, 0);
    {
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        
        CGContextRef     drawRef = CGBitmapContextCreate(byteImage, dim_x, dim_y, 8, dim_x * 4, colorSpace, (CGBitmapInfo) kCGBitmapByteOrder32Big|kCGImageAlphaNoneSkipFirst);
        
        CGColorSpaceRelease( colorSpace );
                
        CGContextSetBlendMode(context, kCGBlendModeCopy);
        CGContextSetShouldAntialias(context, NO);
        CGContextSetAllowsAntialiasing(context, NO);
        {
            CGImageRef image = CGBitmapContextCreateImage(drawRef);
            
            NSRect newRect;
            
            newRect.origin.x = 0;
            newRect.origin.y = 0;
            newRect.size.width = dim_x;
            newRect.size.height = dim_y;
            
            CGContextDrawImage(context, newRect, image);
            CGImageRelease(image);
        }
        CGContextRelease(drawRef);
    }
    CGContextRestoreGState(context);
}

static CVReturn renderCallback(CVDisplayLinkRef displayLink,
                               const CVTimeStamp *inNow,
                               const CVTimeStamp *inOutputTime,
                               CVOptionFlags flagsIn,
                               CVOptionFlags *flagsOut,
                               void *displayLinkContext)
{
    return [(__bridge ASMacView *)displayLinkContext renderTime:inOutputTime];
}

- (CVReturn) renderTime:(const CVTimeStamp *)inOutputTime
{
    __weak ASMacView *weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        weakSelf.needsDisplay = YES;
    });
    
    return kCVReturnSuccess;
}

- (void) startView
{
    self.shared = [[ASShared alloc] initWithFrame:[self bounds]];
    
    NSLog(@"startView %@", self.shared);

    CGDirectDisplayID   displayID = CGMainDisplayID();
    CVReturn            error = kCVReturnSuccess;
    error = CVDisplayLinkCreateWithCGDisplay(displayID, &_displayLink);
    if (error)
    {
        NSLog(@"DisplayLink created with error:%d", error);
        _displayLink = NULL;
    }
    CVDisplayLinkSetOutputCallback(_displayLink, renderCallback, (__bridge void *)self);
    CVDisplayLinkStart(_displayLink);
}

- (void) awakeFromNib
{    
    NSLog(@"Starting up");
    [self startView];
    [self startEverything:YES];
    {
        AppDelegate *appDelegate = (AppDelegate *)[[NSApplication sharedApplication] delegate];
        
        if (appDelegate)
        {
            [appDelegate addShared:self.shared];
        }
    }
}

- (IBAction) menuQuit:(id) sender
{
    NSLog(@"Quit from menu");
    CVDisplayLinkStop(_displayLink);
    [self quitProcedure];
}

- (IBAction) aboutDialog:(id) sender
{
    [self.shared about];
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (BOOL) becomeFirstResponder
{
    return YES;
}

- (BOOL) resignFirstResponder
{
    return YES;
}

- (void) startEverything:(BOOL)headyLifting
{
    NSSize increments;
    increments.height = 4;
    increments.width = 4;
    [[self window] setContentResizeIncrements:increments];
    if (headyLifting)
    {
        if ([self.shared start] == NO)
        {
            NSLog(@"Simulation initialization failed");
            CVDisplayLinkStop(_displayLink);
            [self quitProcedure];
            return;
        }
    }
    [[self window] makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

#pragma mark ---- Method Overrides ----

- (void) keyUp:(NSEvent *)theEvent
{
    [self.shared keyUp];
}

- (void) keyDown:(NSEvent *)theEvent
{
    NSUInteger  local_key = 0;
    if (([theEvent modifierFlags] & NSEventModifierFlagControl) || ([theEvent modifierFlags] & NSEventModifierFlagOption))
    {
        local_key = 2048;
    }
    
    if ([theEvent modifierFlags] & NSEventModifierFlagNumericPad)
    {
        /* arrow keys have this mask */
        NSString *theArrow = [theEvent charactersIgnoringModifiers];
        unichar keyChar = 0;
        if ( [theArrow length] == 0 )
            return;            /* reject dead keys */
        if ( [theArrow length] == 1 )
        {
            keyChar = [theArrow characterAtIndex:0];
            if ( keyChar == NSLeftArrowFunctionKey )
            {
                local_key += 28;
            }
            else if ( keyChar == NSRightArrowFunctionKey )
            {
                local_key += 29;
            }
            else if ( keyChar == NSUpArrowFunctionKey )
            {
                local_key += 30;
            }
            else if ( keyChar == NSDownArrowFunctionKey )
            {
                local_key += 31;
            }
            
            [self.shared keyReceived:local_key];
        }
    }
    if ([theEvent characters])
    {
        NSRange first = [[theEvent characters] rangeOfComposedCharacterSequenceAtIndex:0];
        NSRange match = [[theEvent characters] rangeOfCharacterFromSet:[NSCharacterSet letterCharacterSet] options:0 range:first];
        if (match.location != NSNotFound) {
            unichar firstChar = [[theEvent characters] characterAtIndex:0];
            NSCharacterSet *letters = [NSCharacterSet letterCharacterSet];
            if ([letters characterIsMember:firstChar]) {
                // The first character is a letter in some alphabet
                [self.shared keyReceived:firstChar];
            }
        }
    }
}

- (BOOL) acceptsFirstMouse
{
    return YES;
}

- (void) mouseDown:(NSEvent *)theEvent
{
	NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    [self.shared mouseOption:(([theEvent modifierFlags] & NSEventModifierFlagControl) || ([theEvent modifierFlags] & NSEventModifierFlagOption))];
    [self.shared mouseReceivedWithXLocation:location.x yLocation:[self bounds].size.height - location.y];
}

- (void) rightMouseDown:(NSEvent *)theEvent
{
	[self mouseDown:theEvent];
    [self.shared mouseOption:YES];
}

- (void) otherMouseDown:(NSEvent *)theEvent
{
	[self rightMouseDown:theEvent];
}

- (void) mouseUp:(NSEvent *)theEvent
{
    [self.shared mouseUp];
}

- (void) rightMouseUp:(NSEvent *)theEvent
{
	[self mouseUp:theEvent];
}

- (void) otherMouseUp:(NSEvent *)theEvent
{
	[self mouseUp:theEvent];
}

- (void) mouseDragged:(NSEvent *)theEvent
{
	[self mouseDown:theEvent];
}

- (void) rightMouseDragged:(NSEvent *)theEvent
{
	[self rightMouseDown:theEvent];
}

- (void) otherMouseDragged:(NSEvent *)theEvent
{
	[self rightMouseDown:theEvent];
}

- (void) scrollWheel:(NSEvent *)theEvent
{
    [self.shared delta_x:[theEvent deltaX] delta_y:[theEvent deltaY]];
}

- (void) magnifyWithEvent:(NSEvent *)event
{
    [self.shared zoom:[event magnification]];
}

- (void) rotateWithEvent:(NSEvent *)event
{
    [self.shared rotation:[event rotation]];
}

@end
