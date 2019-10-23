//Practica SISTEMES OPERATIUS
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "practica.h"

char ** c;
char sizeofc = 0;

/**
 * comanda: cadena sencera llegida
 * limit: caracter fins on hem de separar
 * i: casella inicial de la cadena comanda on començar a separar
 * casella: casella de l'array on guardar el valor separat
*/
void separaComanda(char *comanda, char limit, char i, int casella){
    char j = 0;

    while(comanda[i] != limit && comanda[i] != '\n'){
        c[casella][j] = comanda[i];
        i++;
        j++;
    }

    c[casella][j] = '\0';

    if (strcasecmp(c[casella], "SHOW") == 0){
        c[casella][j] = ' ';
        i++;
        j++;
        while(comanda[i] != ' ' && comanda[i] != '\n'){
            c[casella][j] = comanda[i];
            i++;
            j++;
        }

        c[casella][j] = '\0';
    }
}

char llegeixComanda(char *comanda){
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
    c = (char **)malloc(sizeof(char*));
    c[0] = (char *)malloc(sizeof(char) * 20);

    separaComanda(comanda, ' ', 0, 0);

    if (strcasecmp(c[0], "SHOW CONNECTIONS") == 0){
        opcio = 1;
    }
    else if (strcasecmp(c[0], "SHOW AUDIOS") == 0){
        opcio = 5;
        sizeofc = 1;
        c = (char **)realloc(c, sizeof(char*) * 2);
        c[1] = (char *)malloc(sizeof(char) * 5);
        separaComanda(comanda, ' ', strlen("SHOW AUDIOS") + 1, 1);
    }
    
    else if (strcasecmp(c[0], "CONNECT") == 0){
        opcio = 2;
        sizeofc = 1;
        c = (char **)realloc(c, sizeof(char*) * 2);
        c[1] = (char *)malloc(sizeof(char) * 5);
        separaComanda(comanda, ' ', strlen("CONNECT") + 1, 1);
    }
    else if (strcasecmp(c[0], "SAY") == 0){
        opcio = 3;
        sizeofc = 2;
        //user
        c = (char **)realloc(c, sizeof(char*) * 2);
        c[1] = (char *)malloc(sizeof(char) * 5);
        separaComanda(comanda, ' ', strlen("SAY") + 1, 1);

        //text
        c = (char **)realloc(c, sizeof(char*) * 3);
        c[2] = (char *)malloc(sizeof(char) * 100);
        separaComanda(comanda, '\n', strlen("SAY") + strlen(c[1]) + 2, 2);
    }
    else if (strcasecmp(c[0], "BROADCAST") == 0){
        opcio = 4;
        sizeofc = 1;
        c = (char **)realloc(c, sizeof(char*) * 2);
        c[1] = (char *)malloc(sizeof(char) * 5);
        separaComanda(comanda, '\n', strlen("BROADCAST") + 1, 1);
    }
    else if (strcasecmp(c[0], "DOWNLOAD") == 0){
        opcio = 6;
        sizeofc = 2;
        //user
        c = (char **)realloc(c, sizeof(char*) * 2);
        c[1] = (char *)malloc(sizeof(char) * 5);
        separaComanda(comanda, ' ', strlen("DOWNLOAD") + 1, 1);

        //audio
        c = (char **)realloc(c, sizeof(char*) * 3);
        c[2] = (char *)malloc(sizeof(char) * 100);
        separaComanda(comanda, '\n', strlen("DOWNLOAD") + strlen(c[1]) + 2, 2);
    }
    else if (strcasecmp(c[0], "EXIT") == 0){
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

void optionDownload(){

}


int main(int argc, const char* argv[]){
    char opcio, comanda[50];
    char aux[50] = {0x0};
    int fd;


    //Lectura fitxer


    do{
        sprintf(aux,"$%s: ", argv[1]);
        write(1,aux,strlen(aux));

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
                write(1,"Error, opció invàlida\n", strlen("Error, opció invàlida\n"));
        }

        //alliberem memòria de c
        for (char i = 0; i <= sizeofc; i++)
        {
            free(c[i]);
        }
        
        free(c);

    }while(opcio != EXIT);

    return 0;
}

//CANVIAR LA SIGNAL DE Ctrl+C perque primer alliberi tota la memoria i desconnecti el sockets i despres es tanqui