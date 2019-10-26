//Practica SISTEMES OPERATIUS
//Clara Valls - clara.valls
//Ariel Andreas Daniele - arielandreas.daniele

#include "practica.h"

char ** c;
char sizeofc = 0;
Config config;

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

    //alliberem memoria
    free(config.user);
    free(config.dirAudios);
    free(config.ip);
    free(config.sysports);
    free(config.ipWeb);
    
    //desconnectem sockets

    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
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

void lecturaFitxer(const char *fitxer){
	ssize_t nbytes;
	unsigned char cadena;
	int i = 0, f;
    char * aux;

	//Lectura fitxer
    f = open(fitxer, O_RDONLY);
    if (f < 0)
    {
    	write(1, "Error opening file...\n", strlen("Error opening file...\n"));
		config.user = NULL;
	}

	nbytes = read(f, &cadena, 1);

	while (nbytes > 0)
	{
		//User
		i = 0;
		config.user = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
            config.user[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			config.user = (char *)realloc(config.user, sizeof(char) * (i + 1));
		}
		config.user[i] = '\0';

		//DirAudios
		nbytes = read(f, &cadena, 1);

		i = 0;
		config.dirAudios = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
			config.dirAudios[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			config.dirAudios = (char *)realloc(config.dirAudios, sizeof(char) * (i + 1));
		}
		config.dirAudios[i] = '\0';

		//IP
		nbytes = read(f, &cadena, 1);

		i = 0;
		config.ip = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
			config.ip[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			config.ip = (char *)realloc(config.ip, sizeof(char) * (i + 1));
		}
		config.ip[i] = '\0';

		//Port
		nbytes = read(f, &cadena, 1);

		i = 0;
		aux = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
			aux[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			aux = (char *)realloc(aux, sizeof(char) * (i + 1));
		}
		aux[i] = '\0';

		config.port = atoi(aux);
        free(aux);

		//ipWeb
		nbytes = read(f, &cadena, 1);

		i = 0;
		config.ipWeb = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
			config.ipWeb[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			config.ipWeb = (char *)realloc(config.ipWeb, sizeof(char) * (i + 1));
		}
		config.ipWeb[i] = '\0';

		//Sysport Begin
		nbytes = read(f, &cadena, 1);

		i = 0;
		config.sysports = (int *)malloc(sizeof(int) * 2);

        aux = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
			aux[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			aux = (char *)realloc(aux, sizeof(char) * (i + 1));
		}
        aux[i] = '\0';

		config.sysports[0] = atoi(aux);
        free(aux);

		//Sysport End
		nbytes = read(f, &cadena, 1);

		i = 0;

        aux = (char *)malloc(sizeof(char));
		while (cadena != '\n' && nbytes > 0)
		{
			aux[i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			aux = (char *)realloc(aux, sizeof(char) * (i + 1));
		}
        aux[i] = '\0';

		config.sysports[1] = atoi(aux);
        free(aux);
        

        if (nbytes != 0) {
            nbytes = read(f, &cadena, 1);
        }
	}

	//printf("\n$%s: ",config.user);
	/*printf("Dir: %s\n",c.dirAudios);
	printf("IP: %s\n",c.ip);
	printf("Port: %d\n",c.port);
	printf("ip Web: %s\n",c.ipWeb);
	printf("Sysport begin: %d\n",c.sysports[0]);
	printf("Sysport end: %d\n",c.sysports[1]);*/

	close(f);
}

void stopAll(){
    optionExit();
    kill(getpid(), SIGINT);
}


int main(int argc, const char* argv[]){
    char opcio, comanda[50];
    char aux[50] = {0x0};
    int fd;

    if (argc < 2){
        write(1, "Not enough arguments\n", strlen("Not enough arguments\n"));
    }

    signal(SIGINT, stopAll);
    lecturaFitxer(argv[1]);

    if (config.user == NULL)
    {
    	return -1;
    }

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