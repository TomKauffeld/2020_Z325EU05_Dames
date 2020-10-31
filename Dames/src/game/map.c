#include "map.h"
#include <stdlib.h>
#include <string.h>

Map* map_init() {
	Map* map = (Map*)malloc(50);
	if (map == NULL)
		return NULL;
	memset(map, PLAYER_1_MEN, 20);
	memset(map + 20, EMPTY, 10);
	memset(map + 30, PLAYER_2_MEN, 20);
	return map;
}

Vector map_location_to_vector(uint8_t location)
{
	Vector pos;
	pos.y = (location - 1) / 5;
	pos.x = ((location - 1) % 5) * 2 + 1 - pos.y % 2;
	return pos;
}

uint8_t map_vector_to_location(Vector pos)
{
	if (pos.y % 2 == pos.x % 2)
		return 0;
	return pos.y * 5 + pos.x / 2 + 1;
}


bool map_validate_move(Map* map, uint8_t from, uint8_t to, uint8_t * captures, uint8_t maxCaptures, uint8_t * nbCaptures)
{
	Vector posFrom, posTo, dist, diff, tmp;
	uint8_t location;
	if (captures == NULL && maxCaptures > 0)
		return false;
	if (from < 1 || from > 50)
		return false;
	if (to < 1 || to > 50)
		return false;
	if (map[from - 1] == EMPTY)
		return false;
	if (map[to - 1] != EMPTY)
		return false;
	if (nbCaptures != NULL)
		(*nbCaptures) = 0;

	posFrom = map_location_to_vector(from);
	posTo = map_location_to_vector(to);
	dist = vector_distance(posFrom, posTo);
	diff = vector_min(posTo, posFrom);

	if (dist.x != dist.y)
		return false;
	if (map[from - 1] & MEN == MEN)
	{
		if (dist.x == 1)
		{
			if (map[from - 1] == PLAYER_2_MEN && diff.y > 0)
				return false;
			if (map[from - 1] == PLAYER_1_MEN && diff.y < 0)
				return false;
		}
		else if (dist.x == 2)
		{
			tmp.x = diff.x / 2;
			tmp.y = diff.y / 2;
			tmp = vector_add(posFrom, tmp);
			location = map_vector_to_location(tmp);
			if (map[location - 1] == EMPTY)
				return false;
			if (map[location - 1] & PLAYER == map[from - 1] & PLAYER)
				return false;
		}
		else
			return false;

	}


	return true;
}

bool map_move(Map* map, uint8_t from, uint8_t to, uint8_t* captures, uint8_t maxCaptures, uint8_t* nbCaptures)
{
	if (!map_validate_move(map, from, to, NULL, 0, NULL))
		return false;

	map[to - 1] = map[from - 1];
	map[from - 1] = 0;

	if ((map[to - 1] & PLAYER_2_MEN > 0) && to <= 5)
		map[to - 1] = PLAYER_2_KING;
	else if ((map[to - 1] & PLAYER_1_MEN > 0) && to >= 46)
		map[to - 1] = PLAYER_1_KING;

	return true;
}




void map_destroy(Map* map)
{
	free(map);
}
