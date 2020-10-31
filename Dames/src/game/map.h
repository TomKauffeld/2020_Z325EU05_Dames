#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include <inttypes.h>
#include "../utils/bool.h"

#define EMPTY 0x00

#define PLAYER_1 0x10
#define PLAYER_2 0x20

#define MEN 0x01
#define KING 0x02

#define PLAYER_1_MEN (PLAYER_1 | MEN)
#define PLAYER_1_KING (PLAYER_1 | KING)

#define PLAYER_2_MEN (PLAYER_2 | MEN)
#define PLAYER_2_KING (PLAYER_2 | KING)

typedef uint8_t Map;

Map* map_init();

bool map_validate_move(Map* map, uint8_t from, uint8_t to);

bool map_move(Map* map, uint8_t from, uint8_t to);

void map_destroy(Map* map);


#endif