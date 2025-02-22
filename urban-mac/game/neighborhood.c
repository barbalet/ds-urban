/****************************************************************
 
 neighborhood.c
 
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

static simulated_twoblock twoblock[TWO_BLOCK_NUM];
static simulated_park park[PARK_NUM];
static simulated_fence fences[FENCE_NUM];

// Provide the neighborhood two block count.
simulated_twoblock* neighborhoood_twoblock(n_int* count) {
    *count = TWO_BLOCK_NUM;
    return twoblock;
}

// Provide the neighborhood park count.
simulated_park* neighborhoood_park(n_int* count) {
    *count = PARK_NUM;
    return park;
}

// Provide the neighborhood fence count.
simulated_fence* neighborhoood_fence(n_int* count) {
    *count = FENCE_NUM;
    return fences;
}

// Outputs the neighborhood object as a file.
void neighborhood_object(n_string file_location) {
    n_object* return_object = 0L;
    n_array* created_array = 0L;

    // Add twoblocks to the array
    for (n_int loop = 0; loop < TWO_BLOCK_NUM; loop++) {
        array_add_empty(&created_array, array_object(game_object_twoblock(&twoblock[loop])));
    }
    return_object = object_array(0L, "twoblocks", created_array);

    // Add parks to the array
    created_array = 0L;
    for (n_int loop = 0; loop < PARK_NUM; loop++) {
        array_add_empty(&created_array, array_object(game_object_park(&park[loop])));
    }
    object_array(return_object, "parks", created_array);

    // Add fences to the array
    created_array = 0L;
    for (n_int loop = 0; loop < FENCE_NUM; loop++) {
        array_add_empty(&created_array, array_object(game_object_fence(&fences[loop])));
    }
    object_array(return_object, "fences", created_array);

    // Write the output to a file
    n_file* output_file = unknown_json(return_object, OBJECT_OBJECT);
    if (output_file) {
        io_disk_write(output_file, file_location);
        io_file_free(&output_file);
    }
    unknown_free((void**)&return_object, OBJECT_OBJECT);
}

// Initialize a twoblock
void twoblock_init(n_byte2* seed, n_vect2* location, simulated_twoblock* twoblock) {
    twoblock->rotation = math_random(seed) & 1;
    house_init(seed, location, (simulated_building*)&(twoblock->house));
    path_init(location, (simulated_path_group*)&(twoblock->road), twoblock->rotation, 0);
    fence_init(seed, twoblock->rotation, location, (simulated_fence*)&(twoblock->fence));
}

// Initialize the neighborhood
void neighborhood_init(n_byte2* seed) {
    n_vect2 location = {500, 500};
    n_int park_count = 0;
    n_int twoblock_count = 0;

    // Iterate over the grid to initialize twoblocks and parks
    for (n_int py = -TWO_BLOCK_EDGE_HALF; py < TWO_BLOCK_EDGE_HALF; py++) {
        for (n_int px = -TWO_BLOCK_EDGE_HALF; px < TWO_BLOCK_EDGE_HALF; px++) {
            n_vect2 additional = {
                location.x + (px * NEIGHBORHOOD_UNIT_SPACE),
                location.y + (py * NEIGHBORHOOD_UNIT_SPACE)
            };

            n_byte2 park_probability = math_random(seed) & 255;

            // Adjust probability if limits are reached
            if (twoblock_count == TWO_BLOCK_NUM) {
                park_probability = 255;
            }
            if (park_count == PARK_NUM) {
                park_probability = 0;
            }

            // Initialize park or twoblock based on probability
            if (park_probability > PARK_PROBABILITY) {
                park_init(seed, &additional, &park[park_count++]);
            } else {
                twoblock_init(seed, &additional, &twoblock[twoblock_count++]);
            }
        }
    }

    // Initialize fences
    fences[0].points[0] = (n_vect2){CITY_BOTTOM_LEFT_X, CITY_BOTTOM_LEFT_Y};
    fences[0].points[1] = (n_vect2){CITY_TOP_RIGHT_X, CITY_BOTTOM_LEFT_Y};

    fences[1].points[0] = (n_vect2){CITY_TOP_RIGHT_X, CITY_BOTTOM_LEFT_Y};
    fences[1].points[1] = (n_vect2){CITY_TOP_RIGHT_X, CITY_TOP_RIGHT_Y};

    fences[2].points[0] = (n_vect2){CITY_TOP_RIGHT_X, CITY_TOP_RIGHT_Y};
    fences[2].points[1] = (n_vect2){CITY_BOTTOM_LEFT_X, CITY_TOP_RIGHT_Y};

    fences[3].points[0] = (n_vect2){CITY_BOTTOM_LEFT_X, CITY_TOP_RIGHT_Y};
    fences[3].points[1] = (n_vect2){CITY_BOTTOM_LEFT_X, CITY_BOTTOM_LEFT_Y};
}
