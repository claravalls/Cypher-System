//Practica SISTEMES OPERATIUS
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele




#include "practica.h"

char llegeixComanda(char * comanda){
    char opcio = 0;
    /*
        1 - SHOW CONNECTIONS
        2 - CONNECT port
        3 - SAY user text
        4 - BROADCAST text
        5 - SHOW AUDIOS user
        6 - DOWNLOAD
        7 - EXIT
    */
    if (srtcasecmp(comanda[0], "SHOW")){
        if (srtcasecmp(comanda[1], "CONNECTIONS")){
            opcio = 1;
        }
        else if (srtcasecmp(comanda[1], "AUDIOS")){
            opcio = 5;
        }
    }
    else if (srtcasecmp(comanda[0], "CONNECT")){
        opcio = 2;
    }
    else if (srtcasecmp(comanda[0], "SAY")){
        opcio = 3;
    }
    else if (srtcasecmp(comanda[0], "BROADCAST")){
        opcio = 4;
    }
    else if (srtcasecmp(comanda[0], "DOWNLOAD")){
        opcio = 6;
    }
    else if (srtcasecmp(comanda[0], "EXIT")){
        opcio = 7;
    }
    return opcio;
}


void optionExit(){
    write(1,"Disconnecting Trinity...\n", strlen("Disconnecting Trinity...\n"));
}

void optionConnect(){
    write(1,"Connecting...\n", strlen("Connecting...\n"));

}


void optionSay(){


}

void optionShowC(){
    write(1,"Testing...\n", strlen("Testing...\n"));

}

void optionShowA(){


}


void optionBroadcast(){


}


int main(int argc, const char* argv[]){
    char opcio;
    char aux[50] = {0x0};
    int fd;

    opcio = llegeixComanda(*argv);
    printf("%c", opcio);

    do{
        sprintf(aux,"$%s: ", nom_del_config.dat);
        write(1,aux,strlen(aux));

        //READ OPCIO INTRODUIDA

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
                write(1,"Error, opció invàlida\n", strlen("Error, opció invàlida\n"));

        }while(opcio != EXIT);

    return 0;
}