/****************************************************************
 
 fence.c
 
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

#include "mushroom.h"

void fence_init(n_byte2 *seed, n_byte rotate, n_vect2 *location, simulated_fence *fences) {
    n_int count = 0;

    // Generate long fences
    for (n_int px = 0; px < 2; px++) {
        for (n_int py = 0; py < 1; py++) {
            simulated_fence *fence = &fences[count++];
            n_int wabble_x1 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));
            n_int wabble_y1 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));
            n_int wabble_x2 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));
            n_int wabble_y2 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));

            if (rotate) {
                fence->points[0].x = wabble_x1 + location->x + (px * RESIDENCE_SPACE * 2) + FENCE_OFFSET;
                fence->points[0].y = wabble_y1 + location->y + (py * RESIDENCE_SPACE * 4) + FENCE_START;
                fence->points[1].x = wabble_x2 + location->x + (px * RESIDENCE_SPACE * 2) + FENCE_OFFSET;
                fence->points[1].y = wabble_y2 + location->y + (py * RESIDENCE_SPACE * 4) + FENCE_END_LONG;
            } else {
                fence->points[0].x = wabble_x1 + location->x + (py * RESIDENCE_SPACE * 4) + FENCE_START;
                fence->points[0].y = wabble_y1 + location->y + (px * RESIDENCE_SPACE * 2) + FENCE_OFFSET;
                fence->points[1].x = wabble_x2 + location->x + (py * RESIDENCE_SPACE * 4) + FENCE_END_LONG;
                fence->points[1].y = wabble_y2 + location->y + (px * RESIDENCE_SPACE * 2) + FENCE_OFFSET;
            }
        }
    }

    // Generate short fences
    for (n_int px = 0; px < 4; px++) {
        for (n_int py = 0; py < 2; py++) {
            if (((px + 4) & 3) != 3) { // Skip every 4th fence
                simulated_fence *fence = &fences[count++];
                n_int wabble_x1 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));
                n_int wabble_y1 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));
                n_int wabble_x2 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));
                n_int wabble_y2 = (math_random(seed) % FENCE_WABBLE_SPACE - (FENCE_WABBLE_SPACE / 2));

                if (rotate) {
                    fence->points[0].x = wabble_x1 + location->x + (py * RESIDENCE_SPACE * 2) + FENCE_START;
                    fence->points[0].y = wabble_y1 + location->y + (px * RESIDENCE_SPACE * 1) + FENCE_OFFSET;
                    fence->points[1].x = wabble_x2 + location->x + (py * RESIDENCE_SPACE * 2) + FENCE_END_SHORT;
                    fence->points[1].y = wabble_y2 + location->y + (px * RESIDENCE_SPACE * 1) + FENCE_OFFSET;
                } else {
                    fence->points[0].x = wabble_x1 + location->x + (px * RESIDENCE_SPACE * 1) + FENCE_OFFSET;
                    fence->points[0].y = wabble_y1 + location->y + (py * RESIDENCE_SPACE * 2) + FENCE_START;
                    fence->points[1].x = wabble_x2 + location->x + (px * RESIDENCE_SPACE * 1) + FENCE_OFFSET;
                    fence->points[1].y = wabble_y2 + location->y + (py * RESIDENCE_SPACE * 2) + FENCE_END_SHORT;
                }
            }
        }
    }
}
