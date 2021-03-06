#include "serverSendMessage.h"
#include "../common/codeMessages.h"

#include <stdlib.h>
#include <string.h>

boolean send_message_sync(int socket, uint8_t player, void* map)
{
	uint8_t data[51];
	data[0] = player;
	memcpy(data + 1, map, 50);
	return send_message(socket, CM_SYNC, data, 51);
}

boolean send_message_list_games(int socket, struct game* games, uint8_t nbGames)
{
	uint8_t* data;
	size_t dataSize = 1;
	int i, offset = 1;
	boolean result;
	for (i = 0; i < nbGames; i++)
		dataSize += games[i].name_length + 3;

	data = (uint8_t*)malloc(dataSize);
	if (data == NULL)
		return FALSE;
	
	data[0] = nbGames;
	for (i = 0; i < nbGames; i++)
	{
		data[offset] = games[i].id;
		data[offset + 1] = games[i].status;
		data[offset + 2] = games[i].name_length;
		memcpy(data + offset + 3, games[i].name, games[i].name_length);
		offset += 3 + games[i].name_length;
	}

	result = send_message(socket, CM_LISTE_PARTIES, data, dataSize);
	free(data);
	return result;
}

boolean send_message_guest_name(int socket, char* name)
{
	boolean result;
	uint8_t* data = (uint8_t*)malloc(strlen(name) + 1);
	if (data == NULL)
		return FALSE;
	data[0] = strlen(name);
	memcpy(data + 1, name, strlen(name));

	result = send_message(socket, CM_NOM_INVITE, data, strlen(name) + 1);
	free(data);
	return result;
}

boolean send_message_ok(int socket)
{
	return send_message(socket, CM_OK, NULL, 0);
}

boolean send_message_error(int socket, uint8_t error_type)
{
	return send_message(socket, CM_ERREUR, &error_type, 1);
}