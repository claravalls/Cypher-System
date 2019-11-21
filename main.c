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
    int sockfd;
    char ** valors; //valors introduits a la comanda

    if (argc != 2){
        write(1, ERR_ARGS, strlen(ERR_ARGS));
    }

    signal(SIGINT, stopAll);

    config = lecturaFitxer(argv[1]);

    setConfig(config);

    sockfd = connectServer(config.ip, config.port);
    if (sockfd < 0) {
        write(1, ERR_SOCKET, strlen(ERR_SOCKET));
        return -1;
    }

    setSockfd(sockfd);
    config.sockfd = sockfd;

    iniciaThreadServidor(&config); 
     
    if (config.user == NULL)
    {
    	return -1;
    }

    write(1, STARTING, strlen(STARTING));

    do{  
        imprimeixPrompt(config);
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
                optionSay(getValues());
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
                write(1, ERR_OPTION, strlen(ERR_OPTION));
        }

        free(comanda);
        alliberaMemoriaC();

    }while(opcio != EXIT);

    alliberaMemoriaConfig(&config);

    return 0;
}