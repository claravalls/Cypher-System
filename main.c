//Practica SISTEMES OPERATIUS
//Fase actual: 2
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "logica.h"
#include "manager.h"
#include "network.h"
#include "thread.h"

Config config;

int main(int argc, const char* argv[]){
    char opcio;
    char* comanda;
    char aux[50] = {0x0};
    int sockfd;
    char ** valors; //valors introduits a la comanda

    if (argc != 2){
        write(1, "Not enough arguments\n", strlen("Not enough arguments\n"));
    }

    signal(SIGINT, stopAll);

    config = lecturaFitxer(argv[1]);

    sockfd = connectServer(config.ip, config.port);
    if (sockfd < 0) {
        write(1, ERR_SOCKET, strlen(ERR_SOCKET));
        return -1;
    }

    setSockfd(sockfd);
    config.sockfd = sockfd;

    iniciaThread(&config); 
     
    if (config.user == NULL)
    {
    	return -1;
    }

    write(1, "\nStarting Trinity...\n", strlen("\nStarting Trinity...\n"));

    do{
        sprintf(aux, "\n$%s: ", config.user);
        write(1, aux, strlen(aux));

        //Llegir opcio introduida
        comanda = readUntil(0, '\n', '\n');
        opcio = llegeixComanda(comanda);
        valors = getValues();

        switch(opcio){
            case SHOW_CONNECTIONS:
                optionShowC(config.sysports, config.port);
            break;

            case CONNECT:
                optionConnect(valors[1], config.ip, config.user);
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
                optionExit(sockfd);
            break;

            default:
                write(1,"Error, invalid option\n", strlen("Error, invalid option\n"));
        }

        free(comanda);
        alliberaMemoriaC();

    }while(opcio != EXIT);

    alliberaMemoriaConfig(&config);

    return 0;
}