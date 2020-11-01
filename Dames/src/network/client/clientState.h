#ifndef __NETWORK_CLIENT_CLIENTSTATE_H__
#define __NETWORK_CLIENT_CLIENTSTATE_H__

#include "../../utils/bool.h"
#include "../../game/map.h"

#define PING_OK 1
#define PING_WAIT 0


typedef struct clientState {
	Map* map;
	char* username;
	uint8_t player;
	uint8_t turn;
	uint8_t pingState;
	uint8_t* pendingPositions;
	uint8_t nbPendingPositions;
	char* pendingUsername;
} ClientState;


#endif

