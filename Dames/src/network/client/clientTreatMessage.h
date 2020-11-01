#ifndef __NETWORK_CLIENT_TREAT_MESSAGE_H__
#define __NETWORK_CLIENT_TREAT_MESSAGE_H__
#include "clientState.h"

boolean client_treat_message(ClientState* clientState, int socket);

boolean client_treat_message_ping(ClientState* clientState, int socket);

boolean client_treat_message_pong(ClientState* clientState, int socket);

boolean client_treat_message_execute_turn(ClientState* clientState, int socket);

boolean client_treat_message_sync(ClientState* clientState, int socket);

boolean client_treat_message_list_games(ClientState* clientState, int socket);

boolean client_treat_message_guest_name(ClientState* clientState, int socket);

boolean client_treat_message_ok(ClientState* clientState, int socket);

boolean client_treat_message_error(ClientState* clientState, int socket);

#endif