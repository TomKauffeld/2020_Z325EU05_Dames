#include "vector.h"

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
