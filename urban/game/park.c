/****************************************************************
 * park.c
 * =============================================================
 * Copyright 1996-2025 Tom Barbalet. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This software is a continuing work of Tom Barbalet, begun on
 * 13 June 1996. No apes or cats were harmed in the writing of
 * this software.
 ****************************************************************/

#include "mushroom.h"

/**
 * Initializes a park with trees and roads.
 *
 * @param seed       A seed for random number generation.
 * @param location   The base location of the park.
 * @param parks      The simulated park structure to initialize.
 */
void park_init(n_byte2 *seed, n_vect2 *location, simulated_park *parks) {
    n_int count = 0; // Counter for trees
    n_int px = 0;    // X-axis grid position

    // Initialize the road in the park
    path_init(location, &parks->road, 0, 1);

    // Loop through the park grid (4x4)
    while (px < 4) {
        n_int py = 0; // Y-axis grid position
        while (py < 4) {
            // Calculate the center of the current grid cell
            n_vect2 local_center;
            vect2_populate(&local_center,
                          (px * RESIDENCE_SPACE) + location->x,
                          (py * RESIDENCE_SPACE) + location->y);

            // Initialize trees in the current grid cell
            tree_init(parks->trees[count], seed, &local_center);

            // Set random radii for populated trees
            for (n_int i = 0; i < 4; i++) {
                if (tree_populated(&parks->trees[count][i])) {
                    parks->trees[count][i].radius = (math_random(seed) % 10) + 16;
                }
            }

            count++; // Move to the next tree set
            py++;    // Move to the next Y-axis position
        }
        px++; // Move to the next X-axis position
    }
}
