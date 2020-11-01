#ifndef __NETWORK_SERVER_TREAT_MESSAGE_H__
#define __NETWORK_SERVER_TREAT_MESSAGE_H__
#include "server.h"

boolean server_treat_message(ServerState* serverState, int socket);

boolean server_treat_create_account(ServerState* serverState, int socket);

boolean server_treat_login_account(ServerState* serverState, int socket);

boolean server_treat_login_guest(ServerState* serverState, int socket);

boolean server_treat_create_game(ServerState* serverState, int socket);

boolean server_treat_join_game(ServerState* serverState, int socket);

boolean server_treat_spectate(ServerState* serverState, int socket);

boolean server_treat_search(ServerState* serverState, int socket);

boolean server_treat_execute_turn(ServerState* serverState, int socket);

boolean server_treat_req_sync(ServerState* serverState, int socket);

boolean server_treat_ping(ServerState* serverState, int socket);


#endif
