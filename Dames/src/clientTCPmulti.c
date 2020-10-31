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

#define BUFFER_SIZE 1024
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;


void connexion(const char *address, const char *pseudo){
    int running = 1;
    char buffer[BUFFER_SIZE], *p = NULL;
    int mysocket, n = 0;
    SOCKADDR_IN sin = { 0 };
    struct hostent *hostinfo;
    

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

    if(send(mysocket, pseudo, strlen(pseudo), 0) < 0){
        perror("send()");
        exit(errno);
    }
    /**/

    while(running){
        FD_ZERO(&rdfs);
        FD_SET(STDIN_FILENO, &rdfs);
        FD_SET(mysocket, &rdfs);

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
            
            if(send(mysocket, buffer, strlen(buffer), 0) < 0){
                perror("send()");
                exit(errno);
            }

        } else if(FD_ISSET(mysocket, &rdfs)){

            n = 0;
            if((n = recv(mysocket, buffer, BUFFER_SIZE - 1, 0)) < 0){
                perror("recv()");
                exit(errno);
            }
            buffer[n] = 0;

            if(n == 0){
                printf("Serveur dÃ©connectÃ© !\n");
                break;
            }
            puts(buffer);
        }
    }
    close(mysocket);
}

int main(int argc, char **argv){
    if(argc < 2){
        printf("Too few argument (adresse + pseudo) \n");
        return -1;
    }

    connexion(argv[1], argv[2]);

    return 0;
}