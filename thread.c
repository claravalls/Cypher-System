#include "thread.h"

static void *threadServ (void *config){
    int newsock;
    int socket;
    char *clientName;
    Config *c = (Config *) config;
    Protocol p;
    
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
        p = llegeixPaquet(newsock);
        clientName = p.data;
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

static void *threadCli (void *client){
    Conn_cli *c = (Conn_cli *) client;
    Protocol p;
    
    while (1){
        p = llegeixPaquet(c->sockfd);
        switch(p.type){
            case 0x02:
                if(strcmp(p.header, "[MSG]") == 0){
                    imprimeixMissatge(p.data, c->user);
                    imprimeixPrompt();
                }
                break;

            case 0x03:
                break;

            case 0x04:
                break;

            case 0x05:
                break;

            case 0x06:
                break;
        }
    }
    return (void *) c;
}

void iniciaThreadServidor(Config* config){
    pthread_t t1;
    pthread_create(&t1, NULL, threadServ, config);
}

void iniciaThreadClient(Conn_cli* client){
    pthread_t t1;
    pthread_create(&t1, NULL, threadCli, client);
}

