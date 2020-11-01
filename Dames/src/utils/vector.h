#ifndef __UTILS_VECTOR_H__
#define __UTILS_VECTOR_H__

#include <inttypes.h>

typedef struct vector {
	int x;
	int y;
} Vector;

Vector vector_mult(Vector a, int b);

Vector vector_add(Vector a, Vector b);

Vector vector_min(Vector a, Vector b);

Vector vector_distance(Vector a, Vector b);

Vector vector_get_unit(Vector a);

#endif
