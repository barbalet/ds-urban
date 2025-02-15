/****************************************************************

   urbandraw.c

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

#include "../apesdk/toolkit/toolkit.h"
#include "../apesdk/external/png/pnglite.h"

#include <stdio.h>
#include <stdlib.h>

#define FRACTION_MAP (2)

static n_int dimen_x, dimen_y;
static n_byte * png_buffer;

static void setup_draw(n_int x, n_int y, n_byte * buffer)
{
    dimen_x = x;
    dimen_y = y;
    png_buffer = buffer;
}

n_int draw_error( n_constant_string error_text, n_constant_string location, n_int line_number )
{
    if ( error_text )
    {
        printf( "ERROR: %s @ %s %ld\n", ( n_constant_string )error_text, location, line_number );
    }
    return -1;
}

void memory_list_debug( n_string string, memory_list * debug, n_int count)
{
    printf("\n%s\n", string);
    printf("debug->count %ld\n", debug->count);
    printf("debug->max %ld\n", debug->max);
    printf("debug->unit_size %ld\n", debug->unit_size);
}

void vect_roll(n_array * local_array, n_int *filling_value0, n_int *filling_value1)
{
    n_vect2 *data_down = (n_vect2*) local_array->data;
    *filling_value0 = data_down->data[0];
    local_array = (n_array *) local_array->data;
    data_down = (n_vect2*) local_array->next;
    *filling_value1 = data_down->data[0];
}

void quad_vector(memory_list * return_value, n_string array_string)
{
    n_vect2  v1, v2, v3, v4;
    
    n_array * local_array = (n_array*)obj_get_array(array_string);
    
    n_array * local_next = (n_array*)local_array->data;
    object_output_array((n_array *)local_array->data);
    vect_roll(local_array, &v1.x, &v1.y);
    
    local_next = local_array->next;

    object_output_array((n_array *)local_next->data);
    vect_roll(local_next, &v2.x, &v2.y);
    
    local_next = local_array->next;

    local_next = (n_array *)local_next->next;
    
    object_output_array((n_array *)local_next->data);
    vect_roll(local_next, &v3.x, &v3.y);

    local_next = local_array->next;

    local_next = (n_array *)local_next->next;
    local_next = (n_array *)local_next->next;

    object_output_array((n_array *)local_next->data);
    vect_roll(local_next, &v4.x, &v4.y);

    memory_list_copy(return_value, (n_byte*)&v1, sizeof(n_vect2));
    memory_list_copy(return_value, (n_byte*)&v2, sizeof(n_vect2));
    memory_list_copy(return_value, (n_byte*)&v3, sizeof(n_vect2));
    memory_list_copy(return_value, (n_byte*)&v4, sizeof(n_vect2));
}

void wall_grouping(n_array *iwalls_level, memory_list *plain_walls)
{
    if (iwalls_level && iwalls_level->data)
    {
        n_string str_innerwalls = 0L, str_outerwalls = 0L;

        object_output_object((n_object*)iwalls_level->data);
        
        str_innerwalls = obj_contains((n_object *)iwalls_level->data, "inner_walls", OBJECT_ARRAY);
        str_outerwalls = obj_contains((n_object *)iwalls_level->data, "outer_walls", OBJECT_ARRAY);
        
        if (str_innerwalls)
        {
            quad_vector(plain_walls, str_innerwalls);
        }
        if (str_outerwalls)
        {
            quad_vector(plain_walls, str_outerwalls);
        }
    }
}

static void draw_pixel(n_int x, n_int y)
{
    if ((x >= 0) && (x < dimen_x) && (y >= 0) && (y < dimen_y))
    {
        n_int location = (x + (y * dimen_x))*3;
        
        if (png_buffer)
        {
            png_buffer[location] = 255;
            png_buffer[location+1] = 255;
            png_buffer[location+2] = 255;
        }
    }
}

n_byte draw_trickypixel( n_int px, n_int py, n_int dx, n_int dy, void *information )
{
    draw_pixel(px, py);
    return 0;
}


void draw_line(n_vect2 *in, n_vect2 *out)
{
    n_join draw_draw;
    draw_draw.pixel_draw = draw_trickypixel;
    math_line_vect(in, out, &draw_draw);
}

void draw_output(n_vect2 * plus_location, memory_list * plain_walls, n_string outputfile)
{
    n_byte * mapsite = memory_new(plus_location->x * plus_location->y * 4);
    if (mapsite == 0L)
    {
        (void)SHOW_ERROR("No map site allocated");
        return;
    }
    memory_erase(mapsite, plus_location->x * plus_location->y * 4);
    
    setup_draw(plus_location->x, plus_location->y, mapsite);

    // pad drawing here
    // remember / FRACTION_MAP to fit space
    n_vect2 * vector = (n_vect2*)plain_walls->data;
    n_vect2 p1, p2, p3, p4;
    n_int loop = 0;
    while(loop < plain_walls->count){
        p1.x = vector[loop].x * FRACTION_MAP;
        p1.y = vector[loop].y * FRACTION_MAP;
        loop++;
        p2.x = vector[loop].x * FRACTION_MAP;
        p2.y = vector[loop].y * FRACTION_MAP;
        loop++;
        
        p3.x = vector[loop].x * FRACTION_MAP;
        p3.y = vector[loop].y * FRACTION_MAP;
        loop++;
        p4.x = vector[loop].x * FRACTION_MAP;
        p4.y = vector[loop].y * FRACTION_MAP;
        loop++;
        draw_line(&p1, &p2);
        draw_line(&p2, &p3);
        draw_line(&p3, &p4);
        draw_line(&p4, &p1);
    }    
    (void)write_png_file(outputfile, plus_location->x, plus_location->y, mapsite);
    memory_free((void **)&mapsite);
}


void tof_gather( n_string file_in )
{
    n_file   *in_file = io_file_new();
    n_int    file_error = io_disk_read( in_file, file_in );
    n_object_type type_of;
    
    memory_list * plain_walls = memory_list_new(sizeof(n_vect2), 8);
    
    io_whitespace_json( in_file );
    if ( file_error != -1 )
    {
        void *returned_blob = unknown_file_to_tree( in_file, &type_of );
        io_file_free( &in_file );
        if ( returned_blob )
        {
            if (type_of == OBJECT_OBJECT)
            {
                n_object *twoblocks_level = (n_object *)returned_blob;
                
                n_array *new_houses_level = 0L;
                
                n_array *houses_level = object_onionskin(twoblocks_level,"twoblocks");
                
                printf("houses count %ld \n", obj_array_count(houses_level));
                
                while ((new_houses_level = obj_array_next( houses_level, new_houses_level ))){
                    
                    if (new_houses_level && new_houses_level->data)
                    {
                        n_array *new_rooms_level = 0L;
                        n_array *rooms_level = object_onionskin((n_object *)new_houses_level->data,"houses");
                        
                        printf("rooms_level count %ld \n", obj_array_count(rooms_level));
                        
                        while ((new_rooms_level = obj_array_next( rooms_level, new_rooms_level ))){
                            
                            if (new_rooms_level && new_rooms_level->data)
                            {
                                n_array *new_iwalls_level = 0L;
                                n_array *iwalls_level = object_onionskin((n_object *)new_rooms_level->data,"rooms");
                                
                                printf("iwalls_level count %ld \n", obj_array_count(iwalls_level));
                                
                                while ((new_iwalls_level = obj_array_next( iwalls_level, new_iwalls_level ))){
                                    

                                    
                                    wall_grouping(new_iwalls_level, plain_walls);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                printf("expecting object\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            printf( "no returned object\n" );
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf( "reading from disk failed\n" );
        exit(EXIT_FAILURE);
    }
    
    n_vect2 * minmax = vect2_min_max_init();
    n_uint loop = 0;
    n_vect2 * vect_data = (n_vect2 *) plain_walls->data;
    while (loop < plain_walls->count)
    {
        vect2_min_max_permutation(&vect_data[loop], minmax);
        loop++;
    }
    printf("minmax %ld, %ld %ld, %ld\n", minmax[0].x, minmax[0].y, minmax[1].x, minmax[1].y );
    printf("minmax %ld, %ld \n", minmax[1].x - minmax[0].x, minmax[1].y - minmax[0].y );
    
    n_vect2 plus_location;
    
    plus_location.x = (minmax[1].x - minmax[0].x) / FRACTION_MAP;
    plus_location.y = (minmax[1].y - minmax[0].y) / FRACTION_MAP;
    
    draw_output(&plus_location, plain_walls, "group_output.png");
    
    memory_free((void**)&minmax );
    memory_list_free(&plain_walls);
}

int main( int argc, const char *argv[] )
{
    if ( argc == 2 )
    {
        tof_gather( ( n_string )argv[1] );
    }
    exit(EXIT_SUCCESS);
}

