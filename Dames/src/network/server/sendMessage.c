#include "sendMessage.h"
#include "../common/codeMessages.h"

#include <stdlib.h>
#include <string.h>

bool send_message_sync(int socket, uint8_t player, void* map)
{
	uint8_t data[51];
	data[0] = player;
	memcpy(data + 1, map, 50);
	return send_message(socket, CM_SYNC, data, 51);
}

bool send_message_list_games(int socket, struct game* games, uint8_t nbGames)
{
	uint8_t* data;
	size_t dataSize = 1;
	int i, offset = 1;
	bool result;
	for (i = 0; i < nbGames; i++)
		dataSize += games[i].name_length + 3;

	data = (uint8_t*)malloc(dataSize);
	if (data == NULL)
		return false;
	
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

bool send_message_guest_name(int socket, uint8_t name_length, char* name)
{
	bool result;
	uint8_t* data = (uint8_t*)malloc(name_length + 1);
	if (data == NULL)
		return false;
	data[0] = name_length;
	memcpy(data + 1, name, name_length);

	result = send_message(socket, CM_NOM_INVITE, data, name_length + 1);
	free(data);
	return result;
}

bool send_message_ok(int socket)
{
	return send_message(socket, CM_OK, NULL, 0);
}

bool send_message_error(int socket, uint8_t error_type)
{
	return send_message(socket, CM_ERREUR, &error_type, 1);
}