#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include <inttypes.h>
#include "../utils/bool.h"
#include "../utils/vector.h"

#define EMPTY 0x00

#define PLAYER 0xF0
#define PIECE 0x0F

#define PLAYER_1 0x10
#define PLAYER_2 0x20

#define MEN 0x01
#define KING 0x02

#define PLAYER_1_MEN (PLAYER_1 | MEN)
#define PLAYER_1_KING (PLAYER_1 | KING)

#define PLAYER_2_MEN (PLAYER_2 | MEN)
#define PLAYER_2_KING (PLAYER_2 | KING)

typedef uint8_t Map;


/// <summary>
/// Creates a new map
/// </summary>
/// <returns>a new map in the initial state</returns>
Map* map_init();

/// <summary>
/// serializes the map into a buffer
/// </summary>
/// <param name="map">the map to serialize</param>
/// <param name="buffer">the buffer</param>
/// <param name="bufferSize">the buffer size (it should be atleast 50 bytes long)</param>
/// <returns>the amount of bytes used (0 if an error occured)</returns>
unsigned long map_serialize(Map* map, void* buffer, unsigned long bufferSize);

/// <summary>
/// unserialized a map from a buffer
/// </summary>
/// <param name="buffer">the buffer</param>
/// <param name="bufferSize">the buffer size (it should be atleast 50 bytes long)</param>
/// <returns>the map that was serialized (or NULL if an error occured)</returns>
Map* map_unserialize(void* buffer, unsigned long bufferSize);

/// <summary>
/// Converts a location (used by the map system) to an vector
/// </summary>
/// <param name="location">the location to convert</param>
/// <returns>a vector representing the location</returns>
Vector map_location_to_vector(uint8_t location);

/// <summary>
/// Converts a vector to a location (used by the map system)
/// </summary>
/// <param name="pos">the vector to convert</param>
/// <returns>the location, or 0 if the vector is an invalid position</returns>
uint8_t map_vector_to_location(Vector pos);

/// <summary>
/// validates a turn (multiple moves) and returns the captures this move would make
/// </summary>
/// <param name="map">the map pointer</param>
/// <param name="positions">a list of positions</param>
/// <param name="nbPositions">the number of positions in the list</param>
/// <param name="captures">(out) a buffer for the list of captures</param>
/// <param name="maxCaptures">the max the buffer can hold</param>
/// <param name="nbCaptures">(out) a pointer to hold the number of captures made</param>
/// <returns>returns true if the turn is valid, on error and/or invalid moves, it returns false</returns>
bool map_validate_turn(Map* map, uint8_t* positions, uint8_t nbPositions, uint8_t* captures, uint8_t maxCaptures, uint8_t* nbCaptures);

/// <summary>
/// validates a turn (multiple moves) and returns the captures this move would make
/// </summary>
/// <param name="map">the map pointer</param>
/// <param name="positions">a list of positions</param>
/// <param name="nbPositions">the number of positions in the list</param>
/// <param name="captures">(out) a buffer for the list of captures</param>
/// <param name="maxCaptures">the max the buffer can hold</param>
/// <param name="nbCaptures">(out) a pointer to hold the number of captures made</param>
/// <returns>returns true if the move is valid, on error and/or invalid moves, it returns false</returns>
bool map_validate_move(Map* map, uint8_t from, uint8_t to, uint8_t* captures, uint8_t maxCaptures, uint8_t* nbCaptures);

/// <summary>
/// executes a turn (multiple moves)
/// </summary>
/// <param name="map">the map pointer</param>
/// <param name="positions">a list of positions</param>
/// <param name="nbPositions">the number of positions in the list</param>
/// <returns>true if the turn is executes, false otherwise</returns>
bool map_turn(Map* map, uint8_t* positions, uint8_t nbPositions);

/// <summary>
/// frees the memory used by the map
/// </summary>
/// <param name="map">the map pointer</param>
void map_destroy(Map* map);

#endif