#include "clientSendMessage.h"
#include "../common/codeMessages.h"

#include <stdlib.h>
#include <string.h>

boolean send_message_create_or_login(int socket, uint8_t message_type, char* username, char* password)
{
	boolean result;
	uint8_t* data = (uint8_t*)malloc(strlen(username) + strlen(password) + 2);
	if (data == NULL)
		return FALSE;
	data[0] = strlen(username);
	data[1] = strlen(password);
	memcpy(data + 2, username, strlen(username));
	memcpy(data + 2 + strlen(username), password, strlen(password));
	result = send_message(socket, message_type, data, strlen(username) + strlen(password) + 2);
	free(data);
	return result;
}

boolean send_message_create_account(int socket, char* username, char* password)
{
	return send_message_create_or_login(socket, CM_CREATION_DE_COMPTE, username, password);
}

boolean send_message_login_account(int socket, char* username, char* password)
{
	return send_message_create_or_login(socket, CM_CONNEXION_AU_COMPTE, username, password);
}

boolean send_message_login_guest(int socket)
{
	return send_message(socket, CM_CONNEXION_MODE_INVITE, NULL, 0);
}

boolean send_message_create_game(int socket)
{
	return send_message(socket, CM_CREATION_PARTIE, NULL, 0);
}

boolean send_message_join_game(int socket, uint8_t game_id)
{
	return send_message(socket, CM_CONNEXION_PARTIE, &game_id, 1);
}

boolean send_message_spectate_game(int socket, uint8_t game_id)
{
	return send_message(socket, CM_CONNEXION_SPECTATEUR, &game_id, 1);
}

boolean send_message_get_games(int socket)
{
	return send_message(socket, CM_RECHERCHE, NULL, 0);
}

boolean send_message_req_sync(int socket)
{
	return send_message(socket, CM_REQ_SYNC, NULL, 0);
}