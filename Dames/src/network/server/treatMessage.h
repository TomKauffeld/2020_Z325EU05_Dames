#ifndef __NETWORK_SERVER_TREAT_MESSAGE_H__
#define __NETWORK_SERVER_TREAT_MESSAGE_H__

#include <inttypes.h>

void server_treat_message(int socket) 
{
	uint8_t messageType;
	fread(&messageType, 1, 1, socket);
	switch (messageType)
	{
	case MESSAGE_CON:
		treatConnection(socket);
		break;
	default:
		char* error = "MESSAGE NOT CORRECT";
		fwrite(MESSAGE_ERR, 1, 1, socket);
		fwrite(strlen(error), 1, 1, socket);
		fwrite(error, 1, strlen(error), socket);
		break;
	}
}




#endif
