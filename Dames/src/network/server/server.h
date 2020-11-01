#ifndef __NETWORK_SERVER_SERVER_H__
#define __NETWORK_SERVER_SERVER_H__

#include "../../utils/bool.h"
#include "../../utils/sha256.h"
#include "../../game/map.h"
#include "login.h"

#define MAX_SPECTATORS 20

typedef void(*server_on_end_game(Login*));

typedef struct account {
	char* username;
	BYTE password[SHA256_BLOCK_SIZE];
} Account;


typedef struct gameState {
	Login* player1;
	Login* player2;
	Map* map;
	uint8_t turn;
	Login* spectators[MAX_SPECTATORS];
} GameState;

typedef struct serverState {
	int nbAccounts;
	int loginsSize;
	int gamesSize;
	Account* accounts;
	Login** logins;
	GameState** games;
} ServerState;

ServerState * server_init();

Login* server_get_login(ServerState* serverState, int socket);

int server_get_game_index(ServerState* serverState, int socket);

GameState* server_get_game(ServerState* serverState, int socket);

GameState* server_join_game(ServerState* serverState, int socket, uint8_t game_id);

GameState* server_spectate_game(ServerState* serverState, int socket, uint8_t game_id);

void server_remove_login(ServerState* serverState, int socket, server_on_end_game on_end_game);

bool server_is_username_taken(ServerState* serverState, char* username);

bool server_add_account(ServerState* serverState, char* username, char* password, int socket);

bool server_connect(ServerState* serverState, char* username, bool isGuest, int socket);

bool server_check_username_password(ServerState* serverState, char* username, char* password);

bool server_create_new_game(ServerState* serverState, int socket);

void server_destroy(ServerState* serverState);


#endif
