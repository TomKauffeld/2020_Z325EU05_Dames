#include "Header.h"

#ifdef MODE_CLIENT


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 

#include "network/client/clientState.h"
#include "network/client/clientTreatMessage.h"
#include "network/client/clientTreatInput.h"
#include "network/client/clientDisplay.h"


#define BUFFER_SIZE 1024
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


void connexion(const char *address){
    int running = 1;
    char buffer[BUFFER_SIZE], *p = NULL;
    int mysocket, n = 0;
    SOCKADDR_IN sin = { 0 };
    struct hostent *hostinfo;
    ClientState* clientState = client_init();
    

    fd_set rdfs;

    /*initialisation*/
    mysocket= socket(AF_INET, SOCK_STREAM, 0);
    if(mysocket == -1){
        perror("socket()");
        exit(errno);
    }

    hostinfo = gethostbyname(address);

    if (hostinfo == NULL){
        fprintf (stderr, "Unknown host %s.\n", address);
        exit(EXIT_FAILURE);
    }

    sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
    sin.sin_port = htons(1234);
    sin.sin_family = AF_INET;

    if(connect(mysocket,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == -1){
        perror("connect()");
        exit(errno);
    }

    /**/

    while(running){
        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        FD_SET(mysocket, &rdfs);
        client_display(clientState);
        if(select(mysocket + 1, &rdfs, NULL, NULL, NULL) == -1){
            perror("select()");
            exit(errno);
        }

        if(FD_ISSET(STDIN_FILENO, &rdfs)){
            fgets(buffer, BUFFER_SIZE - 1, stdin);
            {
                p = strstr(buffer, "\n");
                if(p != NULL){
                    *p = 0;
                } else {
                    buffer[BUFFER_SIZE - 1] = 0;
                }
            }
            client_treat_input(clientState, buffer, mysocket);

        } else if(FD_ISSET(mysocket, &rdfs)){
            client_treat_message(clientState, mysocket);
        }
    }
    client_destroy(clientState);
    close(mysocket);
}

int main(int argc, char **argv){
    //if(argc < 2){
    //    printf("Too few argument (adresse) \n");
    //    return -1;
    //}

    //connexion(argv[1]);
    connexion("192.168.0.40");


    return 0;
}



#endif // MODE_CLIENT