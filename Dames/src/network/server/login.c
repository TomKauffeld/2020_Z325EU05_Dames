#include "login.h"

#include <stdlib.h>
#include <string.h>

Login* login_init(char* username, bool isGuest, int socket)
{
	Login* login = (Login*)malloc(sizeof(Login));
	if (login == NULL)
		return NULL;
	login->isGuest = isGuest;
	login->socket = socket;
	memset(login->username, 0x00, 1 + UINT8_MAX);
	strcpy(login->username, username);
	return login;
}

void login_destroy(Login* login)
{
	free(login);
}
