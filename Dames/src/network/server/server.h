#ifndef __NETWORK_SERVER_SERVER_H__
#define __NETWORK_SERVER_SERVER_H__

#include "../../utils/bool.h"
#include "../../utils/sha256.h"

typedef struct account {
	char* username;
	BYTE password[SHA256_BLOCK_SIZE];
} Account;

typedef struct login {
	uint8_t id;
	char username[1 + UINT8_MAX];
	bool isGuest;
	int socket;
} Login;

typedef struct serverState {
	int nbAccounts;
	int nblogins;
	Account* accounts;
	Login* logins;
} ServerState;

ServerState * server_init();

bool server_is_username_taken(ServerState* serverState, char* username);

bool server_add_account(ServerState* serverState, char* username, char* password, int socket);

bool server_connect(ServerState* serverState, char* username, bool isGuest, int socket);

bool server_check_username_password(ServerState* serverState, char* username, char* password);

void server_destroy(ServerState* serverState);


#endif
