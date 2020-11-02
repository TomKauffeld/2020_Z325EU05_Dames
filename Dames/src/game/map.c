#include "map.h"
#include <stdlib.h>
#include <string.h>


#define MAP_SIZE 50
#define MAX_CAPTURES 50

/// <summary>
/// Creates a new map
/// </summary>
/// <returns>a new map in the initial state</returns>
Map* map_init() {
	Map* map = (Map*)malloc(MAP_SIZE);
	if (map == NULL)
		return NULL;
	memset(map, PLAYER_1_MEN, 20);
	memset(map + 20, EMPTY, 10);
	memset(map + 30, PLAYER_2_MEN, 20);
	return map;
}

/// <summary>
/// serializes the map into a buffer
/// </summary>
/// <param name="map">the map to serialize</param>
/// <param name="buffer">the buffer</param>
/// <param name="bufferSize">the buffer size (it should be atleast 50 bytes long)</param>
/// <returns>the amount of bytes used (0 if an error occured)</returns>
unsigned long map_serialize(Map * map, void* buffer, unsigned long bufferSize)
{
	if (bufferSize < MAP_SIZE)
		return 0;
	memcpy(buffer, map, MAP_SIZE);
	return MAP_SIZE;
}

/// <summary>
/// unserialized a map from a buffer
/// </summary>
/// <param name="buffer">the buffer</param>
/// <param name="bufferSize">the buffer size (it should be atleast 50 bytes long)</param>
/// <returns>the map that was serialized (or NULL if an error occured)</returns>
Map* map_unserialize(void* buffer, unsigned long bufferSize)
{
	Map* map;
	int i;
	if (bufferSize < MAP_SIZE)
		return NULL;
	for (i = 0; i < MAP_SIZE; i++)
	{
		switch (((uint8_t*)buffer)[i])
		{
		case EMPTY:
		case PLAYER_1_KING:
		case PLAYER_1_MEN:
		case PLAYER_2_KING:
		case PLAYER_2_MEN:
			break;
		default:
			return NULL;
		}
	}
	map = (Map*)malloc(MAP_SIZE);
	if (map == NULL)
		return NULL;
	memcpy(map, buffer, MAP_SIZE);
	return map;
}

uint8_t map_get_symbole(Map* map, uint8_t location)
{
	if (location < 1 || location > MAP_SIZE)
		return 0;
	return map[location - 1];
}

/// <summary>
/// Converts a location (used by the map system) to an vector
/// </summary>
/// <param name="location">the location to convert</param>
/// <returns>a vector representing the location</returns>
Vector map_location_to_vector(uint8_t location)
{
	Vector pos;
	pos.y = (location - 1) / 5;
	pos.x = ((location - 1) % 5) * 2 + 1 - pos.y % 2;
	return pos;
}

/// <summary>
/// Converts a vector to a location (used by the map system)
/// </summary>
/// <param name="pos">the vector to convert</param>
/// <returns>the location, or 0 if the vector is an invalid position</returns>
uint8_t map_vector_to_location(Vector pos)
{
	uint8_t location;
	if (pos.y % 2 == pos.x % 2)
		return 0;
	if (pos.y < 0 || pos.x < 0 || pos.x >= 10 || pos.y >= 10)
		return 0;
	location = pos.y * 5 + pos.x / 2 + 1;
	if (location < 1 || location > MAP_SIZE)
		return 0;
	return location;
}

/// <summary>
/// validates a turn (multiple moves) and returns the captures this move would make
/// </summary>
/// <param name="map">the map pointer</param>
/// <param name="positions">a list of positions</param>
/// <param name="nbPositions">the number of positions in the list</param>
/// <param name="captures">(out) a buffer for the list of captures</param>
/// <param name="maxCaptures">the max the buffer can hold</param>
/// <param name="nbCaptures">(out) a pointer to hold the number of captures made</param>
/// <returns>returns TRUE if the turn is valid, on error and/or invalid moves, it returns FALSE</returns>
boolean map_validate_turn(Map* map, uint8_t* positions, uint8_t nbPositions, uint8_t* captures, uint8_t maxCaptures, uint8_t* nbCaptures)
{
	uint8_t tmpCaptures, i, j, k;
	boolean result;
	if (nbPositions < 2)
		return FALSE;
	*nbCaptures = 0;

	for (i = 0; i < nbPositions - 1; i++)
	{
		for (j = 0; j < *nbCaptures; j++)
			if (captures[j] == positions[i + 1])
				return FALSE;
		result = map_validate_move(map, positions[i], positions[i + 1], captures + *nbCaptures, MAX_CAPTURES - *nbCaptures, &tmpCaptures);
		if (!result)
			return FALSE;
		for (j = 0; j < *nbCaptures; j++)
			for (k = *nbCaptures; k < *nbCaptures + tmpCaptures; k++)
				if (captures[j] == captures[k])
					return FALSE;
		*nbCaptures += tmpCaptures;
	}
	return TRUE;
}

/// <summary>
/// validates the 
/// </summary>
/// <param name="map"></param>
/// <param name="from"></param>
/// <param name="to"></param>
/// <param name="captures"></param>
/// <param name="maxCaptures"></param>
/// <param name="nbCaptures"></param>
/// <returns></returns>
boolean map_validate_move(Map* map, uint8_t from, uint8_t to, uint8_t * captures, uint8_t maxCaptures, uint8_t * nbCaptures)
{
	int i;
	Vector posFrom, posTo, dist, diff, tmp, offset;
	uint8_t location;
	if (captures == NULL && maxCaptures > 0)
		return FALSE;
	if (nbCaptures == NULL && maxCaptures > 0)
		return FALSE;
	if (from < 1 || from > MAP_SIZE)
		return FALSE;
	if (to < 1 || to > MAP_SIZE)
		return FALSE;
	if (map[from - 1] == EMPTY)
		return FALSE;
	if (map[to - 1] != EMPTY)
		return FALSE;
	if (nbCaptures != NULL)
		(*nbCaptures) = 0;

	posFrom = map_location_to_vector(from);
	posTo = map_location_to_vector(to);
	dist = vector_distance(posFrom, posTo);
	diff = vector_min(posTo, posFrom);

	if (dist.x != dist.y)
		return FALSE;
	if ((map[from - 1] & PIECE) == MEN)
	{
		if (dist.x == 1)
		{
			if (map[from - 1] == PLAYER_2_MEN && diff.y > 0)
				return FALSE;
			if (map[from - 1] == PLAYER_1_MEN && diff.y < 0)
				return FALSE;
		}
		else if (dist.x == 2)
		{
			tmp.x = diff.x / 2;
			tmp.y = diff.y / 2;
			tmp = vector_add(posFrom, tmp);
			location = map_vector_to_location(tmp);
			if (map[location - 1] == EMPTY)
				return FALSE;
			if ((map[location - 1] & PLAYER) == (map[from - 1] & PLAYER))
				return FALSE;

			if (nbCaptures != NULL && *nbCaptures < maxCaptures)
				captures[(*nbCaptures)++] = location;
		}
		else
			return FALSE;
	}
	else
	{
		offset = vector_get_unit(diff);
		
		for (i = 1; i < dist.x; i++)
		{
			tmp = vector_mult(offset, i);
			tmp = vector_add(posFrom, tmp);
			location = map_vector_to_location(tmp);
			if ((map[location - 1] & PLAYER) == (map[from - 1] & PLAYER))
				return FALSE;
			if (map[location - 1] != EMPTY && nbCaptures != NULL && *nbCaptures < maxCaptures)
				captures[(*nbCaptures)++] = location;
		}
	}
	return TRUE;
}

/// <summary>
/// executes a turn (multiple moves)
/// </summary>
/// <param name="map">the map pointer</param>
/// <param name="positions">a list of positions</param>
/// <param name="nbPositions">the number of positions in the list</param>
/// <returns>TRUE if the turn is executes, FALSE otherwise</returns>
boolean map_turn(Map* map, uint8_t* positions, uint8_t nbPositions)
{
	uint8_t captures[MAX_CAPTURES];
	uint8_t nbCaptures = 0;
	uint8_t to, from, i;
	if (!map_validate_turn(map, positions, nbPositions, captures, MAX_CAPTURES, &nbCaptures))
		return FALSE;

	from = positions[0];
	to = positions[nbPositions - 1];

	map[to - 1] = map[from - 1];
	map[from - 1] = 0;

	if ((map[to - 1] & PLAYER_2_MEN) > 0 && to <= 5)
		map[to - 1] = PLAYER_2_KING;
	else if ((map[to - 1] & PLAYER_1_MEN) > 0 && to >= 46)
		map[to - 1] = PLAYER_1_KING;

	for (i = 0; i < nbCaptures; i++)
		map[captures[i] - 1] = EMPTY;

	return TRUE;
}

/// <summary>
/// frees the memory used by the map
/// </summary>
/// <param name="map">the map pointer</param>
void map_destroy(Map* map)
{
	free(map);
}
