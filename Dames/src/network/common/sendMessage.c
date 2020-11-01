#include "sendMessage.h"
#include "codeMessages.h"
#include "../common/network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


boolean send_message(int socket, uint8_t messageType, void* data, unsigned long dataSize)
{
	unsigned long dataSend;
	uint8_t* message = (uint8_t*)malloc(dataSize + 1);
	if (message == NULL)
		return FALSE;
	message[0] = messageType;
	if (dataSize > 0)
		memcpy(message + 1, data, dataSize);
	dataSend = write_socket(message, dataSize + 1, 1, socket);
	free(message);
	return dataSend == dataSize + 1;
}

boolean send_message_ping(int socket)
{
	return send_message(socket, CM_PING, NULL, 0);
}

boolean send_message_pong(int socket)
{
	return send_message(socket, CM_PONG, NULL, 0);
}

boolean send_message_execute_turn(int socket, uint8_t* positions, uint8_t nbPositions)
{
	boolean result;
	uint8_t* data = (uint8_t*)malloc(nbPositions + 1);
	if (data == NULL)
		return FALSE;
	data[0] = nbPositions;
	memcpy(data + 1, positions, nbPositions);
	result = send_message(socket, CM_TOUR, data, nbPositions + 1);
	free(data);
	return result;
}
