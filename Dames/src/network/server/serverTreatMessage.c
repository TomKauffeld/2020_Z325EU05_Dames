#include "serverTreatMessage.h"
#include "serverSendMessage.h"
#include "../common/network.h"
#include "../common/codeMessages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void server_treat_on_end_game(Login* login)
{
	send_message_error(login->socket, ERROR_PARTIE_NON_TROUVE);
}

boolean server_treat_message(ServerState* serverState, int socket)
{
	boolean result;
	uint8_t messageType;
	int read = read_socket(&messageType, 1, 1, socket);
	if (read != 1)
	{
		server_remove_login(serverState, socket, server_treat_on_end_game);
		return FALSE;
	}
	switch (messageType)
	{
	case CM_CREATION_DE_COMPTE:
#ifdef _DEBUG
		printf("CM_CREATION_DE_COMPTE\n");
#endif // _DEBUG
		result = server_treat_create_account(serverState, socket);
		break;
	case CM_CONNEXION_AU_COMPTE:
#ifdef _DEBUG
		printf("CM_CONNEXION_AU_COMPTE\n");
#endif // _DEBUG
		result = server_treat_login_account(serverState, socket);
		break;
	case CM_CONNEXION_MODE_INVITE:
#ifdef _DEBUG
		printf("CM_CONNEXION_MODE_INVITE\n");
#endif // _DEBUG
		result = server_treat_login_guest(serverState, socket);
		break;
	case CM_CREATION_PARTIE:
#ifdef _DEBUG
		printf("CM_CREATION_PARTIE\n");
#endif // _DEBUG
		result = server_treat_create_game(serverState, socket);
		break;
	case CM_CONNEXION_PARTIE:
#ifdef _DEBUG
		printf("CM_CONNEXION_PARTIE\n");
#endif // _DEBUG
		result = server_treat_join_game(serverState, socket);
		break;
	case CM_CONNEXION_SPECTATEUR:
#ifdef _DEBUG
		printf("CM_CONNEXION_SPECTATEUR\n");
#endif // _DEBUG
		result = server_treat_spectate(serverState, socket);
		break;
	case CM_RECHERCHE:
#ifdef _DEBUG
		printf("CM_RECHERCHE\n");
#endif // _DEBUG
		result = server_treat_search(serverState, socket);
		break;
	case CM_TOUR:
#ifdef _DEBUG
		printf("CM_TOUR\n");
#endif // _DEBUG
		result = server_treat_execute_turn(serverState, socket);
		break;
	case CM_REQ_SYNC:
#ifdef _DEBUG
		printf("CM_REQ_SYNC\n");
#endif // _DEBUG
		result = server_treat_req_sync(serverState, socket);
		break;
	case CM_PING:
#ifdef _DEBUG
		printf("CM_PING\n");
#endif // _DEBUG
		result = server_treat_ping(serverState, socket);
		break;
	case CM_PONG:
#ifdef _DEBUG
		printf("CM_PONG\n");
#endif // _DEBUG
		result = TRUE;
		break;
	default:
#ifdef _DEBUG
		printf("CM_UNKNOWN\n");
#endif // _DEBUG
		result = send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
		break;
	}
	if (!result)
	{
		result = send_message_error(socket, ERROR_GENERIQUE);
		if (!result)
			server_remove_login(serverState, socket, server_treat_on_end_game);
	}
	return result;
}


boolean server_treat_create_account(ServerState* serverState, int socket)
{
	uint8_t usernameLength, passwordLength;
	char* username;
	char* password;

	read_socket(&usernameLength, 1, 1, socket);
	read_socket(&passwordLength, 1, 1, socket);

	if (usernameLength == 0 || passwordLength == 0)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);


	username = (char*)malloc(1 + usernameLength);
	if (username == NULL)
		return FALSE;
	password = (char*)malloc(1 + passwordLength);
	if (password == NULL)
	{
		free(username);
		return FALSE;
	}
	read_socket(username, 1, usernameLength, socket);
	read_socket(password, 1, passwordLength, socket);
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
		return FALSE;
	}
	return send_message_ok(socket);
}

boolean server_treat_login_account(ServerState* serverState, int socket)
{
	int read;
	uint8_t usernameLength, passwordLength;
	char* username;
	char* password;

	read = read_socket(&usernameLength, 1, 1, socket);
	if (read != 1)
		return FALSE;
	read = read_socket(&passwordLength, 1, 1, socket);
	if (read != 1)
		return FALSE;

	if (usernameLength == 0 || passwordLength == 0)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);


	username = (char*)malloc(1 + usernameLength);
	if (username == NULL)
		return FALSE;
	password = (char*)malloc(1 + passwordLength);
	if (password == NULL)
	{
		free(username);
		return FALSE;
	}
	read = read_socket(username, 1, usernameLength, socket);
	if (read != usernameLength)
		return FALSE;
	read = read_socket(password, 1, passwordLength, socket);
	if (read != passwordLength)
		return FALSE;
	username[usernameLength] = 0;
	password[passwordLength] = 0;
	
	if (!server_check_username_password(serverState, username, password))
	{
		free(username);
		free(password);
		return send_message_error(socket, ERROR_CONNEXION);
	}
	free(password);
	if (!server_connect(serverState, username, FALSE, socket))
	{
		free(username);
		return FALSE;
	}
	free(username);
	return send_message_ok(socket);
}

boolean server_treat_login_guest(ServerState* serverState, int socket)
{
	char username[20];
	memset(username, 0x00, 20);
	sprintf(username, "invite #%d", socket);
	if (!server_connect(serverState, username, TRUE, socket))
		return FALSE;
	return send_message_guest_name(socket, username);
}

boolean server_treat_create_game(ServerState* serverState, int socket)
{
	if (server_create_new_game(serverState, socket))
		return send_message_ok(socket);
	return FALSE;
}

boolean server_treat_join_game(ServerState* serverState, int socket)
{
	int read;
	uint8_t game_id;
	uint8_t mapBuffer[50];
	read = read_socket(&game_id, 1, 1, socket);
	if (read != 1)
		return FALSE;
	GameState* game = server_join_game(serverState, socket, game_id);
	if (game == NULL)
		return send_message_error(socket, ERROR_PARTIE_NON_TROUVE);
	if (game->player2->socket != socket)
		return send_message_error(socket, ERROR_PARTIE_EN_COURS);
	if (!send_message_ok(socket))
		return FALSE;
	map_serialize(game->map, mapBuffer, 50);
	return send_message_sync(game->player1->socket, game->turn, mapBuffer);
}

boolean server_treat_spectate(ServerState* serverState, int socket)
{
	int read;
	uint8_t game_id;
	uint8_t mapBuffer[50];
	read = read_socket(&game_id, 1, 1, socket);
	if (read != 1)
		return FALSE;
	GameState* game = server_spectate_game(serverState, socket, game_id);
	if (game == NULL)
		return send_message_error(socket, ERROR_PARTIE_NON_TROUVE);
	map_serialize(game->map, mapBuffer, 50);
	return send_message_sync(socket, game->turn, mapBuffer);
}

boolean server_treat_search(ServerState* serverState, int socket)
{
	boolean result;
	char gameName[UINT8_MAX];
	int i;
	uint8_t nbGames = 0;
	struct game* games = NULL;
	if (serverState->gamesSize > 0)
	{
		games = (struct game*)malloc(sizeof(struct game) * serverState->gamesSize);
		for (i = 0; i < serverState->gamesSize; i++)
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
	}
	result = send_message_list_games(socket, games, nbGames);
	if (games != NULL)
		free(games);
	return result;
}

boolean server_treat_execute_turn(ServerState* serverState, int socket)
{
	int i, read;
	uint8_t nbPositions;
	uint8_t* positions;
	GameState* game = server_get_game(serverState, socket);
	read = read_socket(&nbPositions, 1, 1, socket);
	if (read != 1)
		return FALSE;
	if (nbPositions == 0)
		return FALSE;
	positions = (uint8_t*)malloc(nbPositions);
	if (positions == NULL)
		return FALSE;
	read = read_socket(positions, 1, nbPositions, socket);
	if (read != nbPositions)
		return FALSE;
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
		game->turn = 2;
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

boolean server_treat_req_sync(ServerState* serverState, int socket)
{
	uint8_t mapBuffer[50];
	GameState* game = server_get_game(serverState, socket);
	if (game == NULL)
		return send_message_error(socket, ERROR_MESSAGE_NON_VALIDE);
	map_serialize(game->map, mapBuffer, 50);
	return send_message_sync(socket, game->turn, mapBuffer);
}

boolean server_treat_ping(ServerState* serverState, int socket)
{
	return send_message_pong(socket);
}
