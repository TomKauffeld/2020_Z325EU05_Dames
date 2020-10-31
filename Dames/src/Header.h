#pragma once
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
typedef char bool;





void treatConnection(int socket) 
{
	uint8_t usrLen, pwdLen;
	char* username;
	char* password;
	fread(&usrLen, 1, 1, socket);
	fread(&pwdLen, 1, 1, socket);
	username = (char*)malloc(usrLen+1);
	password = (char*)malloc(pwdLen+1);
	fread(username, 1, usrLen, socket);
	fread(password, 1, pwdLen, socket);
	username[usrLen] = 0x00;
	password[pwdLen] = 0x00;




}


#define MESSAGE_CON 0x01
#define MESSAGE_ERR 0xFF

void treatMessage(int socket) {
	uint8_t messageType;
	fread(&messageType, 1, 1, socket);
	switch (messageType)
	{
	case MESSAGE_CON:
		treatConnection(socket);
		break;
	default:
		char* error = "MESSAGE NOT CORRECT";
		fwrite(MESSAGE_ERR, 1, 1, socket);
		fwrite(strlen(error), 1, 1, socket);
		fwrite(error, 1, strlen(error), socket);
		break;
	}
}