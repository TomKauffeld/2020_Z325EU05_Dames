#ifndef __NETWORK_CLIENT_SENDMESSAGE_H__
#define __NETWORK_CLIENT_SENDMESSAGE_H__
#include "../common/sendMessage.h"


bool send_message_create_account(int socket, char* username, char* password);

bool send_message_login_account(int socket, char* username, char* password);

bool send_message_login_guest(int socket);

bool send_message_create_game(int socket);

bool send_message_join_game(int socket, uint8_t game_id);

bool send_message_spectate_game(int socket, uint8_t game_id);

bool send_message_get_games(int socket);

bool send_message_req_sync(int socket);


#endif