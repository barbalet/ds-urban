/****************************************************************
 
 tree.c
 
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
/****************************************************************
 
 tree.c
 
 Copyright 1996-2025 Tom Barbalet. All rights reserved.
 
 Permission is granted to use, modify, and distribute this software freely,
 provided the copyright notice and permission notice are included in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 
 ****************************************************************/

#include "mushroom.h"

// Creates a tree with random points and a center location.
void tree_create(simulated_tree *tree, n_byte2 *seed, n_vect2 *center) {
    n_int wandering = (math_random(seed) % 11) - 5; // Random starting offset

    // Generate random points for the tree
    for (n_int loop = 0; loop < POINTS_PER_TREE; loop++) {
        wandering += (math_random(seed) % 11) - 5; // Randomly adjust wandering
        tree->points[loop] = 60 - wandering;      // Set tree point
    }

    // Smooth the tree points by averaging with neighboring points
    for (n_int loop = 0; loop < POINTS_PER_TREE; loop++) {
        tree->points[loop] = (tree->points[(loop + POINTS_PER_TREE - 1) % POINTS_PER_TREE] +
                              tree->points[loop] +
                              tree->points[(loop + 1) % POINTS_PER_TREE]) / 3;
    }

    // Set the tree's center and radius
    tree->center.x = center->x;
    tree->center.y = center->y;
    tree->radius = (math_random(seed) % 8) + 5; // Random radius between 5 and 12
}

// Generates a random offset for tree placement.
static n_int tree_offset(n_byte2 *seed) {
    return (math_random(seed) % 61) - 30; // Random offset between -30 and 30
}

// Clears all trees in the array.
static void tree_clear(simulated_tree *trees) {
    for (n_int loop = 0; loop < 4; loop++) {
        trees[loop].center.x = 0;
        trees[loop].center.y = 0;
        trees[loop].radius = 0;
    }
}

// Checks if a tree is populated (has a valid center or radius).
n_int tree_populated(simulated_tree *tree) {
    return (tree->center.x || tree->center.y || tree->radius) ? 1 : 0;
}

// Initializes trees randomly within a given edge boundary.
void tree_init(simulated_tree *trees, n_byte2 *seed, n_vect2 *edge) {
    n_byte mod = math_random(seed) & 15; // Random bitmask for tree placement
    n_int count = 0;

    tree_clear(trees); // Clear existing trees

    // Place trees in a grid pattern based on the bitmask
    for (n_int px = -1; px < 1; px++) {
        for (n_int py = -1; py < 1; py++) {
            if ((mod >> count) & 1) { // Check if the current bit is set
                n_vect2 local_center;
                simulated_tree *tree = &trees[count++];

                // Calculate tree center with random offset
                vect2_populate(&local_center,
                               (px * TREE_SPACE) + (TREE_SPACE / 2) + tree_offset(seed),
                               (py * TREE_SPACE) + (TREE_SPACE / 2) + tree_offset(seed));
                vect2_delta(&local_center, edge); // Adjust for edge boundary

                tree_create(tree, seed, &local_center); // Create the tree
            }
        }
    }
}
