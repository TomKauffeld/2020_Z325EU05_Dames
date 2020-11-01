#ifndef __UTILS_VECTOR_H__
#define __UTILS_VECTOR_H__

#include <inttypes.h>

typedef struct vector {
	int x;
	int y;
} Vector;

/// <summary>
/// multiplies a vector with a number
/// </summary>
/// <param name="a">a vector</param>
/// <param name="b">a number</param>
/// <returns>a new vector with the value: A * b</returns>
Vector vector_mult(Vector a, int b);

/// <summary>
/// adds two vectors together
/// </summary>
/// <param name="a">the first vector</param>
/// <param name="b">the second vector</param>
/// <returns>a new vector with the value: A + B</returns>
Vector vector_add(Vector a, Vector b);

/// <summary>
/// substracts a vector from another
/// </summary>
/// <param name="a">the first vector</param>
/// <param name="b">the second vector</param>
/// <returns>a new vector with the value: A - B</returns>
Vector vector_min(Vector a, Vector b);

/// <summary>
/// returns the absolute distance between two vectors as a vector (distance x, and distance y)
/// </summary>
/// <param name="a">the first vector</param>
/// <param name="b">the second vector</param>
/// <returns>a new vector</returns>
Vector vector_distance(Vector a, Vector b);

/// <summary>
/// returns a vector where the values are 1, -1 or 0 depending if the vector given has a positive value or a negative value (or zero)
/// </summary>
/// <param name="a">a vector</param>
/// <returns>a new vector</returns>
Vector vector_get_unit(Vector a);

#endif
