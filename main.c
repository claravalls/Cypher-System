//Practica SISTEMES OPERATIUS
//Fase actual: 4
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "logica.h"
#include "manager.h"
#include "network.h"
#include "thread.h"

extern Config config;      		//valors del fitxer de configuració
extern char ** c; 			    //valors introduits a la comanda
char* comanda;                  //cadena de la comanda entrada


int main(int argc, const char* argv[]){
    char opcio;         //valors de la opció introduida
    int sockfd;         //sockfd del meu servidor

    if (argc != 2){
        escriuTerminal(ERR_ARGS);
        return -1;
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
        escriuTerminal(ERR_SOCKET);
        alliberaMemoriaConfig(&config);
        return -1;
    }

    //guardem el valor del nostre socket
    setSockfd(sockfd);
    config.sockfd = sockfd;

    //iniciem el thread del servidor per acceptar connexions
    iniciaThreadEscolta(&config); 

    escriuTerminal(STARTING);

    do{  
        imprimeixPrompt(config);
        
        //Llegim opcio introduida
        comanda = readUntil(0, '\n', '\n');
        opcio = llegeixComanda(comanda);

        switch(opcio){
            case SHOW_CONNECTIONS:
                optionShowC(config.sysports, config.port, config.ip);
            break;

            case CONNECT:
                optionConnect(c[1], config.ip, config.user);
            break;

            case SAY:
                optionSay(c);
            break;

            case BROADCAST:
                optionBroadcast(c[1]);
            break;  

            case SHOW_AUDIOS:
                optionShowA(c[1]);
            break;

            case DOWNLOAD:
                optionDownload(c[1], c[2]);
            break;

            case EXIT:
                optionExit(config.user);
            break;

            default:
                escriuTerminal(ERR_OPTION);
        }

        free(comanda);
        alliberaMemoriaC();

    }while(opcio != EXIT);

    return 0;
}