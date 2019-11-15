//Practica SISTEMES OPERATIUS
//Fase actual: 2
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "logica.h"
#include "manager.h"
#include "network.h"

Config config;

static void *threadFunc (void *sockfd){
    int newsock;
    
    while (1){
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        int *socket = (int*) malloc(sizeof(int));
        socket = (int*) (sockfd);
        newsock = accept (*socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            break;
        }
        free(socket);
        afegeixClient(newsock);
    }
    return (void *) sockfd;
}

int main(int argc, const char* argv[]){
    char opcio, comanda[50];
    char aux[50] = {0x0};
    int sockfd;
    pthread_t t1;

    if (argc < 2){
        write(1, "Not enough arguments\n", strlen("Not enough arguments\n"));
    }

    signal(SIGINT, stopAll);

    config = lecturaFitxer(argv[1]);

    sockfd = connectServer(config.ip, config.port);
    if (sockfd < 0) {
        write(1, ERR_SOCKET, strlen(ERR_SOCKET));
        return -1;
    }

    pthread_create(&t1, NULL, threadFunc, &sockfd);  

    if (config.user == NULL)
    {
    	return -1;
    }

    write(1, "\nStarting Trinity...\n", strlen("\nStarting Trinity...\n"));


    do{
        sprintf(aux, "\n$%s: ", config.user);
        write(1, aux, strlen(aux));

        //Llegir opcio introduida
        strcpy(comanda, "");
        read(0, comanda, 120);
        opcio = llegeixComanda(comanda);

        switch(opcio){
            case SHOW_CONNECTIONS:
                optionShowC();
            break;

            case CONNECT:
                optionConnect(config.port, config.ip, config.user, sockfd);
            break;

            case SAY:
                optionSay();
            break;

            case BROADCAST:
                optionBroadcast();
            break;  

            case SHOW_AUDIOS:
                optionShowA();
            break;

            case DOWNLOAD:
                optionDownload();
            break;

            case EXIT:
                optionExit();
            break;

            default:
                write(1,"Error, invalid option\n", strlen("Error, invalid option\n"));
        }

        alliberaMemoriaC();

    }while(opcio != EXIT);

    alliberaMemoriaConfig(&config);

    return 0;
}