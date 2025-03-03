/****************************************************************

 test_gui.c

 =============================================================

 Copyright 1996-2025 Tom Barbalet. All rights reserved.

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

#include <stdio.h>
#include <time.h>

#include "../apesdk/toolkit/toolkit.h"
#include "../apesdk/shared.h"
#include "../apesdk/script/script.h"
#include "../apesdk/sim/sim.h"
#include "../apesdk/entity/entity.h"

void test_gui_run(void)
{
    n_int   counter = 0;
    n_int   tickCounter = 0x12738291;
    
    shared_init(0,tickCounter);
    tickCounter += 2601;
    
    while (counter < (512*4))
    {
        (void) shared_cycle(tickCounter, 0);
        tickCounter += 2601;
        
        shared_draw(0, 800, 600, 0);
        tickCounter += 2601;
        
        counter ++;
    }
    
    shared_close();
}

int main(int argc, const char * argv[])
{
    printf(" --- test gui --- start -----------------------------------------------\n");
    test_gui_run();
    printf(" --- test gui ---  end  -----------------------------------------------\n");
    return 0;
}

