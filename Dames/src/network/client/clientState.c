#include "clientState.h"

#include <stdlib.h>
#include <string.h>

ClientState* client_init()
{
	ClientState* clientState = (ClientState*)malloc(sizeof(ClientState));
	if (clientState == NULL)
		return NULL;
	clientState->availableGames = NULL;
	clientState->connectionStatus = CONNECTION_STATUS_NON_CONNECTE;
	clientState->map = NULL;
	clientState->nbAvailableGames = 0;
	clientState->nbPendingPositions = 0;
	clientState->pendingPositions = NULL;
	clientState->pendingUsername = NULL;
	clientState->pingState = PING_WAIT;
	clientState->player = 0;
	clientState->turn = 0;
	clientState->username = NULL;
}

void client_destroy(ClientState* clientState)
{
	int i;
	if (clientState->availableGames != NULL)
	{
		for (i = 0; i < clientState->nbAvailableGames; i++)
			free(clientState->availableGames[i].name);
		free(clientState->availableGames);
	}
	if (clientState->pendingPositions != NULL)
		free(clientState->pendingPositions);
	if (clientState->pendingUsername != NULL)
		free(clientState->pendingUsername);
	if (clientState->username != NULL)
		free(clientState->username);
	free(clientState);
}