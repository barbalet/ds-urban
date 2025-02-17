/****************************************************************
 
 matrix.c
 
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
#include "mushroom.h"

// Global variables
static memory_list *block_list;
static memory_list *draw_identifier_list;
static n_int hit_block = 0;
static n_int matrix_timer = 0;

// Function prototypes
memory_list *matrix_draw_identifier(void);
memory_list *matrix_draw_block(void);
void matrix_draw_identifier_clear(void);
void matrix_draw_add(n_vect2 *start, n_vect2 *end);
void matrix_init(void);
void matrix_close(void);
void matrix_add_door(n_vect2 *start, n_vect2 *end);
void matrix_add_fence(n_vect2 *start, n_vect2 *end);
void matrix_add_window(n_vect2 *start, n_vect2 *end);
void matrix_add_wall(n_vect2 *start, n_vect2 *end);
n_byte matrix_visually_open(n_vect2 *origin, n_vect2 *end);
void matrix_account(void);

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
