#include "clientTreatInput.h"
#include "clientSendMessage.h"
#include "../common/codeMessages.h"

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
	int i = 0;
	if (clientState->player == CLIENT_PLAYER_NONE)
	{
		sscanf(text, "%d", &i);
		if (i != 0)
		{
			sscanf(text, "%d", &i);
			if (i > 0 && i <= clientState->nbAvailableGames)
			{
				struct game game = clientState->availableGames[i - 1];
				if (game.status == STATUS_ATTENTE)
				{
					clientState->player = CLIENT_PLAYER_2;
					send_message_join_game(socket, game.id);
				}
				else
				{
					clientState->player = CLIENT_PLAYER_SPECTATEUR;
					send_message_spectate_game(socket, game.id);
				}
			}
			else
				return FALSE;
		}
		else if (strcmp(text, "create") == 0)
		{
			clientState->map = map_init();
			clientState->player = CLIENT_PLAYER_1;
			clientState->turn = 0;
			send_message_create_game(socket);
		}
		else
			send_message_get_games(socket);
		return TRUE;
	}
	else if (clientState->player == clientState->turn && clientState->turn != 0)
	{
		boolean valid = TRUE;
		uint8_t positions[UINT8_MAX];
		uint8_t nbPositions = 0;
		char tmp[4];
		memset(tmp, 0x00, 4);
		int index = 0;
		for (i = 0; valid && i < strlen(text); i++)
		{
			switch (text[i])
			{
			case ';':
				if (index >= 2)
				{
					Vector vector;
					char c = tmp[0];
					if (c >= 0x61)
						c -= 32;
					vector.x = (int)c - 65;
					int ui;
					sscanf(tmp + 1, "%d", &ui);
					vector.y = ui - 1;
					uint8_t pos = map_vector_to_location(vector);
					if (pos > 0)
						positions[nbPositions++] = pos;
					else
						valid = FALSE;
					index = 0;
					memset(tmp, 0x00, 4);
				}
				else
					valid = FALSE;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (index > 0 && index < 3)
					tmp[index++] = text[i];
				else
					valid = FALSE;
				index++;
				break;
			case 'A':
			case 'a':
			case 'B':
			case 'b':
			case 'C':
			case 'c':
			case 'D':
			case 'd':
			case 'E':
			case 'e':
			case 'F':
			case 'f':
			case 'G':
			case 'g':
			case 'H':
			case 'h':
			case 'I':
			case 'i':
			case 'J':
			case 'j':
				if (index < 1)
					tmp[index++] = text[i];
				else
					valid = FALSE;
				break;
			default:
				valid = FALSE;
				break;
			}
		}
		if (valid)
		{
			if (index >= 2)
			{
				Vector vector;
				char c = tmp[0];
				if (c >= 0x61)
					c -= 32;
				vector.x = (int)c - 65;
				uint8_t ui;
				sscanf(tmp + 1, "%d", &ui);
				vector.y = ui - 1;
				uint8_t pos = map_vector_to_location(vector);
				if (pos > 0)
					positions[nbPositions++] = pos;
				else
					valid = FALSE;
			}
			else
				valid = FALSE;
			uint8_t captures[20];
			uint8_t nbCaptures;
			valid &= map_validate_turn(clientState->map, positions, nbPositions, captures, 20, &nbCaptures);
			if (valid)
			{
				clientState->pendingPositions = (uint8_t*)malloc(nbPositions);
				if (clientState->pendingPositions != NULL)
				{
					memcpy(clientState->pendingPositions, positions, nbPositions);
					clientState->nbPendingPositions = nbPositions;
					send_message_execute_turn(socket, positions, nbPositions);
				}
			}
		}
	}
	else
		return FALSE;
}