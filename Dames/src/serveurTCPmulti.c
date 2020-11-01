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

#include "network/server/treatMessage.h"
#include "network/server/server.h"
#include "network/server/login.h"

typedef struct {
    int mysocket;
    char pseudo[1024];
}Joueur;


#define BUFFER_SIZE 1024
#define MAX_JOUEURS 3
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

void server(void){

    int mysocket;
    SOCKADDR_IN csin = { 0 };
    char buffer[BUFFER_SIZE];
    int running = 1, actuel = 0, max, n = 0; 
    Joueur joueurs[MAX_JOUEURS];
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

    if(listen(mysocket, MAX_JOUEURS) == -1){
        perror("listen()");
        exit(errno);
    }

    max = mysocket;
    fd_set rdfs;

    while(running){
        
        FD_ZERO(&rdfs);

            
        FD_SET(STDIN_FILENO, &rdfs);

            
        FD_SET(mysocket, &rdfs);

            
        for(int i = 0; i < actuel; i++){
            FD_SET(joueurs[i].mysocket, &rdfs);
        }

        if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1){
            perror("select()");
            exit(errno);
        }

            
        if(FD_ISSET(STDIN_FILENO, &rdfs)){
        
            break;
        }
        else if(FD_ISSET(mysocket, &rdfs)){
        
            
            socklen_t sinsize = sizeof csin;
            int csock = accept(mysocket, (SOCKADDR *)&csin, &sinsize);
            if(csock == -1){
                perror("accept()");
                continue;
            }

            n = 0;

            if((n = recv(csock, buffer, BUFFER_SIZE - 1, 0)) < 0){
                perror("recv()");
                n = 0;
            }
            buffer[n] = 0;

            max = csock > max ? csock : max;

            FD_SET(csock, &rdfs);

            Joueur j = { csock };
            strncpy(j.pseudo, buffer, BUFFER_SIZE - 1);
            joueurs[actuel] = j;
            actuel++;
        } else {
            for(int i = 0; i < actuel; i++){
                    
                if(FD_ISSET(joueurs[i].mysocket, &rdfs)){
                    
                    if(!server_treat_message(serverState, joueurs[1].mysocket)){
                        perror("server_treat_message()");
                        exit(errno);
                    }

                    
                }
            }
        }
    }

    for(int i = 0; i < actuel; i++){
        close(joueurs[i].mysocket);
    }
    close(mysocket);
    server_destroy(serverState);
}


int main(int argc, char **argv){
   server();

   return 0;
}