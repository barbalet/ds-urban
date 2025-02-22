/****************************************************************
 * house.c
 *
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
#include "toolkit.h"

#define FOOTPATH_OFFSET 300
#define FOOTPATH_WIDTH  200 // 48 inches versus 144 inches


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


void path_set(n_vect2 * points, n_int px, n_int py)
{
    points[0].x = px;
    points[0].y = py;
    points[1].x = px;
    points[1].y = py;
    points[2].x = px;
    points[2].y = py;
    points[3].x = px;
    points[3].y = py;
}

void path_init(n_vect2 * location, simulated_path_group * group, n_int rotation, n_int ringroad)
{
    n_int count = 0;
    n_int px = 0;
    n_int py = 0;
    
    px = 0;
    while (px < 2)
    {
        simulated_path * temp_path = &group->paths[count++];
        
        path_set(temp_path->points,
                     location->x + 0 - FOOTPATH_OFFSET - FOOTPATH_WIDTH,
                     location->y + (px * RESIDENCE_SPACE * 4) - FOOTPATH_OFFSET - FOOTPATH_WIDTH);
        
        temp_path->points[1].x += (RESIDENCE_SPACE * 4);
        temp_path->points[2].x += (RESIDENCE_SPACE * 4);
        
        temp_path->points[2].y += FOOTPATH_WIDTH;
        temp_path->points[3].y += FOOTPATH_WIDTH;
        
        px++;
    }
    px = 0;
    while (py < 2)
    {
        simulated_path * temp_path = &group->paths[count++];

        path_set(temp_path->points,
                     location->x + (py * RESIDENCE_SPACE * 4) - FOOTPATH_OFFSET - FOOTPATH_WIDTH,
                     location->y + 0 - FOOTPATH_OFFSET - FOOTPATH_WIDTH);
    
        temp_path->points[1].y += (RESIDENCE_SPACE * 4);
        temp_path->points[2].y += (RESIDENCE_SPACE * 4);
        
        if (py == 1)
        {
            temp_path->points[1].y += FOOTPATH_WIDTH;
            temp_path->points[2].y += FOOTPATH_WIDTH;
        }
        
        temp_path->points[2].x += FOOTPATH_WIDTH;
        temp_path->points[3].x += FOOTPATH_WIDTH;
    
        py++;
    }
    
    if (ringroad == 0)
    {
        simulated_path * temp_path = &group->paths[count++];

        if (rotation)
        {
            path_set(temp_path->points,
                     location->x + (RESIDENCE_SPACE * 2) - FOOTPATH_OFFSET - FOOTPATH_WIDTH,
                     location->y + 0 - FOOTPATH_OFFSET - FOOTPATH_WIDTH);
            
            temp_path->points[1].y += (RESIDENCE_SPACE * 4);
            temp_path->points[2].y += (RESIDENCE_SPACE * 4);
            
            temp_path->points[2].x += FOOTPATH_WIDTH;
            temp_path->points[3].x += FOOTPATH_WIDTH;

        }
        else
        {
            path_set(temp_path->points,
                     location->x + 0 - FOOTPATH_OFFSET - FOOTPATH_WIDTH ,
                     location->y + (RESIDENCE_SPACE * 2) - FOOTPATH_OFFSET - FOOTPATH_WIDTH);
            
            temp_path->points[1].x += (RESIDENCE_SPACE * 4);
            temp_path->points[2].x += (RESIDENCE_SPACE * 4);
            
            temp_path->points[2].y += FOOTPATH_WIDTH;
            temp_path->points[3].y += FOOTPATH_WIDTH;
        }
    }
    group->number = count;
}


// Function to rotate a point around a center using a direction vector
static void house_rotate_logic(n_vect2 *point, n_vect2 *points_center, n_vect2 *direction_vector, n_vect2 *center) {
    vect2_subtract(point, point, points_center);
    vect2_rotation(point, direction_vector);
    vect2_add(point, point, center);
}

// Function to transform a building's rooms based on direction and center
static void house_transform(simulated_building *building, n_vect2 *center, n_int direction) {
    n_int loop_room = 0;
    n_vect2 *min_max = vect2_min_max_init();
    n_vect2 direction_vector;
    n_vect2 points_center;

    if (min_max == 0L) return;

    // Calculate min and max points for all rooms
    while (loop_room < building->roomcount) {
        simulated_room *room = &building->room[loop_room++];
        vect2_min_max(room->points, POINTS_PER_ROOM_STRUCTURE, min_max);
    }

    vect2_center(&points_center, &min_max[1], &min_max[0]);
    vect2_direction(&direction_vector, direction, 1);
    memory_free((void **)&min_max);

    // Apply rotation logic to each room's points
    loop_room = 0;
    while (loop_room < building->roomcount) {
        n_int loop = 0;
        simulated_room *room = &building->room[loop_room++];

        // Rotate room structure points
        while (loop < 8) {
            house_rotate_logic(&room->points[loop], &points_center, &direction_vector, center);
            loop++;
        }

        // Rotate window points if present
        while (loop < 16) {
            if (house_window_present(&room->points[loop])) {
                house_rotate_logic(&room->points[loop], &points_center, &direction_vector, center);
                house_rotate_logic(&room->points[loop + 1], &points_center, &direction_vector, center);
            }
            loop += 2;
        }

        // Rotate door points if present
        while (loop < 32) {
            if (house_door_present(&room->points[loop])) {
                house_rotate_logic(&room->points[loop], &points_center, &direction_vector, center);
                house_rotate_logic(&room->points[loop + 1], &points_center, &direction_vector, center);
                house_rotate_logic(&room->points[loop + 2], &points_center, &direction_vector, center);
                house_rotate_logic(&room->points[loop + 3], &points_center, &direction_vector, center);
            }
            loop += 4;
        }
    }
}

// Check if a window is present
n_int house_window_present(n_vect2 *window) {
    return (vect2_nonzero(&window[0]) || (vect2_nonzero(&window[1]))) ? 1 : 0;
}

// Check if a door is present
n_int house_door_present(n_vect2 *door) {
    return (vect2_nonzero(&door[0]) || (vect2_nonzero(&door[1])) || (vect2_nonzero(&door[2])) || (vect2_nonzero(&door[3]))) ? 1 : 0;
}

// Construct a room with given dimensions, windows, and doors
static void house_construct(simulated_room *room, n_int topx, n_int topy, n_int botx, n_int boty, n_byte window, n_byte door) {
    n_int midx, midy;
    n_int loop = 0;

    // Initialize all points to zero
    while (loop < POINTS_PER_ROOM) {
        room->points[loop].data[0] = 0;
        room->points[loop].data[1] = 0;
        loop++;
    }

    // Ensure topx < botx and topy < boty
    if (topx > botx) { n_int temp = botx; botx = topx; topx = temp; }
    if (topy > boty) { n_int temp = boty; boty = topy; topy = temp; }

    // Populate room structure points
    vect2_populate(&room->points[0], topx, topy);
    vect2_populate(&room->points[1], topx, boty);
    vect2_populate(&room->points[2], botx, boty);
    vect2_populate(&room->points[3], botx, topy);
    vect2_populate(&room->points[4], topx + 6, topy + 6);
    vect2_populate(&room->points[5], topx + 6, boty - 6);
    vect2_populate(&room->points[6], botx - 6, boty - 6);
    vect2_populate(&room->points[7], botx - 6, topy + 6);

    // Add windows if specified
    if (window & DC_NORTH) {
        vect2_populate(&room->points[8], topx + 3, topy + 10);
        vect2_populate(&room->points[9], topx + 3, boty - 10);
    }
    if (window & DC_SOUTH) {
        vect2_populate(&room->points[10], botx - 3, topy + 10);
        vect2_populate(&room->points[11], botx - 3, boty - 10);
    }
    if (window & DC_EAST) {
        vect2_populate(&room->points[12], topx + 10, boty - 3);
        vect2_populate(&room->points[13], botx - 10, boty - 3);
    }
    if (window & DC_WEST) {
        vect2_populate(&room->points[14], topx + 10, topy + 3);
        vect2_populate(&room->points[15], botx - 10, topy + 3);
    }

    // Calculate midpoint for doors
    midx = (topx + botx) / 2;
    midy = (topy + boty) / 2;

    // Add doors if specified
    if (door & DC_NORTH) {
        vect2_populate(&room->points[16], topx + 2, midy + 5);
        vect2_populate(&room->points[17], topx + 2, midy - 5);
        vect2_populate(&room->points[18], topx - 2, midy - 5);
        vect2_populate(&room->points[19], topx - 2, midy + 5);
    }
    if (door & DC_SOUTH) {
        vect2_populate(&room->points[20], botx - 2, midy + 5);
        vect2_populate(&room->points[21], botx - 2, midy - 5);
        vect2_populate(&room->points[22], botx + 2, midy - 5);
        vect2_populate(&room->points[23], botx + 2, midy + 5);
    }
    if (door & DC_EAST) {
        vect2_populate(&room->points[24], midx + 5, boty - 2);
        vect2_populate(&room->points[25], midx - 5, boty - 2);
        vect2_populate(&room->points[26], midx - 5, boty + 2);
        vect2_populate(&room->points[27], midx + 5, boty + 2);
    }
    if (door & DC_WEST) {
        vect2_populate(&room->points[28], midx + 5, topy + 2);
        vect2_populate(&room->points[29], midx - 5, topy + 2);
        vect2_populate(&room->points[30], midx - 5, topy - 2);
        vect2_populate(&room->points[31], midx + 5, topy - 2);
    }
}

// Generate a random value within a specific range
static n_int house_plusminus(n_byte2 *seed) {
    n_int tmp = 0;
    while ((tmp > -3) && (tmp < 3)) {
        tmp = (math_random(seed) % 11) - 5;
    }
    return tmp;
}

// Generate house layout based on random seed
static n_int house_genetics(n_int *house, n_byte2 *seed) {
    n_int count = 1;
    n_int lpend = house[0] = 5 + (math_random(seed) % 11); // 5 to 15
    n_int lp = (math_random(seed) % 3) + 2;

    while (lp < lpend) {
        house[(count * 3) - 1] = 0;
        house[count * 3] = lp;
        house[(count * 3) + 1] = house_plusminus(seed);
        count++;
        lp += (math_random(seed) % 3) + 2;
    }
    house[0]++;
    return count;
}

#define HOUSE_SIZE (30)

// Create a house with rooms and transform it
void house_create(simulated_building *building, n_byte2 *seed, n_vect2 *center) {
    n_int rooms = house_genetics(building->house, seed);
    n_int count = 0;
    n_int pointp = -4;
    n_int pointm = -4;
    n_int loop = 2;
    simulated_room *room = building->room;
    n_int *house = building->house;

    // Construct rooms based on generated layout
    if (house[4] > 1) {
        house_construct(&room[count++], -8, -4, 8 + (house[4] * HOUSE_SIZE), -4 + (house[3] * HOUSE_SIZE), DC_SOUTH, DC_WEST);
        pointp = (house[3] * HOUSE_SIZE) - 4;
    } else {
        house_construct(&room[count++], -8 + (house[4] * HOUSE_SIZE), -4, 8, -4 + (house[3] * HOUSE_SIZE), DC_NORTH, DC_WEST);
        pointm = (house[3] * HOUSE_SIZE) - 4;
    }

    while (loop < (rooms - 1)) {
        if (house[(loop * 3) + 1] > 1) {
            house_construct(&room[count++], 8, -4 + (house[loop * 3] * HOUSE_SIZE), 8 + (house[(loop * 3) + 1] * HOUSE_SIZE), pointp, DC_SOUTH, DC_EAST);
            pointp = (house[loop * 3] * HOUSE_SIZE) - 4;
        } else {
            house_construct(&room[count++], -8 + (house[(loop * 3) + 1] * HOUSE_SIZE), -4 + (house[loop * 3] * HOUSE_SIZE), -8, pointm, DC_NORTH, DC_EAST);
            pointm = (house[loop * 3] * HOUSE_SIZE) - 4;
        }
        loop++;
    }

    if (house[((rooms - 1) * 3) + 1] > 1) {
        house_construct(&room[count++], -8, (house[0] * HOUSE_SIZE) + 4, 8 + (house[((rooms - 1) * 3) + 1] * HOUSE_SIZE), -4 + (house[(rooms - 1) * 3] * HOUSE_SIZE), DC_EAST, DC_WEST);
        house_construct(&room[count++], -8, pointm, -8 - (house[((rooms - 1) * 3) + 1] * HOUSE_SIZE), (house[0] * HOUSE_SIZE) + 4, DC_NORTH, DC_SOUTH);
        if (rooms != loop) {
            house_construct(&room[count++], 8, -4 + (house[(rooms - 1) * 3] * HOUSE_SIZE), 8 + (house[((rooms - 1) * 3) + 1] * HOUSE_SIZE), pointp, DC_SOUTH, DC_NORTH);
        }
    } else {
        house_construct(&room[count++], 8, (house[0] * HOUSE_SIZE) + 4, 8 + (house[((rooms - 1) * 3) + 1] * HOUSE_SIZE), -4 + (house[(rooms - 1) * 3] * HOUSE_SIZE), DC_EAST, DC_WEST);
        house_construct(&room[count++], 8, pointp, 8 - (house[((rooms - 1) * 3) + 1] * HOUSE_SIZE), (house[0] * HOUSE_SIZE) + 4, DC_SOUTH, DC_NORTH);
        if (rooms != loop) {
            house_construct(&room[count++], -8, -4 + (house[(rooms - 1) * 3] * HOUSE_SIZE), -8 + (house[((rooms - 1) * 3) + 1] * HOUSE_SIZE), pointm, DC_NORTH, DC_SOUTH);
        }
    }

    if (rooms > 2) {
        house_construct(&room[count++], 8, -4 + (house[3] * HOUSE_SIZE), -8, -4 + (house[(rooms - 1) * 3] * HOUSE_SIZE), DC_NONE, DC_EAST | DC_WEST);
    }
    building->roomcount = count;

    // Transform the building based on the center and random direction
    house_transform(building, center, math_random(seed) & 255);
}

// Initialize houses in a grid pattern
void house_init(n_byte2 *seed, n_vect2 *location, simulated_building *buildings) {
    n_int count = 0;
    n_int px = 0;

    while (px < 4) {
        n_int py = 0;
        while (py < 4) {
            n_vect2 local_center;
            simulated_building *building = &buildings[count++];
            vect2_populate(&local_center, (px * RESIDENCE_SPACE) + location->x, (py * RESIDENCE_SPACE) + location->y);
            house_create(building, seed, &local_center);
            tree_init(building->trees, seed, &local_center);
            py++;
        }
        px++;
    }
}

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
