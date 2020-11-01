#include "server.h"
#include <stdlib.h>
#include <string.h>

ServerState* server_init()
{
	ServerState* serverState = (ServerState*)malloc(sizeof(ServerState));
	if (serverState == NULL)
		return NULL;
	serverState->accounts = NULL;
	serverState->logins = NULL;
	serverState->nbAccounts = 0;
	serverState->nblogins = 0;
	return serverState;
}

bool server_is_username_taken(ServerState* serverState, char* username)
{
	int i;
	for (i = 0; i < serverState->nbAccounts; i++)
		if (strcmp(serverState->accounts[i].username, username) == 0)
			return true;
	return false;
}

bool server_add_account(ServerState* serverState, char* username, char* password, int socket)
{
	BYTE hash[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, password, strlen(password));
	sha256_final(&ctx, hash);
	Account* tmpAccounts;
	if (serverState->accounts != NULL)
		tmpAccounts = (Account*)realloc(serverState->accounts, (serverState->nbAccounts + 1) * sizeof(Account));
	else
		tmpAccounts = (Account*)malloc(sizeof(Account));
	if (tmpAccounts == NULL)
		return false;
	memcpy(tmpAccounts[serverState->nbAccounts].password, hash, SHA256_BLOCK_SIZE);
	tmpAccounts[serverState->nbAccounts].username = username;
	serverState->accounts = tmpAccounts;
	serverState->nbAccounts++;
	return server_connect(serverState, username, false, socket);
}

bool server_connect(ServerState* serverState, char* username, bool isGuest, int socket)
{
	Login* tmpLogins;
	if (serverState->logins != NULL)
		tmpLogins = (Login*)realloc(serverState->logins, (serverState->nblogins + 1) * sizeof(Login));
	else
		tmpLogins = (Login*)malloc(sizeof(Login));
	if (tmpLogins == NULL)
		return false;
	tmpLogins[serverState->nblogins].id = serverState->nblogins;
	memset(tmpLogins[serverState->nblogins].username, 0x00, UINT8_MAX + 1);
	strcpy(tmpLogins[serverState->nblogins].username, username);
	tmpLogins[serverState->nblogins].isGuest = isGuest;
	tmpLogins[serverState->nblogins].socket = socket;
	serverState->logins = tmpLogins;
	serverState->nblogins++;
	return true;
}

bool server_check_username_password(ServerState* serverState, char* username, char* password)
{
	BYTE hash[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, password, strlen(password));
	sha256_final(&ctx, hash);
	int i;
	for (i = 0; i < serverState->nbAccounts; i++)
		if (strcmp(serverState->accounts[i].username, username) == 0)
			return memcmp(serverState->accounts[i].password, hash, SHA256_BLOCK_SIZE) == 0;
	return false;
}

void server_destroy(ServerState* serverState)
{
	int i;
	if (serverState->logins != NULL)
		free(serverState->logins);
	if (serverState->accounts != NULL)
	{
		for (i = 0; i < serverState->nbAccounts; i++)
		{
			free(serverState->accounts[i].username);
		}
		free(serverState->accounts);
	}
	free(serverState);
}
