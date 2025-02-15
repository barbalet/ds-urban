/****************************************************************

 urbanoutput.c

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../apesdk/toolkit/toolkit.h"
#include "../apesdk/render/glrender.h"
#include "game/mushroom.h"
#include "game/city.h"

extern n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number);

n_int draw_error(n_constant_string error_text, n_constant_string location, n_int line_number)
{
    printf("ERROR: %s, %s line: %ld\n", error_text, location, line_number);
    return -1;
}

int main(int argc, const char * argv[]) {
    n_byte2   seed[4];
    n_uint random;
    
    
    if ( argc != 2 )
    {
        return 0;
    }
    
    srand( ( unsigned int ) time( NULL ) );

    random  = rand();
    random ^= (n_uint)rand()>>8;
    random ^= (n_uint)rand()>>16;
    random ^= (n_uint)rand()>>24;

    seed[3] = (random >>  0) & 0xffff;
    seed[2] = (random >> 16) & 0xffff;
    seed[1] = (random >> 32) & 0xffff;
    seed[0] = (random >> 48) & 0xffff;
    
    seed[0] ^= seed[2];
    seed[1] ^= seed[3];

    math_random(seed);
    math_random(seed);
    math_random(seed);
    math_random(seed);
    math_random(seed);
    
    neighborhood_init(seed);

    neighborhood_object(( n_string )argv[1]);
    
    printf("%s\n", ( n_string )argv[1]);

    return 0;
}
