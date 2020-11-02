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
	case CONNECTION_STATUS_CREATION_COMPTE:
		client_display_create_account(clientState);
		break;
	case CONNECTION_STATUS_CONNEXION_COMPTE:
		client_display_connection_account(clientState);
		break;
	case CONNECTION_STATUS_CONNEXION_PENDING:
		client_display_connection_pending(clientState);
		break;
	case CONNECTION_STATUS_CONNEXION_OK:
		client_display_main(clientState);
		break;
	default:
		printf("Etat non reconnue\n");
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


void client_display_connection_pending(ClientState* clientState)
{
	printf("Connexion en cours\n");
}

void client_display_create_account(ClientState* clientState)
{
	if (clientState->pendingUsername == NULL)
	{
		printf("Creation de compte\n");
		printf("Nom d'utilisateur : ");
	}
	else
		printf("Mot de passe : ");
}

void client_display_connection_account(ClientState* clientState)
{
	if (clientState->pendingUsername == NULL)
	{
		printf("Connection au compte\n");
		printf("Nom d'utilisateur : \n");
	}
	else
		printf("Mot de passe : \n");
}

void client_display_main(ClientState* clientState)
{
	printf("Connecte en tant que %s\n", clientState->username);
	if (clientState->player == CLIENT_PLAYER_NONE)
		client_display_list_games(clientState);
	else
		client_display_game(clientState);
}

void client_display_list_games(ClientState* clientState)
{
	int i;
	printf("Parties en cours:\n");
	if (clientState->availableGames != NULL)
	{
		for (i = 0; i < clientState->nbAvailableGames; i++)
			printf("[%d] %s\n", i + 1, clientState->availableGames[i].name);
	}
	else
		printf("chargement...\n");
	printf("Pour creer une nouvelle partie, tapez \"create\"\n");
}

void client_display_game(ClientState* clientState)
{
	int i, line;
	char symbole;
	if (clientState->turn == 0)
		printf("en attente d'un autre jouer\n");
	else
	{
		printf("    A B C D E F G H I J\n");
		for (i = 1; i <= 50; i++)
		{
			line = (i - 1) / 5;
			switch (map_get_symbole(clientState->map, i))
			{
			case PLAYER_1_KING:
				symbole = 'B';
				break;
			case PLAYER_1_MEN:
				symbole = 'b';
				break;
			case PLAYER_2_KING:
				symbole = 'N';
				break;
			case PLAYER_2_MEN:
				symbole = 'n';
				break;
			case EMPTY:
				symbole = '.';
				break;
			default:
				symbole = '?';
				break;
			}

			if (i % 5 == 1)
				printf("%2d ", line + 1);
			if (line % 2 == 0)
				printf("   %lc", symbole);
			else
				printf(" %lc  ", symbole);
			if (i % 5 == 0)
				printf("\n");
		}
		if (clientState->turn == clientState->player)
			printf("C'est a ton tour\n");
		else
			printf("c'est au tour du joueur %d\n", clientState->turn);
	}
}