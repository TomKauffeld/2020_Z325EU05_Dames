#ifndef __NETWORK_SERVER_LOGIN_H__
#define __NETWORK_SERVER_LOGIN_H__

#include <inttypes.h>
#include "../../utils/bool.h"

typedef struct login {
	char username[1 + UINT8_MAX];
	bool isGuest;
	int socket;
} Login;

Login* login_init(char* username, bool isGuest, int socket);

void login_destroy(Login* login);

#endif