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

int main(int argc, const char* argv[]){
    char opcio;

    opcio = llegeixComanda(*argv);
    printf("%c", opcio);
    return 0;
}