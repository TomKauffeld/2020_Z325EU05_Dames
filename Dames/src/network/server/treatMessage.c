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

bool server_treat_ping(ServerState* serverState, int socket)
{
	return send_message_pong(socket);
}
