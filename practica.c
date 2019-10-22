#include "practica.h"

char llegeixComanda(const char * argv[]){
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
    if (strcasecmp(argv[1], "SHOW") == 0){
        if (strcasecmp(argv[2], "CONNECTIONS") == 0){
            opcio = 1;
        }
        else if (strcasecmp(argv[2], "AUDIOS") == 0){
            opcio = 5;
        }
    }
    else if (strcasecmp(argv[1], "CONNECT") == 0){
        opcio = 2;
    }
    else if (strcasecmp(argv[1], "SAY") == 0){
        opcio = 3;
    }
    else if (strcasecmp(argv[1], "BROADCAST") == 0){
        opcio = 4;
    }
    else if (strcasecmp(argv[1], "DOWNLOAD") == 0){
        opcio = 6;
    }
    else if (strcasecmp(argv[1], "EXIT") == 0){
        opcio = 7;
    }
    return opcio;
}

int main(int argc, const char* argv[]){
    char opcio;

    opcio = llegeixComanda(argv);
    return 0;
}