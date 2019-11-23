//Practica SISTEMES OPERATIUS
//Fase actual: 2
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "logica.h"
#include "manager.h"
#include "network.h"
#include "thread.h"

Config config;      //valors del fitxer de configuració

int main(int argc, const char* argv[]){
    char opcio;         //valors de la opció introduida
    char* comanda;      //cadena de la comanda entrada
    int sockfd;         //sockfd del meu servidor
    char ** valors;     //valors introduits a la comanda

    if (argc != 2){
        write(1, ERR_ARGS, strlen(ERR_ARGS));
    }

    //reconfigurem la senyal SIGINT
    signal(SIGINT, stopAll);

    //llegim el fitxer de configuració
    config = lecturaFitxer(argv[1]);

    //comprovem que s'ha llegit correctament
    if (config.user == NULL)
    	return -1;

    //iniciem la connexió del servidor
    sockfd = connectServer(config.ip, config.port);
    if (sockfd < 0) {
        write(1, ERR_SOCKET, strlen(ERR_SOCKET));
        return -1;
    }

    //guardem el valor del nostre socket
    setSockfd(sockfd);
    config.sockfd = sockfd;

    //guardem els valors llegits
    setConfig(config);

    //iniciem el thread del servidor per acceptar connexions
    iniciaThreadEscolta(&config); 

    write(1, STARTING, strlen(STARTING));

    do{  
        imprimeixPrompt(config);
        
        //Llegim opcio introduida
        comanda = readUntil(0, '\n', '\n');
        opcio = llegeixComanda(comanda);
        //obtenim els valors introduits a la comanda
        valors = getValues();

        switch(opcio){
            case SHOW_CONNECTIONS:
                optionShowC(config.sysports, config.port);
            break;

            case CONNECT:
                optionConnect(valors[1], config.ip, config.user);
            break;

            case SAY:
                optionSay(valors);
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
                optionExit(config.user);
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