#ifndef __NETWORK_COMMON_SENDMESSAGE_H__
#define __NETWORK_COMMON_SENDMESSAGE_H__
#include <inttypes.h>
#include "../../utils/bool.h"

bool send_message(int socket, uint8_t messageType, void* data, size_t dataSize);

bool send_message_ping(int socket);

bool send_message_pong(int socket);

bool send_message_execute_turn(int socket, uint8_t* positions, uint8_t nbPositions);


#endif