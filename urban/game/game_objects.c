/****************************************************************
 
 game_objects.c
 
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

// Global variables
static memory_list *block_list;
static memory_list *draw_identifier_list;
static n_int hit_block = 0;
static n_int matrix_timer = 0;
static simulated_agent mushroom_boy;

// Initialize the agent's position, facing direction, and zoom level
void agent_init(void) {
    mushroom_boy.location = (n_vect2){0, 0}; // Start at origin
    mushroom_boy.facing = 0;                 // Facing default direction
    mushroom_boy.zooming = 0;                // No zoom initially
}

// Get the agent's current location
n_vect2* agent_location(void) {
    return &mushroom_boy.location;
}

// Get the agent's current facing direction
n_int agent_facing(void) {
    return mushroom_boy.facing;
}

// Turn the agent by a specified delta
void agent_turn(n_int delta) {
    mushroom_boy.facing_delta += delta;
}

// Adjust the agent's zoom level within bounds
void agent_zoom(n_int zoom) {
    n_int total_zoom = mushroom_boy.zooming + zoom;
    if (total_zoom > -99 && total_zoom < 120) { // Keep zoom within limits
        mushroom_boy.zooming = total_zoom;
    }
}

// Get the agent's current zoom level
n_int agent_zooming(void) {
    return mushroom_boy.zooming;
}

// Move the agent forward or backward
void agent_move(n_int distance) {
    n_vect2 direction, local_location;
    n_vect2* current_location = agent_location();
    n_int translated_facing;

    // Save the current location
    vect2_copy(&local_location, current_location);

    // Update the agent's facing direction
    mushroom_boy.facing = (mushroom_boy.facing + mushroom_boy.facing_delta + 256) & 255;

    // Calculate the direction vector based on the facing angle
    translated_facing = (128 + 64 + 256 - agent_facing()) & 255;
    vect2_direction(&direction, translated_facing, 1);

    // Move the agent and update the location delta
    vect2_d(current_location, &direction, distance, 26880 / 20);
    vect2_subtract(&mushroom_boy.location_delta, current_location, &local_location);
}

// Reset the agent's state for the next cycle
void agent_cycle(void) {
    mushroom_boy.facing_delta = 0;
    mushroom_boy.location_delta = (n_vect2){0, 0};
}



// Function implementations

memory_list *matrix_draw_identifier(void) {
    return draw_identifier_list;
}

memory_list *matrix_draw_block(void) {
    return block_list;
}

void matrix_draw_identifier_clear(void) {
    matrix_timer++;
    if (matrix_timer == 6) {
        draw_identifier_list->count = 0;
        matrix_timer = 0;
    }
}

void matrix_draw_add(n_vect2 *start, n_vect2 *end) {
    matrix_plane draw_identifier;
    draw_identifier.start = *start;
    draw_identifier.end = *end;
#ifdef DEBUG_BLOCKING_BOUNDARIES
    draw_identifier.color = 2;
    draw_identifier.thickness = 1;
#endif
    memory_list_copy(draw_identifier_list, (n_byte *)&draw_identifier, sizeof(draw_identifier));
}

void matrix_init(void) {
    block_list = memory_list_new(sizeof(matrix_plane), 50000);
    draw_identifier_list = memory_list_new(sizeof(matrix_plane), 60000);
}

void matrix_close(void) {
    memory_list_free(&block_list);
    memory_list_free(&draw_identifier_list);
}

void matrix_add_door(n_vect2 *start, n_vect2 *end) {
    // TODO: Implement door addition logic
}

void matrix_add_fence(n_vect2 *start, n_vect2 *end) {
    matrix_plane new_fence;
    new_fence.start = *start;
    new_fence.end = *end;
#ifdef DEBUG_BLOCKING_BOUNDARIES
    new_fence.color = 3;
    new_fence.thickness = 1;
#endif
    memory_list_copy(block_list, (n_byte *)&new_fence, sizeof(new_fence));
}

void matrix_add_window(n_vect2 *start, n_vect2 *end) {
    // TODO: Implement window addition logic
}

void matrix_add_wall(n_vect2 *start, n_vect2 *end) {
    matrix_plane new_wall;
    new_wall.start = *start;
    new_wall.end = *end;
#ifdef DEBUG_BLOCKING_BOUNDARIES
    new_wall.color = 2;
    new_wall.thickness = 1;
#endif
    memory_list_copy(block_list, (n_byte *)&new_wall, sizeof(new_wall));
}

n_byte matrix_visually_open(n_vect2 *origin, n_vect2 *end) {
    n_int loop = 0;
    matrix_plane *recorded_walls;

    if (block_list == NULL) {
        return 0;
    }

    recorded_walls = (matrix_plane *)block_list->data;
    matrix_draw_add(origin, end);

    while (loop < block_list->count) {
        if (math_do_intersect(origin, end, &recorded_walls[loop].start, &recorded_walls[loop].end)) {
            matrix_draw_add(&recorded_walls[loop].start, &recorded_walls[loop].end);
            hit_block++;
            return 0;
        }
        loop++;
    }
    return 1;
}

void matrix_account(void) {
    hit_block = 0;
    matrix_draw_identifier_clear();
}


// Function to convert direction integer to string
static n_string game_object_direction(n_int direction) {
    switch (direction) {
        case 0: return "north";
        case 1: return "south";
        case 2: return "east";
        case 3: return "west";
        default: return "unspecified";
    }
}

// Function to create a fence object
n_object *game_object_fence(simulated_fence *fence) {
    return object_array(0L, "fence", object_vect2_pointer(fence->points, POINTS_PER_FENCE));
}

// Function to create a path object
static n_object *game_object_path(simulated_path *path) {
    return object_array(0L, "path", object_vect2_pointer(path->points, POINTS_PER_PATH));
}

// Function to create a path group object
static n_object *game_object_path_group(simulated_path_group *path_group) {
    n_array *path_array = 0L;
    if (path_group->number) {
        for (n_int loop = 0; loop < path_group->number; loop++) {
            array_add_empty(&path_array, array_object(game_object_path(&path_group->paths[loop])));
        }
    }
    return path_array ? object_array(0L, "paths", path_array) : 0L;
}

// Function to create an array of numbers
static n_array *game_object_numbers_array_internal(n_int *numbers, n_uint count) {
    n_array *points = 0L;
    for (n_int loop = 0; loop < count; loop++) {
        array_add_empty(&points, array_number(numbers[loop]));
    }
    return points;
}

// Function to create a tree object
static n_object *game_object_tree(simulated_tree *tree, n_string location) {
    n_object *return_object = object_string(0L, "location", location);
    object_number(return_object, "radius", tree->radius);
    object_array(return_object, "center", object_vect2_array(&tree->center));
    object_array(return_object, "values", game_object_numbers_array_internal(tree->points, POINTS_PER_TREE));
    return return_object;
}

// Function to create an array of four trees
static n_array *game_object_four_trees_internal(simulated_tree *trees) {
    n_array *created_array = 0L;
    for (n_int loop = 0; loop < 4; loop++) {
        if (tree_populated(&trees[loop])) {
            array_add_empty(&created_array, array_object(game_object_tree(&trees[loop], game_object_direction(loop))));
        }
    }
    return created_array;
}

// Function to create a window object
static n_object *game_object_window(n_vect2 *window, n_string location) {
    n_object *return_object = object_string(0L, "location", location);
    object_array(return_object, "points", object_vect2_pointer(window, 2));
    return return_object;
}

// Function to create a door object
static n_object *game_object_door(n_vect2 *door, n_string location) {
    n_object *return_object = object_string(0L, "location", location);
    object_array(return_object, "points", object_vect2_pointer(door, 4));
    return return_object;
}

// Function to create a room object
static n_object *game_object_room(simulated_room *room) {
    n_array *windows_array = 0L;
    n_array *doors_array = 0L;
    n_object *return_object = object_array(0L, "inner_walls", object_vect2_pointer(&room->points[0], 4));
    object_array(return_object, "outer_walls", object_vect2_pointer(&room->points[4], 4));

    for (n_int loop = 0; loop < 4; loop++) {
        n_string direction = game_object_direction(loop);
        n_vect2 *window = &room->points[8 + (loop * 2)];
        n_vect2 *door = &room->points[16 + (loop * 4)];
        if (house_window_present(window)) {
            array_add_empty(&windows_array, array_object(game_object_window(window, direction)));
        }
        if (house_door_present(door)) {
            array_add_empty(&doors_array, array_object(game_object_door(door, direction)));
        }
    }

    if (doors_array) object_array(return_object, "doors", doors_array);
    if (windows_array) object_array(return_object, "windows", windows_array);
    return return_object;
}

// Function to create a building object
static n_object *game_object_building(simulated_building *building) {
    n_array *created_array = 0L;
    for (n_int loop = 0; loop < building->roomcount; loop++) {
        array_add_empty(&created_array, array_object(game_object_room(&building->room[loop])));
    }
    return created_array ? object_array(0L, "rooms", created_array) : 0L;
}

// Function to create a park object
n_object *game_object_park(simulated_park *park) {
    n_object *return_object = object_object(0L, "road", game_object_path_group(&park->road));
    n_array *created_array = 0L;

    for (n_int loop = 0; loop < 16; loop++) {
        n_array *tree_array = game_object_four_trees_internal(park->trees[loop]);
        if (tree_array) {
            array_add_empty(&created_array, array_array(tree_array));
        }
    }
    object_array(return_object, "trees", created_array);
    return return_object;
}

// Function to create a twoblock object
n_object *game_object_twoblock(simulated_twoblock *twoblock) {
    n_object *return_object = 0L;
    n_array *created_array = 0L;
    n_array *trees_array = 0L;

    for (n_int loop = 0; loop < 16; loop++) {
        simulated_tree *four_trees = (simulated_tree *)&(twoblock->house[loop].trees);
        if (four_trees) {
            n_array *four_trees_array = game_object_four_trees_internal(four_trees);
            if (four_trees_array) {
                array_add_empty(&trees_array, array_array(four_trees_array));
            }
        }
        array_add_empty(&created_array, array_object(game_object_building(&twoblock->house[loop])));
    }

    return_object = object_array(0L, "houses", created_array);
    if (trees_array) object_array(return_object, "trees", trees_array);

    created_array = 0L;
    for (n_int loop = 0; loop < 8; loop++) {
        array_add_empty(&created_array, array_object(game_object_fence(&twoblock->fence[loop])));
    }
    object_array(return_object, "fences", created_array);

    object_object(return_object, "roads", game_object_path_group(&twoblock->road));
    return return_object;
}
