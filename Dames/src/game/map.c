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

bool map_validate_move(Map* map, uint8_t from, uint8_t to)
{
	if (from < 1 || from > 50)
		return false;
	if (to < 1 || to > 50)
		return false;
	if (map[from - 1] == EMPTY)
		return false;
	if (map[to - 1] != EMPTY)
		return false;
	return true;
}

bool map_move(Map* map, uint8_t from, uint8_t to)
{
	if (!map_validate_move(map, from, to))
		return false;
	map[to - 1] = map[from - 1];
	map[from - 1] = 0;
	return true;
}




void map_destroy(Map* map)
{
	free(map);
}
