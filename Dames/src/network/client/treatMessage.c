#include "treatMessage.h"
#include "../common/codeMessages.h"
#include "sendMessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool client_treat_message(ClientState* clientState, int socket)
{
	uint8_t messageType;
	fread(&messageType, 1, 1, socket);
	switch (messageType)
	{
	case CM_PING:
		return client_treat_message_ping(clientState, socket);
	case CM_PONG:
		return client_treat_message_pong(clientState, socket);
	case CM_TOUR:
		return client_treat_message_execute_turn(clientState, socket);
	case CM_SYNC:
		return client_treat_message_sync(clientState, socket);
	case CM_LISTE_PARTIES:
		return client_treat_message_list_games(clientState, socket);
	case CM_NOM_INVITE:
		return client_treat_message_guest_name(clientState, socket);
	case CM_OK:
		return client_treat_message_ok(clientState, socket);
	case CM_ERREUR:
		return client_treat_message_error(clientState, socket);
	default:
		return false;
	}
}

bool client_treat_message_ping(ClientState* clientState, int socket)
{
	return send_message_pong(socket);
}

bool client_treat_message_pong(ClientState* clientState, int socket)
{
	if (clientState->pingState != PING_WAIT)
		return false;
	clientState->pingState = PING_OK;
	return true;
}

bool client_treat_message_execute_turn(ClientState* clientState, int socket)
{
	uint8_t nbPositions;
	uint8_t* positions;
	fread(&nbPositions, 1, 1, socket);
	if (positions == 0)
		return false;
	positions = (uint8_t*)malloc(nbPositions);
	if (positions == NULL)
		return false;
	fread(positions, 1, nbPositions, socket);
	if (clientState->map == NULL)
		return false;
	if (clientState->turn == clientState->player)
		return false;
	if (!map_turn(clientState->map, positions, nbPositions))
		return false;
	if (clientState->turn == 1)
		clientState->turn = 2;
	else
		clientState->turn = 1;
	return true;
}

bool client_treat_message_sync(ClientState* clientState, int socket)
{
	uint8_t turn;
	uint8_t data[50];
	fread(&turn, 1, 1, socket);
	fread(data, 1, 50, socket);
	if (clientState->map != NULL)
		map_destroy(clientState->map);
	clientState->map = map_unserialize(data, 50);
	clientState->turn = turn;
	return true;
}

bool client_treat_message_list_games(ClientState* clientState, int socket)
{
	int i;
	uint8_t nbGames;
	fread(&nbGames, 1, 1, socket);
	if (clientState->availableGames != NULL)
	{
		for (i = 0; i < clientState->nbAvailableGames; i++)
			free(clientState->availableGames[i].name);
		free(clientState->availableGames);
		clientState->availableGames = NULL;
		clientState->nbAvailableGames = 0;
	}
	if (nbGames < 1)
		return true;
	clientState->availableGames = (struct game*)malloc(sizeof(struct game) * nbGames);
	clientState->nbAvailableGames = nbGames;
	for (i = 0; i < nbGames; i++)
	{
		fread(&clientState->availableGames[i].id, 1, 1, socket);
		fread(&clientState->availableGames[i].status, 1, 1, socket);
		fread(&clientState->availableGames[i].name_length, 1, 1, socket);
		clientState->availableGames[i].name = (char*)malloc(clientState->availableGames[i].name_length + 1);
		if (clientState->availableGames[i].name != NULL)
		{
			fread(clientState->availableGames[i].name, 1, clientState->availableGames[i].name_length, socket);
			clientState->availableGames[i].name[clientState->availableGames[i].name_length] = 0;
		}
		else
			fseek(socket, clientState->availableGames[i].name_length, SEEK_CUR);
	}
	return true;
}

bool client_treat_message_guest_name(ClientState* clientState, int socket)
{
	uint8_t usernameLength;
	char* username;
	fread(&usernameLength, 1, 1, socket);
	if (usernameLength == 0)
		return false;
	username = (char*)malloc(usernameLength + 1);
	if (username == NULL)
		return false;
	fread(username, 1, usernameLength, socket);
	username[usernameLength] = 0;
	if (clientState->username == NULL)
	{
		clientState->username = username;
		free(username);
		return true;
	}
	else
	{
		free(username);
		return false;
	}
}

bool client_treat_message_ok(ClientState* clientState, int socket)
{
	if (clientState->pendingPositions != NULL)
	{
		if (!map_turn(clientState->map, clientState->pendingPositions, clientState->nbPendingPositions))
			return false;
		if (clientState->turn == 1)
			clientState->turn = 2;
		else
			clientState->turn = 1;
		clientState->nbPendingPositions = 0;
		free(clientState->pendingPositions);
		clientState->pendingPositions = NULL;
	}
	if (clientState->pendingUsername != NULL)
	{
		if (clientState->username != NULL)
			free(clientState->username);
		clientState->username = clientState->pendingUsername;
		clientState->pendingUsername = NULL;
	}
}

bool client_treat_message_error(ClientState* clientState, int socket)
{
	uint8_t errorCode;
	fread(&errorCode, 1, 1, socket);
	switch (errorCode)
	{
	case ERROR_NOM_UTILISE:
	case ERROR_CONNEXION:
		if (clientState->pendingUsername != NULL)
		{
			free(clientState->pendingUsername);
			clientState->pendingUsername = NULL;
			return true;
		}
		break;
	case ERROR_COUP_NON_VALIDE:
		if (clientState->pendingPositions != NULL)
		{
			free(clientState->pendingPositions);
			clientState->pendingPositions = NULL;
			clientState->nbPendingPositions = 0;
			return send_message_req_sync(socket);
		}
		break;
	case ERROR_PARTIE_EN_COURS:
	case ERROR_PARTIE_NON_TROUVE:
		if (clientState->map != NULL)
			map_destroy(clientState->map);
		if (clientState->pendingPositions != NULL)
			free(clientState->pendingPositions);
		clientState->map = NULL;
		clientState->pendingPositions = NULL;
		clientState->nbPendingPositions = 0;
		clientState->player = 0;
		clientState->turn = 0;
		return send_message_get_games(socket);
	default:
		break;
	}
	printf("ERROR : %d\n", errorCode);
	return false;
}
