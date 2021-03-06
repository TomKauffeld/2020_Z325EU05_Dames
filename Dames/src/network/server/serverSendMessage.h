#ifndef __NETWORK_SERVER_SENDMESSAGE_H__
#define __NETWORK_SERVER_SENDMESSAGE_H__
#include "../common/sendMessage.h"

struct game {
	uint8_t id;
	uint8_t status;
	uint8_t name_length;
	char* name;
};

boolean send_message_sync(int socket, uint8_t player, void* map);

boolean send_message_list_games(int socket, struct game* games, uint8_t nbGames);

boolean send_message_guest_name(int socket, char* name);

boolean send_message_ok(int socket);

boolean send_message_error(int socket, uint8_t error_type);


#endif