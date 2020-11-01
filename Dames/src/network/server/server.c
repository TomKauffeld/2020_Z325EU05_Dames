#include "server.h"
#include <stdlib.h>
#include <string.h>

ServerState* server_init()
{
	ServerState* serverState = (ServerState*)malloc(sizeof(ServerState));
	if (serverState == NULL)
		return NULL;
	serverState->accounts = NULL;
	serverState->logins = NULL;
	serverState->games = NULL;
	serverState->nbAccounts = 0;
	serverState->loginsSize = 0;
	serverState->gamesSize = 0;
	return serverState;
}

Login* server_get_login(ServerState* serverState, int socket)
{
	int i;
	for (i = 0; i < serverState->loginsSize; i++)
		if (serverState->logins[i] != NULL && serverState->logins[i]->socket == socket)
			return serverState->logins[i];
	return NULL;
}

int server_get_game_index(ServerState* serverState, int socket)
{
	int i, j;
	for (i = 0; i < serverState->gamesSize; i++)
		if (serverState->games[i] != NULL)
			if (serverState->games[i]->player1->socket == socket || (serverState->games[i]->player2 != NULL && serverState->games[i]->player2->socket == socket))
				return i;
			else
				for (j = 0; j < MAX_SPECTATORS; j++)
					if (serverState->games[i]->spectators[j] != NULL && serverState->games[i]->spectators[j]->socket == socket)
						return serverState->games[i];
	return -1;
}

GameState* server_get_game(ServerState* serverState, int socket)
{
	int index = server_get_game_index(serverState, socket);
	if (index < 0)
		return NULL;
	return serverState->games[index];
}

GameState* server_join_game(ServerState* serverState, int socket, uint8_t game_id)
{
	Login* login = server_get_login(serverState, socket);
	if (server_get_game_index(serverState, socket) >= 0)
		return NULL;
	if (login == NULL)
		return NULL;
	if (game_id >= serverState->gamesSize)
		return NULL;
	if (serverState->games[game_id] == NULL)
		return NULL;
	if (serverState->games[game_id]->player2 == NULL)
		serverState->games[game_id]->player2 = login;
	return serverState->games[game_id];
}

GameState* server_spectate_game(ServerState* serverState, int socket, uint8_t game_id)
{
	int i;
	Login* login = server_get_login(serverState, socket);
	if (server_get_game_index(serverState, socket) >= 0)
		return NULL;
	if (login == NULL)
		return NULL;
	if (game_id >= serverState->gamesSize)
		return NULL;
	if (serverState->games[game_id] == NULL)
		return NULL;
	if (serverState->games[game_id]->player2 == NULL)
		return NULL;
	for (i = 0; i < MAX_SPECTATORS; i++)
		if (serverState->games[game_id]->spectators[i] == NULL)
		{
			serverState->games[game_id]->spectators[i] = login;
			return serverState->games[game_id];
		}
	return NULL;
}

void server_remove_login(ServerState* serverState, int socket, server_on_end_game on_end_game)
{
	int i, j;
	for (i = 0; i < serverState->gamesSize; i++)
		if (serverState->games[i] != NULL)
		{
			if (serverState->games[i]->player1->socket == socket || (serverState->games[i]->player2 != NULL && serverState->games[i]->player2->socket == socket))
			{
				if (serverState->games[i]->player1->socket == socket && serverState->games[i]->player2 != NULL)
					on_end_game(serverState->games[i]->player2);
				else if (serverState->games[i]->player2 != NULL && serverState->games[i]->player2->socket == socket)
					on_end_game(serverState->games[i]->player1);
				for (j = 0; j < MAX_SPECTATORS; j++)
					if (serverState->games[i]->spectators[j] != NULL)
						on_end_game(serverState->games[i]->spectators[j]);
				free(serverState->games[i]);
				serverState->games[i] = NULL;
			}
			else 
			{
				for (j = 0; j < MAX_SPECTATORS; j++)
					if (serverState->games[i]->spectators[j] != NULL && serverState->games[i]->spectators[j]->socket == socket)
						serverState->games[i]->spectators[j] = NULL;
			}
		}

	for (i = 0; i < serverState->loginsSize; i++)
		if (serverState->logins[i] != NULL && serverState->logins[i]->socket == socket)
		{
			login_destroy(serverState->logins[i]);
			serverState->logins[i] = NULL;
			break;
		}
}

bool server_is_username_taken(ServerState* serverState, char* username)
{
	int i;
	for (i = 0; i < serverState->nbAccounts; i++)
		if (strcmp(serverState->accounts[i].username, username) == 0)
			return true;
	return false;
}

bool server_add_account(ServerState* serverState, char* username, char* password, int socket)
{
	Login* login = server_get_login(serverState, socket);
	BYTE hash[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, password, strlen(password));
	sha256_final(&ctx, hash);
	Account* tmpAccounts;
	if (login != NULL)
		return false;
	if (serverState->accounts != NULL)
		tmpAccounts = (Account*)realloc(serverState->accounts, (serverState->nbAccounts + 1) * sizeof(Account));
	else
		tmpAccounts = (Account*)malloc(sizeof(Account));
	if (tmpAccounts == NULL)
		return false;
	memcpy(tmpAccounts[serverState->nbAccounts].password, hash, SHA256_BLOCK_SIZE);
	tmpAccounts[serverState->nbAccounts].username = username;
	serverState->accounts = tmpAccounts;
	serverState->nbAccounts++;
	return server_connect(serverState, username, false, socket);
}

bool server_connect(ServerState* serverState, char* username, bool isGuest, int socket)
{
	Login* login = server_get_login(serverState, socket);
	Login** tmpLogins;
	int free_index = -1;
	int i;
	if (login != NULL)
		return false;
	for (i = 0; i < serverState->loginsSize; i++)
		if (serverState->logins[i] == NULL)
		{
			free_index = i;
			break;
		}
	if (free_index < 0)
	{
		if (serverState->logins != NULL)
			tmpLogins = (Login**)realloc(serverState->logins, (serverState->loginsSize + 1) * sizeof(Login*));
		else
			tmpLogins = (Login**)malloc(sizeof(Login*));
		if (tmpLogins == NULL)
			return false;
		serverState->logins = tmpLogins;
		free_index = serverState->loginsSize++;
	}
	serverState->logins[free_index] = login_init(username, isGuest, socket);
	return serverState->logins[free_index] != NULL;
}

bool server_check_username_password(ServerState* serverState, char* username, char* password)
{
	BYTE hash[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, password, strlen(password));
	sha256_final(&ctx, hash);
	int i;
	for (i = 0; i < serverState->nbAccounts; i++)
		if (strcmp(serverState->accounts[i].username, username) == 0)
			return memcmp(serverState->accounts[i].password, hash, SHA256_BLOCK_SIZE) == 0;
	return false;
}

bool server_create_new_game(ServerState* serverState, int socket)
{
	Login* login = server_get_login(serverState, socket);
	int index = server_get_game_index(serverState, socket);
	GameState** tmpGames;
	GameState* tmpGame;
	int free_index = -1;
	int i;
	if (index >= 0)
		return false;
	if (login == NULL)
		return false;
	for (i = 0; i < serverState->gamesSize; i++)
		if (serverState->games[i] == NULL)
		{
			free_index = i;
			break;
		}
	if (free_index < 0)
	{
		if (serverState->games != NULL)
			tmpGames = (GameState**)realloc(serverState->games, (serverState->gamesSize + 1) * sizeof(GameState*));
		else
			tmpGames = (GameState**)malloc(sizeof(GameState*));
		if (tmpGames == NULL)
			return false;
		serverState->games = tmpGames;
		free_index = serverState->gamesSize++;
	}
	tmpGame = (GameState*)malloc(sizeof(GameState));
	if (tmpGame == NULL)
		return false;
	tmpGame->map = map_init();
	if (tmpGame->map == NULL)
	{
		free(tmpGame);
		return false;
	}
	tmpGame->player1 = login;
	tmpGame->player2 = NULL;
	tmpGame->turn = 1;
	for (i = 0; i < MAX_SPECTATORS; i++)
		tmpGame->spectators[i] = NULL;
	serverState->games[free_index] = tmpGame;
	return true;
}

void server_destroy(ServerState* serverState)
{
	int i;
	if (serverState->logins != NULL)
	{
		for (i = 0; i < serverState->loginsSize; i++)
			if (serverState->logins[i] != NULL)
				login_destroy(serverState->logins[i]);
		free(serverState->logins);
	}
	if (serverState->games != NULL)
	{
		for (i = 0; i < serverState->gamesSize; i++)
			if (serverState->games[i] != NULL)
				free(serverState->games[i]);
		free(serverState->games);
	}
	if (serverState->accounts != NULL)
	{
		for (i = 0; i < serverState->nbAccounts; i++)
			free(serverState->accounts[i].username);
		free(serverState->accounts);
	}
	free(serverState);
}
