#include "clientTreatInput.h"
#include "clientSendMessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

boolean client_treat_input(ClientState* clientState, char* text, int socket)
{
	switch (clientState->connectionStatus)
	{
	case CONNECTION_STATUS_NON_CONNECTE:
		if (strcmp(text, "1") == 0)
			clientState->connectionStatus = CONNECTION_STATUS_CREATION_COMPTE;
		else if (strcmp(text, "2") == 0)
			clientState->connectionStatus = CONNECTION_STATUS_CONNEXION_COMPTE;
		else if (strcmp(text, "3") == 0)
		{
			clientState->connectionStatus = CONNECTION_STATUS_CONNEXION_PENDING;
			send_message_login_guest(socket);
		}
		else
			return FALSE;
		return TRUE;
	case CONNECTION_STATUS_CONNEXION_COMPTE:
		if (clientState->pendingUsername == NULL)
		{
			clientState->pendingUsername = (char*)malloc(sizeof(char) * (strlen(text) + 1));
			memcpy(clientState->pendingUsername, text, strlen(text));
			clientState->pendingUsername[strlen(text)] = 0;
		}
		else
		{
			clientState->connectionStatus = CONNECTION_STATUS_CONNEXION_PENDING;
			send_message_login_account(socket, clientState->pendingUsername, text);
		}
		break;
	case CONNECTION_STATUS_CREATION_COMPTE:
		if (clientState->pendingUsername == NULL)
		{
			clientState->pendingUsername = (char*)malloc(sizeof(char) * (strlen(text) + 1));
			memcpy(clientState->pendingUsername, text, strlen(text));
			clientState->pendingUsername[strlen(text)] = 0;
		}
		else
		{
			clientState->connectionStatus = CONNECTION_STATUS_CONNEXION_PENDING;
			send_message_create_account(socket, clientState->pendingUsername, text);
		}
		break;
	case CONNECTION_STATUS_CONNEXION_OK:
		return client_treat_input_main(clientState, text, socket);
	default:
		return FALSE;
	}
}

boolean client_treat_input_main(ClientState* clientState, char* text, int socket)
{
	int i;
	if (clientState->player == CLIENT_PLAYER_NONE)
	{
		boolean digits = TRUE;
		for (i = 0; digits && i < strlen(text); i++)
			digits &= isdigit(text[i]);
		if (digits)
		{

		}
		else if (strcmp(text, "create") == 0)
		{
			clientState->map = map_init();
			clientState->player = CLIENT_PLAYER_1;
			clientState->turn = 0;
			send_message_create_game(socket);
		}
		else
			return FALSE;
		return TRUE;
	}
	else
		return FALSE;
}