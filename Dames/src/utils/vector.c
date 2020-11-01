#include "vector.h"

/// <summary>
/// multiplies a vector with a number
/// </summary>
/// <param name="a">a vector</param>
/// <param name="b">a number</param>
/// <returns>a new vector with the value: A * b</returns>
Vector vector_mult(Vector a, int b)
{
	Vector result;
	result.x = a.x * b;
	result.y = a.y * b;
	return result;
}

/// <summary>
/// adds two vectors together
/// </summary>
/// <param name="a">the first vector</param>
/// <param name="b">the second vector</param>
/// <returns>a new vector with the value: A + B</returns>
Vector vector_add(Vector a, Vector b)
{
	Vector result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

/// <summary>
/// substracts a vector from another
/// </summary>
/// <param name="a">the first vector</param>
/// <param name="b">the second vector</param>
/// <returns>a new vector with the value: A - B</returns>
Vector vector_min(Vector a, Vector b)
{
	Vector result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

/// <summary>
/// returns the absolute distance between two vectors as a vector (distance x, and distance y)
/// </summary>
/// <param name="a">the first vector</param>
/// <param name="b">the second vector</param>
/// <returns>a new vector</returns>
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

/// <summary>
/// returns a vector where the values are 1, -1 or 0 depending if the vector given has a positive value or a negative value (or zero)
/// </summary>
/// <param name="a">a vector</param>
/// <returns>a new vector</returns>
Vector vector_get_unit(Vector a)
{
	Vector result;
	if (a.x > 0)
		result.x = 1;
	else if (a.x < 0)
		result.x = -1;
	else
		result.x = 0;
	if (a.y > 0)
		result.y = 1;
	else if (a.y < 0)
		result.y = -1;
	else
		result.y = 0;
	return result;
}