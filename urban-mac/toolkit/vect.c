/****************************************************************

    vect.c

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

/*! \file   vect.c
 *  \brief  This covers vector math in Simulated Ape.
 */

#include "toolkit.h"
#include "stdio.h"

/** \brief new_sd stands for new sine dump and holds the sine and cosine values for the simulation */
static const n_int new_sd[256] = {
    0, 659, 1318, 1977, 2634, 3290, 3944, 4595, 5244, 5889, 6531, 7169, 7802, 8431, 9055, 9673,
    10286, 10892, 11492, 12085, 12671, 13249, 13819, 14380, 14933, 15477, 16012, 16537, 17052, 17557, 18051, 18534,
    19007, 19467, 19916, 20353, 20778, 21190, 21590, 21976, 22349, 22709, 23055, 23387, 23706, 24009, 24299, 24573,
    24833, 25078, 25308, 25523, 25722, 25906, 26074, 26226, 26363, 26484, 26589, 26677, 26750, 26807, 26847, 26871,
    26880, 26871, 26847, 26807, 26750, 26677, 26589, 26484, 26363, 26226, 26074, 25906, 25722, 25523, 25308, 25078,
    24833, 24573, 24299, 24009, 23706, 23387, 23055, 22709, 22349, 21976, 21590, 21190, 20778, 20353, 19916, 19467,
    19007, 18534, 18051, 17557, 17052, 16537, 16012, 15477, 14933, 14380, 13819, 13249, 12671, 12085, 11492, 10892,
    10286, 9673, 9055, 8431, 7802, 7169, 6531, 5889, 5244, 4595, 3944, 3290, 2634, 1977, 1318, 659,
    0, -659, -1318, -1977, -2634, -3290, -3944, -4595, -5244, -5889, -6531, -7169, -7802, -8431, -9055, -9673,
    -10286, -10892, -11492, -12085, -12671, -13249, -13819, -14380, -14933, -15477, -16012, -16537, -17052, -17557, -18051, -18534,
    -19007, -19467, -19916, -20353, -20778, -21190, -21590, -21976, -22349, -22709, -23055, -23387, -23706, -24009, -24299, -24573,
    -24833, -25078, -25308, -25523, -25722, -25906, -26074, -26226, -26363, -26484, -26589, -26677, -26750, -26807, -26847, -26871,
    -26880, -26871, -26847, -26807, -26750, -26677, -26589, -26484, -26363, -26226, -26074, -25906, -25722, -25523, -25308, -25078,
    -24833, -24573, -24299, -24009, -23706, -23387, -23055, -22709, -22349, -21976, -21590, -21190, -20778, -20353, -19916, -19467,
    -19007, -18534, -18051, -17557, -17052, -16537, -16012, -15477, -14933, -14380, -13819, -13249, -12671, -12085, -11492, -10892,
    -10286, -9673, -9055, -8431, -7802, -7169, -6531, -5889, -5244, -4595, -3944, -3290, -2634, -1977, -1318, -659
};

/**
 * Adds a 2D vector to a 2D area, updating the area's boundaries.
 * @param area The area to update.
 * @param vect The vector to add.
 * @param first Whether this is the first vector being added.
 */
void area2_add(n_area2 *area, n_vect2 *vect, n_byte first) {
    if (first) {
        area->bottom_right.x = vect->x;
        area->bottom_right.y = vect->y;
        area->top_left.x = vect->x;
        area->top_left.y = vect->y;
        return;
    }

    if (vect->x < area->top_left.x) area->top_left.x = vect->x;
    if (vect->y < area->top_left.y) area->top_left.y = vect->y;
    if (vect->x > area->bottom_right.x) area->bottom_right.x = vect->x;
    if (vect->y > area->bottom_right.y) area->bottom_right.y = vect->y;
}

/**
 * Converts an array of n_byte2 to a 2D vector (n_vect2).
 * @param converter The vector to hold the result.
 * @param input The n_byte2 array to convert.
 */
void vect2_byte2(n_vect2 *converter, n_byte2 *input) {
    NA_ASSERT(converter, "converter NULL");
    NA_ASSERT(input, "input NULL");

    if (!converter || !input) return;

    converter->x = input[0];
    converter->y = input[1];
}

/**
 * Adds two 2D vectors and stores the result in a third vector.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 */
void vect2_add(n_vect2 *equals, n_vect2 *initial, n_vect2 *second) {
    NA_ASSERT(equals, "equals NULL");
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");

    if (!equals || !initial || !second) return;

    equals->x = initial->x + second->x;
    equals->y = initial->y + second->y;
}

/**
 * Computes the center point between two 2D vectors.
 * @param center The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 */
void vect2_center(n_vect2 *center, n_vect2 *initial, n_vect2 *second) {
    vect2_add(center, initial, second);
    center->x /= 2;
    center->y /= 2;
}

/**
 * Multiplies a 2D vector by a scalar.
 * @param value The vector to multiply.
 * @param multiplier The scalar multiplier.
 */
void vect2_scalar_multiply(n_vect2 *value, n_int multiplier) {
    value->x *= multiplier;
    value->y *= multiplier;
}

/**
 * Divides a 2D vector by a scalar.
 * @param value The vector to divide.
 * @param divisor The scalar divisor.
 */
void vect2_scalar_divide(n_vect2 *value, n_int divisor) {
    value->x /= divisor;
    value->y /= divisor;
}

/**
 * Performs a bitwise right shift on a 2D vector.
 * @param value The vector to shift.
 * @param bitshiftdown The number of bits to shift.
 */
void vect2_scalar_bitshiftdown(n_vect2 *value, n_int bitshiftdown) {
    value->x >>= bitshiftdown;
    value->y >>= bitshiftdown;
}

/**
 * Subtracts one 2D vector from another and stores the result.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The vector to subtract.
 */
void vect2_subtract(n_vect2 *equals, n_vect2 *initial, n_vect2 *second) {
    NA_ASSERT(equals, "equals NULL");
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");

    if (!equals || !initial || !second) return;

    equals->x = initial->x - second->x;
    equals->y = initial->y - second->y;
}

/**
 * Divides the difference of two 2D vectors by a scalar.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 * @param divisor The scalar divisor.
 */
void vect2_divide(n_vect2 *equals, n_vect2 *initial, n_vect2 *second, n_int divisor) {
    vect2_subtract(equals, second, initial);

    if (!equals || !divisor) return;

    equals->x /= divisor;
    equals->y /= divisor;
}

/**
 * Multiplies two 2D vectors with a scalar multiplier and divisor.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 * @param multiplier The scalar multiplier.
 * @param divisor The scalar divisor.
 */
void vect2_multiplier(n_vect2 *equals, n_vect2 *initial, n_vect2 *second, n_int multiplier, n_int divisor) {
    NA_ASSERT(equals, "equals NULL");
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");
    NA_ASSERT(divisor, "divisor ZERO");

    if (!equals || !initial || !second || !divisor) return;

    equals->x = (multiplier * initial->x * second->x) / divisor;
    equals->y = (multiplier * initial->y * second->y) / divisor;
}

/**
 * Adds a scaled 2D vector to another vector.
 * @param initial The vector to update.
 * @param second The vector to scale and add.
 * @param multiplier The scalar multiplier.
 * @param divisor The scalar divisor.
 */
void vect2_d(n_vect2 *initial, n_vect2 *second, n_int multiplier, n_int divisor) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");
    NA_ASSERT(divisor, "divisor ZERO");

    if (!initial || !second || !divisor) return;

    initial->x += (multiplier * second->x) / divisor;
    initial->y += (multiplier * second->y) / divisor;
}

/**
 * Computes the dot product of two 2D vectors with a scalar multiplier and divisor.
 * @param initial The first vector.
 * @param second The second vector.
 * @param multiplier The scalar multiplier.
 * @param divisor The scalar divisor.
 * @return The dot product result.
 */
n_int vect2_dot(n_vect2 *initial, n_vect2 *second, n_int multiplier, n_int divisor) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");
    NA_ASSERT(divisor, "divisor ZERO");

    if (!initial || !second) return 0;

    return (multiplier * ((initial->x * second->x) + (initial->y * second->y))) / divisor;
}

/**
 * Checks if the distance between two 2D vectors is under a given threshold.
 * @param first The first vector.
 * @param second The second vector.
 * @param distance The distance threshold.
 * @return 1 if the distance is under the threshold, 0 otherwise.
 */
n_int vect2_distance_under(n_vect2 *first, n_vect2 *second, n_int distance) {
    n_vect2 difference = {{0, 0}};
    n_int distance_sq;
    vect2_subtract(&difference, first, second);
    distance_sq = (difference.x * difference.x) + (difference.y * difference.y);
    return (distance * distance) > distance_sq;
}

/**
 * Computes the sine value for a given direction and divisor.
 * @param direction The direction (256 units per rotation).
 * @param divisor The divisor for the output value.
 * @return The sine value.
 */
n_int math_sine(n_int direction, n_int divisor) {
    NA_ASSERT(divisor, "divisor ZERO");
    return new_sd[direction & 255] / divisor;
}

/**
 * Rotates a 2D vector by 90 degrees.
 * @param rotation The vector to rotate.
 */
void vect2_rotate90(n_vect2 *rotation) {
    n_int temp = rotation->y;
    rotation->y = -rotation->x;
    rotation->x = temp;
}

/**
 * Computes a direction vector based on a given direction and divisor.
 * @param initial The result vector.
 * @param direction The direction (256 units per rotation).
 * @param divisor The divisor for the output value.
 */
void vect2_direction(n_vect2 *initial, n_int direction, n_int divisor) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(divisor, "divisor ZERO");

    initial->x = new_sd[(direction + 64) & 255] / divisor;
    initial->y = new_sd[direction & 255] / divisor;
}

/**
 * Adds a delta vector to a 2D vector.
 * @param initial The vector to update.
 * @param delta The delta vector.
 */
void vect2_delta(n_vect2 *initial, n_vect2 *delta) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(delta, "delta NULL");

    if (!initial || !delta) return;

    initial->x += delta->x;
    initial->y += delta->y;
}

/**
 * Adds an offset to a 2D vector.
 * @param initial The vector to update.
 * @param dx The x offset.
 * @param dy The y offset.
 */
void vect2_offset(n_vect2 *initial, n_int dx, n_int dy) {
    NA_ASSERT(initial, "initial NULL");

    if (!initial) return;

    initial->x += dx;
    initial->y += dy;
}

/**
 * Converts a 2D vector back to an n_byte2 array.
 * @param converter The vector to convert.
 * @param output The resulting n_byte2 array.
 */
void vect2_back_byte2(n_vect2 *converter, n_byte2 *output) {
    NA_ASSERT(converter, "converter NULL");
    NA_ASSERT(output, "output NULL");

    if (!converter || !output) return;

    converter->x = (converter->x > 65535) ? 65535 : (converter->x < 0) ? 0 : converter->x;
    converter->y = (converter->y > 65535) ? 65535 : (converter->y < 0) ? 0 : converter->y;

    output[0] = (n_byte2)converter->x;
    output[1] = (n_byte2)converter->y;
}

/**
 * Copies one 2D vector to another.
 * @param to The destination vector.
 * @param from The source vector.
 */
void vect2_copy(n_vect2 *to, n_vect2 *from) {
    to->x = from->x;
    to->y = from->y;
}

/**
 * Populates a 2D vector with given x and y values.
 * @param value The vector to populate.
 * @param x The x value.
 * @param y The y value.
 */
void vect2_populate(n_vect2 *value, n_int x, n_int y) {
    value->x = x;
    value->y = y;
}

/**
 * Rotates a 2D vector using another vector as the rotation matrix.
 * @param location The vector to rotate.
 * @param rotation The rotation matrix.
 */
void vect2_rotation(n_vect2 *location, n_vect2 *rotation) {
    n_vect2 temp;
    temp.x = ((location->x * rotation->x) + (location->y * rotation->y)) / SINE_MAXIMUM;
    temp.y = ((location->x * rotation->y) - (location->y * rotation->x)) / SINE_MAXIMUM;
    location->x = temp.x;
    location->y = temp.y;
}

/**
 * Rotates a 2D vector using bitwise shifting.
 * @param location The vector to rotate.
 * @param rotation The rotation matrix.
 */
void vect2_rotation_bitshift(n_vect2 *location, n_vect2 *rotation) {
    n_vect2 temp;
    temp.x = ((location->x * rotation->x) + (location->y * rotation->y)) >> 15;
    temp.y = ((location->x * rotation->y) - (location->y * rotation->x)) >> 15;
    location->x = temp.x;
    location->y = temp.y;
}

/**
 * Checks if a 2D vector is non-zero.
 * @param nonzero The vector to check.
 * @return 1 if non-zero, 0 otherwise.
 */
n_int vect2_nonzero(n_vect2 *nonzero) {
    return (nonzero->x != 0) || (nonzero->y != 0);
}

/**
 * Initializes a min-max vector pair.
 * @return A pointer to the initialized min-max vector pair.
 */
n_vect2 *vect2_min_max_init(void) {
    n_vect2 *min_max = memory_new(2 * sizeof(n_vect2));
    if (!min_max) return NULL;

    vect2_populate(&min_max[0], BIG_INTEGER, BIG_INTEGER);
    vect2_populate(&min_max[1], BIG_NEGATIVE_INTEGER, BIG_NEGATIVE_INTEGER);
    return min_max;
}

/**
 * Updates the min-max vectors based on a given point.
 * @param points The point to consider.
 * @param minmax The min-max vector pair to update.
 */
void vect2_min_max_permutation(n_vect2 *points, n_vect2 *minmax) {
    n_int px = points->x;
    n_int py = points->y;

    if (px < minmax[0].x) minmax[0].x = px;
    if (py < minmax[0].y) minmax[0].y = py;
    if (px > minmax[1].x) minmax[1].x = px;
    if (py > minmax[1].y) minmax[1].y = py;
}

/**
 * Computes the min-max vectors for a set of points.
 * @param points The array of points.
 * @param number The number of points.
 * @param maxmin The min-max vector pair to update.
 */
void vect2_min_max(n_vect2 *points, n_int number, n_vect2 *maxmin) {
    for (n_int loop = 0; loop < number; loop++) {
        vect2_min_max_permutation(&points[loop], maxmin);
    }
}

/**
 * Converts an array of n_double to a 3D vector (n_vect3).
 * @param converter The vector to hold the result.
 * @param input The n_double array to convert.
 */
void vect3_double(n_vect3 *converter, n_double *input) {
    NA_ASSERT(converter, "converter NULL");
    NA_ASSERT(input, "input NULL");

    if (!converter || !input) return;

    converter->x = input[0];
    converter->y = input[1];
    converter->z = input[2];
}

/**
 * Adds two 3D vectors and stores the result in a third vector.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 */
void vect3_add(n_vect3 *equals, n_vect3 *initial, n_vect3 *second) {
    NA_ASSERT(equals, "equals NULL");
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");

    if (!equals || !initial || !second) return;

    equals->x = initial->x + second->x;
    equals->y = initial->y + second->y;
    equals->z = initial->z + second->z;
}

/**
 * Computes the center point between two 3D vectors.
 * @param center The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 */
void vect3_center(n_vect3 *center, n_vect3 *initial, n_vect3 *second) {
    vect3_add(center, initial, second);
    center->x /= 2;
    center->y /= 2;
    center->z /= 2;
}

/**
 * Subtracts one 3D vector from another and stores the result.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The vector to subtract.
 */
void vect3_subtract(n_vect3 *equals, n_vect3 *initial, n_vect3 *second) {
    NA_ASSERT(equals, "equals NULL");
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");

    if (!equals || !initial || !second) return;

    equals->x = initial->x - second->x;
    equals->y = initial->y - second->y;
    equals->z = initial->z - second->z;
}

/**
 * Divides the difference of two 3D vectors by a scalar.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 * @param divisor The scalar divisor.
 */
void vect3_divide(n_vect3 *equals, n_vect3 *initial, n_vect3 *second, n_double divisor) {
    vect3_subtract(equals, second, initial);

    if (!equals || !divisor) return;

    equals->x /= divisor;
    equals->y /= divisor;
    equals->z /= divisor;
}

/**
 * Multiplies two 3D vectors with a scalar multiplier and divisor.
 * @param equals The result vector.
 * @param initial The first vector.
 * @param second The second vector.
 * @param multiplier The scalar multiplier.
 * @param divisor The scalar divisor.
 */
void vect3_multiplier(n_vect3 *equals, n_vect3 *initial, n_vect3 *second, n_double multiplier, n_double divisor) {
    NA_ASSERT(equals, "equals NULL");
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");
    NA_ASSERT(divisor != 0, "divisor ZERO");

    if (!equals || !initial || !second || !divisor) return;

    equals->x = (multiplier * initial->x * second->x) / divisor;
    equals->y = (multiplier * initial->y * second->y) / divisor;
    equals->z = (multiplier * initial->z * second->z) / divisor;
}

/**
 * Adds a scaled 3D vector to another vector.
 * @param initial The vector to update.
 * @param second The vector to scale and add.
 * @param multiplier The scalar multiplier.
 * @param divisor The scalar divisor.
 */
void vect3_d(n_vect3 *initial, n_vect3 *second, n_double multiplier, n_double divisor) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");
    NA_ASSERT(divisor != 0, "divisor ZERO");

    if (!initial || !second || !divisor) return;

    initial->x += (multiplier * second->x) / divisor;
    initial->y += (multiplier * second->y) / divisor;
    initial->z += (multiplier * second->z) / divisor;
}

/**
 * Computes the dot product of two 3D vectors with a scalar multiplier and divisor.
 * @param initial The first vector.
 * @param second The second vector.
 * @param multiplier The scalar multiplier.
 * @param divisor The scalar divisor.
 * @return The dot product result.
 */
n_double vect3_dot(n_vect3 *initial, n_vect3 *second, n_double multiplier, n_double divisor) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(second, "second NULL");
    NA_ASSERT(divisor != 0, "divisor ZERO");

    if (!initial || !second) return 0;

    return (multiplier * ((initial->x * second->x) + (initial->y * second->y) + (initial->z * second->z))) / divisor;
}

/**
 * Adds a delta vector to a 3D vector.
 * @param initial The vector to update.
 * @param delta The delta vector.
 */
void vect3_delta(n_vect3 *initial, n_vect3 *delta) {
    NA_ASSERT(initial, "initial NULL");
    NA_ASSERT(delta, "delta NULL");

    if (!initial || !delta) return;

    initial->x += delta->x;
    initial->y += delta->y;
    initial->z += delta->z;
}

/**
 * Adds an offset to a 3D vector.
 * @param initial The vector to update.
 * @param dx The x offset.
 * @param dy The y offset.
 * @param dz The z offset.
 */
void vect3_offset(n_vect3 *initial, n_double dx, n_double dy, n_double dz) {
    NA_ASSERT(initial, "initial NULL");

    if (!initial) return;

    initial->x += dx;
    initial->y += dy;
    initial->z += dz;
}

/**
 * Converts a 3D vector back to an n_double array.
 * @param converter The vector to convert.
 * @param output The resulting n_double array.
 */
void vect3_back_double(n_vect3 *converter, n_double *output) {
    NA_ASSERT(converter, "converter NULL");
    NA_ASSERT(output, "output NULL");

    if (!converter || !output) return;

    output[0] = converter->x;
    output[1] = converter->y;
    output[2] = converter->z;
}

/**
 * Copies one 3D vector to another.
 * @param to The destination vector.
 * @param from The source vector.
 */
void vect3_copy(n_vect3 *to, n_vect3 *from) {
    to->x = from->x;
    to->y = from->y;
    to->z = from->z;
}

/**
 * Populates a 3D vector with given x, y, and z values.
 * @param value The vector to populate.
 * @param x The x value.
 * @param y The y value.
 * @param z The z value.
 */
void vect3_populate(n_vect3 *value, n_double x, n_double y, n_double z) {
    value->x = x;
    value->y = y;
    value->z = z;
}

/**
 * Checks if a 3D vector is non-zero.
 * @param nonzero The vector to check.
 * @return 1 if non-zero, 0 otherwise.
 */
n_int vect3_nonzero(n_vect3 *nonzero) {
    return (nonzero->x != 0) || (nonzero->y != 0) || (nonzero->z != 0);
}

/**
 * Converts a memory list of 2D vectors to an array.
 * @param list The memory list.
 * @param number The number of vectors.
 * @return The resulting array.
 */
n_array *vect2_memory_list_number_array(memory_list *list, n_int number) {
    n_array *array = NULL;

    if (!list || !number) return NULL;

    if (list->count) {
        for (n_int count = 0; count < list->count; count++) {
            n_vect2 *vects = (n_vect2 *)list->data;
            array_add_empty(&array, array_array(object_vect2_pointer(&vects[count * number], number)));
        }
    }
    return array;
}

/**
 * Unwraps a number of 2D vectors from an array.
 * @param array The array to unwrap.
 * @param entry The resulting vectors.
 * @param number The number of vectors.
 * @return 1 if successful, 0 otherwise.
 */
n_int vect2_unwrap_number(n_array *array, n_vect2 *entry, n_int number) {
    n_int out_value = 0;

    if (array) {
        memory_list *elements = object_list_vect2(array);
        if (elements->count == number) {
            n_vect2 *vect = (n_vect2 *)elements->data;
            for (n_int count = 0; count < number; count++) {
                entry[count].data[0] = vect[count].data[0];
                entry[count].data[1] = vect[count].data[1];
            }
            out_value = 1;
        }
        memory_list_free(&elements);
    }
    return out_value;
}

/**
 * Unwraps a number of 2D vectors from an array using a buffer.
 * @param pass_through The array to unwrap.
 * @param buffer The buffer to store the vectors.
 * @param number The number of vectors.
 * @return 1 if successful, 0 otherwise.
 */
n_int vect2_unwrap_number_entry(n_string pass_through, n_byte *buffer, n_int number) {
    return vect2_unwrap_number((n_array *)pass_through, (n_vect2 *)buffer, number);
}

/**
 * Unwraps four 2D vectors from an array.
 * @param pass_through The array to unwrap.
 * @param buffer The buffer to store the vectors.
 * @return 1 if successful, 0 otherwise.
 */
n_int vect2_unwrap_quad(n_string pass_through, n_byte *buffer) {
    return vect2_unwrap_number_entry(pass_through, buffer, 4);
}

/**
 * Unwraps two 2D vectors from an array.
 * @param pass_through The array to unwrap.
 * @param buffer The buffer to store the vectors.
 * @return 1 if successful, 0 otherwise.
 */
n_int vect2_unwrap_line(n_string pass_through, n_byte *buffer) {
    return vect2_unwrap_number_entry(pass_through, buffer, 2);
}
