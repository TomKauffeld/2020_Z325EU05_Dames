#include "Header.h"
#ifdef MODE_SERVEUR

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

#include "network/server/serverTreatMessage.h"
#include "network/server/server.h"
#include "network/server/login.h"



#define MAX_WAITING 3
#define MAX_JOUEURS 200
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

void server(void){

    int mysocket, i;
    SOCKADDR_IN csin = { 0 };
    int running = 1, max; 
    int sockets[MAX_JOUEURS];
    memset(sockets, 0, sizeof(int) * MAX_JOUEURS);
    ServerState *serverState = server_init();


    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = { 0 };

    if(mysocket == -1){
        perror("socket()");
        exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(1234);
    sin.sin_family = AF_INET;

    if(bind(mysocket,(SOCKADDR *) &sin, sizeof sin) == -1){
        perror("bind()");
        exit(errno);
    }

    if(listen(mysocket, MAX_WAITING) == -1){
        perror("listen()");
        exit(errno);
    }

    max = mysocket;
    fd_set rdfs;

    while(running){
        
        FD_ZERO(&rdfs);

            
        FD_SET(STDIN_FILENO, &rdfs);

            
        FD_SET(mysocket, &rdfs);

            
        for(i = 0; i < MAX_JOUEURS; i++){
            if (sockets[i] > 0)
                FD_SET(sockets[i], &rdfs);
        }

        if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1){
            perror("select()");
            exit(errno);
        }

        
            
        if(FD_ISSET(STDIN_FILENO, &rdfs)){
            break;
        } else if(FD_ISSET(mysocket, &rdfs)){
        
            
            socklen_t sinsize = sizeof csin;
            int csock = accept(mysocket, (SOCKADDR *)&csin, &sinsize);
            if(csock == -1){
                perror("accept()");
                continue;
            }


            max = csock > max ? csock : max;

            FD_SET(csock, &rdfs);

            for (i = 0; i < MAX_JOUEURS; i++)
                if (sockets[i] <= 0)
                {
                    sockets[i] = csock;
                    break;
                }
        } else {
            for(i = 0; i < MAX_JOUEURS; i++){
                    
                if(sockets[i] > 0 && FD_ISSET(sockets[i], &rdfs)){
                    
                    if(!server_treat_message(serverState, sockets[i])){
                        close(sockets[i]);
                        sockets[i] = -1;
                    }
                    
                }
            }
        }
    }

    for(int i = 0; i < MAX_JOUEURS; i++){
        if (sockets[i] > 0)
            close(sockets[i]);
    }
    close(mysocket);
    server_destroy(serverState);
}


int main(int argc, char **argv){
   server();

   return 0;
}

#endif // MODE_SERVEUR