#include "sendMessage.h"
#include "codeMessages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool send_message(int socket, uint8_t messageType, void* data, size_t dataSize)
{
	uint8_t* message = (uint8_t*)malloc(dataSize + 1);
	if (message == NULL)
		return false;
	message[0] = messageType;
	if (dataSize > 0)
		memcpy(message + 1, data, dataSize);
	fwrite(message, dataSize + 1, 1, socket);
	fflush(socket);
	free(message);
	return true;
}

bool send_message_ping(int socket)
{
	return send_message(socket, CM_PING, NULL, 0);
}

bool send_message_pong(int socket)
{
	return send_message(socket, CM_PONG, NULL, 0);
}

bool send_message_execute_turn(int socket, uint8_t* positions, uint8_t nbPositions)
{
	bool result;
	uint8_t* data = (uint8_t*)malloc(nbPositions + 1);
	if (data == NULL)
		return false;
	data[0] = nbPositions;
	memcpy(data + 1, positions, nbPositions);
	result = send_message(socket, CM_TOUR, data, nbPositions + 1);
	free(data);
	return result;
}
