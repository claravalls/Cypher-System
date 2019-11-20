#include "thread.h"

static void *threadFunc (void *config){
    int newsock;
    int socket;
    char *clientName;
    Config *c = (Config *) config;
    
    while (1){
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        socket = c->sockfd;
        newsock = accept (socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            
            /*
            *A QUI LI HAIG D'ENVIAR EL PAQUET??
            p.type = 0x01;
            p.header = (char*) malloc(sizeof(char) * 8);
            p.header = "[CONKO]";
            p.length = 0
            
            enviaPaquet(newsock, p);*/

            return (void *) -1;
        }
        clientName = llegeixPaquet(newsock);
        if (clientName == NULL){
            //És una connexió falsa. La tanquem
            close(newsock);
        }
        else {
            afegeixClient(newsock, c->user, clientName);
        }
    }
    return (void *) c;
}

void iniciaThread(Config* config){
    pthread_t t1;
    pthread_create(&t1, NULL, threadFunc, config);
}