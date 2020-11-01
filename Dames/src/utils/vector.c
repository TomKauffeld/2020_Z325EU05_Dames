#include "vector.h"

Vector vector_mult(Vector a, int b)
{
	Vector result;
	result.x = a.x * b;
	result.y = a.y * b;
	return result;
}

Vector vector_add(Vector a, Vector b)
{
	Vector result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

Vector vector_min(Vector a, Vector b)
{
	Vector result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

Vector vector_get_unit(Vector a)
{
	Vector result;
	if (a.x > 0)
		result.x = 1;
	else
		result.x = -1;
	if (a.y > 0)
		result.y = 1;
	else
		result.y = -1;
	return result;
}

Vector vector_distance(Vector a, Vector b)
{
	Vector result;
	if (a.x > b.x)
		result.x = a.x - b.x;
	else
		result.x = b.x - a.x;
	if (a.y > b.y)
		result.y = a.y - b.y;
	else
		result.y = b.y - a.y;
	return result;
}
