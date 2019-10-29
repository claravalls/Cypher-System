//Practica SISTEMES OPERATIUS
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "logica.h"
#include "manager.h"


Config config;


int main(int argc, const char* argv[]){
    char opcio, comanda[50];
    char aux[50] = {0x0};

    if (argc < 2){
        write(1, "Not enough arguments\n", strlen("Not enough arguments\n"));
    }

    signal(SIGINT, stopAll);
    config = lecturaFitxer(argv[1]);

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
                optionConnect();
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