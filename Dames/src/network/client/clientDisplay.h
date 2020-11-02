#ifndef __NETWORK_CLIENT_DISPLAY_H__
#define __NETWORK_CLIENT_DISPLAY_H__

#include "clientState.h"


void client_display(ClientState* clientState);

void client_display_connection_options(ClientState* clientState);

void client_display_connection_pending(ClientState* clientState);

void client_display_create_account(ClientState* clientState);

void client_display_connection_account(ClientState* clientState);

void client_display_main(ClientState* clientState);

void client_display_list_games(ClientState* clientState);

void client_display_game(ClientState* clientState);

#endif