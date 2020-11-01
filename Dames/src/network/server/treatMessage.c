#include "treatMessage.h"
#include "sendMessage.h"
#include "../common/codeMessages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool server_treat_message(ServerState* serverState, int socket)
{
	bool result;
	uint8_t messageType;
	fread(&messageType, 1, 1, socket);
	switch (messageType)
	{
	case CM_CREATION_DE_COMPTE:
		result = server_treat_create_account(serverState, socket);
		break;
	case CM_CONNEXION_AU_COMPTE:
		result = server_treat_login_account(serverState, socket);
		break;
	case CM_CONNEXION_MODE_INVITE:
		result = server_treat_login_guest(serverState, socket);
		break;
	case CM_CREATION_PARTIE:
		result = server_treat_create_game(serverState, socket);
		break;
	case CM_CONNEXION_PARTIE:
		result = server_treat_join_game(serverState, socket);
		break;
	case CM_CONNEXION_SPECTATEUR:
		result = server_treat_spectate(serverState, socket);
		break;
	case CM_RECHERCHE:
		result = server_treat_search(serverState, socket);
		break;
	case CM_TOUR:
		result = server_treat_execute_turn(serverState, socket);
		break;
	case CM_REQ_SYNC:
		result = server_treat_req_sync(serverState, socket);
		break;
	case CM_PING:
		result = server_treat_ping(serverState, socket);
		break;
	case CM_PONG:
		result = true;
		break;
	default:
		result = send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
		break;
	}
	if (!result)
		return send_message_error(socket, ERROR_GENERIQUE);
	return true;
}


bool server_treat_create_account(ServerState* serverState, int socket)
{
	uint8_t usernameLength, passwordLength;
	char* username;
	char* password;

	fread(&usernameLength, 1, 1, socket);
	fread(&passwordLength, 1, 1, socket);

	if (usernameLength == 0 || passwordLength == 0)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);


	username = (char*)malloc(1 + usernameLength);
	if (username == NULL)
		return false;
	password = (char*)malloc(1 + passwordLength);
	if (password == NULL)
	{
		free(username);
		return false;
	}
	fread(username, 1, usernameLength, socket);
	fread(password, 1, passwordLength, socket);
	username[usernameLength] = 0;
	password[passwordLength] = 0;

	if (server_is_username_taken(serverState, username))
	{
		free(username);
		free(password);
		return send_message_error(socket, ERROR_NOM_UTILISE);
	}
	
	if (!server_add_account(serverState, username, password, socket))
	{
		free(username);
		free(password);
		return false;
	}
	return send_message_ok(socket);
}

bool server_treat_login_account(ServerState* serverState, int socket)
{
	uint8_t usernameLength, passwordLength;
	char* username;
	char* password;

	fread(&usernameLength, 1, 1, socket);
	fread(&passwordLength, 1, 1, socket);

	if (usernameLength == 0 || passwordLength == 0)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);


	username = (char*)malloc(1 + usernameLength);
	if (username == NULL)
		return false;
	password = (char*)malloc(1 + passwordLength);
	if (password == NULL)
	{
		free(username);
		return false;
	}
	fread(username, 1, usernameLength, socket);
	fread(password, 1, passwordLength, socket);
	username[usernameLength] = 0;
	password[passwordLength] = 0;
	
	if (!server_check_username_password(serverState, username, password))
	{
		free(username);
		free(password);
		return send_message_error(socket, ERROR_CONNEXION);
	}
	free(password);
	if (!server_connect(serverState, username, false, socket))
	{
		free(username);
		return false;
	}
	free(username);
	return send_message_ok(socket);
}

bool server_treat_login_guest(ServerState* serverState, int socket)
{
	char username[20];
	memset(username, 0x00, 20);
	sprintf(username, "invite #%d", socket);
	if (!server_connect(serverState, username, true, socket))
		return false;
	return send_message_guest_name(socket, username);
}

bool server_treat_create_game(ServerState* serverState, int socket)
{
	if (server_create_new_game(serverState, socket))
		return send_message_ok(socket);
	return false;
}

bool server_treat_join_game(ServerState* serverState, int socket)
{
	uint8_t game_id;
	uint8_t mapBuffer[50];
	fread(&game_id, 1, 1, socket);
	GameState* game = server_join_game(serverState, socket, game_id);
	if (game == NULL)
		return send_message_error(socket, ERROR_PARTIE_NON_TROUVE);
	if (game->player2->socket != socket)
		return send_message_error(socket, ERROR_PARTIE_EN_COURS);
	if (!send_message_ok(socket))
		return false;
	map_serialize(game->map, mapBuffer, 50);
	return send_message_sync(game->player1->socket, game->turn, mapBuffer);
}

bool server_treat_spectate(ServerState* serverState, int socket)
{
	uint8_t game_id;
	uint8_t mapBuffer[50];
	fread(&game_id, 1, 1, socket);
	GameState* game = server_spectate_game(serverState, socket, game_id);
	if (game == NULL)
		return send_message_error(socket, ERROR_PARTIE_NON_TROUVE);
	map_serialize(game->map, mapBuffer, 50);
	return send_message_sync(socket, game->turn, mapBuffer);
}

bool server_treat_search(ServerState* serverState, int socket)
{
	char gameName[UINT8_MAX];
	int i;
	uint8_t nbGames;
	struct game* games = (struct game*)malloc(sizeof(struct game) * serverState->gamesSize);
	for(i = 0; i < serverState->gamesSize; i++)
		if (serverState->games[i] != NULL)
		{
			memset(gameName, 0x00, UINT8_MAX);
			games[nbGames].id = i;
			if (serverState->games[i]->player2 != NULL)
			{
				sprintf(gameName, "%s vs %s", serverState->games[i]->player1->username, serverState->games[i]->player2->username);
				games[nbGames].status = STATUS_ENCOURS;
			}
			else
			{
				sprintf(gameName, "%s", serverState->games[i]->player1->username);
				games[nbGames].status = STATUS_ATTENTE;
			}
			games[nbGames].name = gameName;
			games[nbGames].name_length = strlen(gameName);
			nbGames++;
		}
	return send_message_list_games(socket, games, nbGames);
}

bool server_treat_execute_turn(ServerState* serverState, int socket)
{
	int i;
	uint8_t nbPositions;
	uint8_t* positions;
	GameState* game = server_get_game(serverState, socket);
	fread(&nbPositions, 1, 1, socket);
	if (positions == 0)
		return false;
	positions = (uint8_t*)malloc(nbPositions);
	if (positions == NULL)
		return false;
	fread(positions, 1, nbPositions, socket);
	if (game == NULL || game->player2 == NULL)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
	if (game->turn == 1 && game->player1->socket != socket)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
	if (game->turn == 2 && game->player2->socket != socket)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
	if (!map_turn(game->map, positions, nbPositions))
		return send_message_error(socket, ERROR_COUP_NON_VALIDE);
	if (game->turn == 1)
	{
		send_message_execute_turn(game->player2->socket, positions, nbPositions);
		game->turn == 2;
	}
	else
	{
		send_message_execute_turn(game->player1->socket, positions, nbPositions);
		game->turn = 1;
	}
	for (i = 0; i < MAX_SPECTATORS; i++)
		if (game->spectators[i] != NULL)
			send_message_execute_turn(game->spectators[i]->socket, positions, nbPositions);
	return send_message_ok(socket);
}

bool server_treat_req_sync(ServerState* serverState, int socket)
{
	uint8_t mapBuffer[50];
	GameState* game = server_get_game(serverState, socket);
	if (game == NULL)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
	map_serialize(game->map, mapBuffer, 50);
	return send_message_sync(socket, game->turn, mapBuffer);
}

bool server_treat_ping(ServerState* serverState, int socket)
{
	return send_message_pong(socket);
}
