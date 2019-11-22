#include "thread.h"

char apaga = 1;

static void *threadServ (void *config){
    int newsock;                    //socket que vol connectar-se
    int socket;                     //socket del meu servidor
    char *clientName;               //nom del client que es connecta
    Config *c = (Config *) config;  //valors del fitxer de configuració
    Protocol p;                     //protocol de comunicació
    
    while (apaga){
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        socket = c->sockfd;
        newsock = accept (socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            return (void *) -1;
        }

        //esperem el missatge del client que s'ha connectat
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

void apagaServidor(){
    apaga = 0;
}

static void *threadCli (void *client){
    Conn_cli *c = (Conn_cli *) client;      //informació del client que s'ha connectat
    Protocol p;                             //protocol de comunicació
    char connectat = 1;                     //variable que indica quan aturar el thread
    
    while (connectat){
        //escoltem si el client ens envia un missatge
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
                //tancar la connexio
                p = llegeixPaquet(c->sockfd);

                //connectat = 0;
                //close(c->sockfd);
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

