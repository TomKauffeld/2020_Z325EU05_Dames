#ifndef __MESSAGES_CODEMESSAGES_H__
#define __MESSAGES_CODEMESSAGES_H__

#define CM_CREATION_DE_COMPTE 0x01
#define CM_CONNEXION_AU_COMPTE 0x02
#define CM_CONNEXION_MODE_INVITE 0x03
#define CM_CREATION_PARTIE 0x04
#define CM_CONNEXION_PARTIE 0x05
#define CM_CONNEXION_SPECTATEUR 0x06
#define CM_RECHERCHE 0x07
#define CM_TOUR 0x08
#define CM_REQ_SYNC 0x09
#define CM_SYNC 0x0A
#define CM_LISTE_PARTIES 0x0B
#define CM_NOM_INVITE 0x0C
#define CM_PING 0xF0
#define CM_PONG 0xF1
#define CM_OK 0xFE
#define CM_ERREUR 0xFF

#define ERROR_GENERIQUE 0x00
#define ERROR_NOM_UTILISE 0x01
#define ERROR_CONNEXION 0x02
#define ERROR_PARTIE_NON_TROUVE 0x03
#define ERROR_PARTIE_EN_COURS 0x04
#define ERROR_COUP_NON_VALIDE 0x05
#define ERROR_MESSAGE_NON_VALIDE 0x06

#define STATUS_ATTENTE 0x01
#define STATUS_ENCOURS 0x02

#endif