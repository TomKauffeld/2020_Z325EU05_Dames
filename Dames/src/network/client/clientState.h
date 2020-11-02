#ifndef __NETWORK_CLIENT_CLIENTSTATE_H__
#define __NETWORK_CLIENT_CLIENTSTATE_H__

#include "../../utils/bool.h"
#include "../../game/map.h"

#define PING_OK 1
#define PING_WAIT 0

struct game {
	uint8_t id;
	uint8_t status;
	uint8_t name_length;
	char* name;
};

#define CONNECTION_STATUS_NON_CONNECTE 0x01
#define CONNECTION_STATUS_CREATION_COMPTE 0x02
#define CONNECTION_STATUS_CONNEXION_COMPTE 0x03
#define CONNECTION_STATUS_CONNEXION_PENDING 0x04
#define CONNECTION_STATUS_CONNEXION_OK 0x05

#define CLIENT_PLAYER_1 1
#define CLIENT_PLAYER_2 2
#define CLIENT_PLAYER_SPECTATEUR 0
#define CLIENT_PLAYER_NONE 0xff

typedef struct clientState {
	uint8_t connectionStatus;
	Map* map;
	char* username;
	uint8_t player;
	uint8_t turn;
	uint8_t pingState;
	uint8_t* pendingPositions;
	uint8_t nbPendingPositions;
	char* pendingUsername;
	struct game* availableGames;
	int nbAvailableGames;
} ClientState;

ClientState* client_init();

void client_destroy(ClientState* clientState);

#endif

