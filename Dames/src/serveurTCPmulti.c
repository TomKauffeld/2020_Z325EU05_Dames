#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SOCKET_PROTOCOL_TCP 6
#define BUFFER_SIZE 64

typedef struct {
    int socket;
    char nom[BUFFER_SIZE];
}Joueur;

#define TCP_CONNECTIONS 5


int main(int argc, char** argv)
{
    uint16_t port = 1234;
    struct sockaddr_in sin;
    int i, ret, s, client, running = 1, nbclient = 0, socket;
    char buffer[BUFFER_SIZE];
    size_t len;
    Joueur joueur[2];
    fd_set fd;
    sockaddr_in sain = { 0 };

    len = sizeof sain;
    
    if (argc > 1)
        sscanf(argv[1], "%hu", &port);
    printf("Utilisation du port %hu\n", port);
    
    
    s = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCOL_TCP);
    
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    for (i=0; i<8; i++) {
        sin.sin_zero[i] = 0;
    }
    if (s < 0)
    {
        printf("Erreur dans l'ouverture du socket\n");
        return 1;
    }

    
    if (bind(s,(struct sockaddr *)&sin, sizeof sin) < 0)
    {
        close(s);
        printf("Erreur de bind avec le socket\n");
        return 2;
    }
    
    if (listen(s, TCP_CONNECTIONS) < 0)
    {
        close(s);
        printf("Erreur d'ecoute sur le socket\n");
        return 2;
    }

    while(running)
    {
        FD_ZERO(&fd);
        FD_SET(STDIN_FILENO, &fd);
        FD_SET(s, &fd);
        
        for(int i = 0; i< nbclient; i++){
            FD_SET(joueur[i].sock, &fd);
        }

        if(select(s + 1, &fd, NULL, NULL, NULL) < 0){
            close(s);
            printf("erreur select");
            return 2;
        }

        if(FD_ISSET(STDIN_FILENO, &fd)){
            break;
        }
        else if(FD_ISSET(s, &fd)){
            socket = accept(s, (sockaddr * )&sain, &len);
            if(socket < 0){
                printf("erreur accept");
                continue;
            }
            

        }


    }


    close(s);
    return 0;
}