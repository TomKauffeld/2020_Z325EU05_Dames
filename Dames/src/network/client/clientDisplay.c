#include "clientDisplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void client_display(ClientState* clientState)
{
	switch (clientState->connectionStatus)
	{
	case CONNECTION_STATUS_NON_CONNECTE:
		client_display_connection_options(clientState);
		break;
	case CONNECTION_STATUS_CONNEXION_INVITE:

	default:
		break;
	}
}


void client_display_connection_options(ClientState* clientState)
{
	printf("Connexion\n");
	printf("[1] creer un compte\n");
	printf("[2] connexion avec un compte\n");
	printf("[3] connexion en mode invite\n");
}

void client_display_create_account(ClientState* clientState)
{

}