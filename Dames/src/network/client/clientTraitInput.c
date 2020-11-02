#include "clientTreatInput.h"
#include "clientSendMessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
			clientState->connectionStatus = CONNECTION_STATUS_CONNEXION_INVITE;
			send_message_login_guest(socket);
		}
		else
			return FALSE;
		return TRUE;
	default:
		return FALSE;
	}
}
