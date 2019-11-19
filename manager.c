#include "manager.h"


char ** c;
char sizeofc;

char* readUntil(int fd, char end, char lastChar) {
    int i = 0;
    char c = '\0';
    char* string = (char*)malloc(sizeof(char));

    while (c != end) {
        read(fd, &c, sizeof(char));
        if (c != end) {
            string = (char*)realloc(string, sizeof(char) * (i + 2));
            string[i] = c;
        }
        i++;
    }

	string[i - 1] = lastChar;
    return string;
}

/**
 * comanda: cadena sencera llegida
 * limit: caracter fins on hem de separar
 * i: casella inicial de la cadena comanda on començar a separar
 * casella: casella de l'array on guardar el valor separat
*/
void separaComanda(char *comanda, char limit, int i, int casella){
    int j = 0;

    if(comanda[i] == ' '){ //em salto l'espai entre paraules
        i++;
    }

    if(comanda[i] == '\n'){
        write(1, "Not enough arguments.\n", strlen("Not enough arguments.\n"));
        c[casella][0] = '\0';
    }
    else{
        while(comanda[i] != limit && comanda[i] != '\n' && j < 20){
            c[casella][j] = comanda[i];
            
            //printf("%d %c\n", c[casella][j], c[casella][j]);
            i++;
            j++;
            c[casella] = (char *)realloc(c[casella], sizeof(char) * j);
        }

        c[casella][j] = '\0';

        if (strcasecmp(c[casella], "SHOW") == 0){
            c[casella][j] = ' ';
            i++;
            j++;
            while(comanda[i] != ' ' && comanda[i] != '\n'&& j < 20){
                c[casella][j] = comanda[i];
                i++;
                j++;
            }

            c[casella][j] = '\0';
        }
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
    c[0] = (char *)malloc(sizeof(char));

    separaComanda(comanda, ' ', 0, 0);

    if (strcasecmp(c[0], "SHOW CONNECTIONS") == 0){
        sizeofc = 0;
        opcio = 1;
    }
    else if (strcasecmp(c[0], "SHOW AUDIOS") == 0){
        opcio = 5;
        sizeofc = 1;
        c = (char **)realloc(c, sizeof(char*) * 2);
        
        c[1] = (char *)malloc(sizeof(char));
        separaComanda(comanda, ' ', strlen("SHOW AUDIOS"), 1);
        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
    }
    
    else if (strcasecmp(c[0], "CONNECT") == 0){
        opcio = 2;
        sizeofc = 1;
        c = (char **)realloc(c, sizeof(char*) * 2);

        c[1] = (char *)malloc(sizeof(char));
        separaComanda(comanda, ' ', strlen("CONNECT"), 1);

        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
    }
    else if (strcasecmp(c[0], "SAY") == 0){
        opcio = 3;
        sizeofc = 2;
        //user
        c = (char **)realloc(c, sizeof(char*) * 2);

        c[1] = (char *)malloc(sizeof(char));
        separaComanda(comanda, ' ', strlen("SAY"), 1);
        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
        else{
            //text
            c = (char **)realloc(c, sizeof(char*) * 3);
            c[2] = (char *)malloc(sizeof(char));
            separaComanda(comanda, '\n', strlen("SAY") + strlen(c[1]) + 1, 2);
            //ens assegurem que hi hagi els arguments necessaris
            if(c[2][0] == '\0'){
                opcio = 0;
            }
        }
    }
    else if (strcasecmp(c[0], "BROADCAST") == 0){
        opcio = 4;
        sizeofc = 1;
        c = (char **)realloc(c, sizeof(char*) * 2);

        c[1] = (char *)malloc(sizeof(char));
        separaComanda(comanda, '\n', strlen("BROADCAST"), 1);
        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
    }
    else if (strcasecmp(c[0], "DOWNLOAD") == 0){
        opcio = 6;
        sizeofc = 2;
        //user
        c = (char **)realloc(c, sizeof(char*) * 2);

        c[1] = (char *)malloc(sizeof(char));
        separaComanda(comanda, ' ', strlen("DOWNLOAD"), 1);
        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
        else{
            //audio
            c = (char **)realloc(c, sizeof(char*) * 3);
            c[2] = (char *)malloc(sizeof(char));
            separaComanda(comanda, '\n', strlen("DOWNLOAD") + strlen(c[1]) + 1, 2);
            //ens assegurem que hi hagi els arguments necessaris
            if(c[2][0] == '\0'){
                opcio = 0;
            }
        }
    }
    else if (strcasecmp(c[0], "EXIT") == 0){
        opcio = 7;
        sizeofc = 0;
    }
    
    return opcio;
}

Config lecturaFitxer(const char *fitxer){
	ssize_t nbytes;
	unsigned char cadena;
	int i = 0, f;
    char * aux;
    Config config;

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
		config.sysports = (char **)malloc(sizeof(char) * 2);

        config.sysports[0] = (char *)malloc(sizeof(char));
		while (cadena != '\n')
		{
			config.sysports[0][i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			config.sysports[0] = (char *)realloc(config.sysports[0], sizeof(char) * (i + 1));
		}
        config.sysports[0][i] = '\0';

		//Sysport End
		nbytes = read(f, &cadena, 1);

		i = 0;

        config.sysports[1] = (char *)malloc(sizeof(char));
		while (cadena != '\n' && nbytes > 0)
		{
			config.sysports[1][i] = cadena;
			i++;
			nbytes = read(f, &cadena, 1);
			config.sysports[1] = (char *)realloc(config.sysports[1], sizeof(char) * (i + 1));
		}
        config.sysports[1][i] = '\0';        

        if (nbytes != 0) {
            nbytes = read(f, &cadena, 1);
        }
	}

	close(f);
    return config;
}



void alliberaMemoriaC(){
    //alliberem memòria de c
    for (int i = 0; i <= sizeofc; i++)
    {
        free(c[i]);
    }
    
    free(c);
}


void alliberaMemoriaConfig(Config *config){
    //alliberem memoria
    free(config->user);
    free(config->dirAudios);
    free(config->ip);
    free(config->sysports);
    free(config->ipWeb);
}

char ** getValues(){
    return c;
}

void buscaPorts(int pipe, int myPort){
    ssize_t nbytes;
	unsigned char c;
    char *port, *extra, *connexio;

    nbytes = read(pipe, &c, 1);
    while (nbytes > 0){
        //llegeixo "port"
        extra = readUntil(pipe, ' ', '\n');
        free(extra);

        //llegeixo el numero del port
        port = readUntil(pipe, ' ', '\0');
        //comprovo si se el nom de l'usuari
        connexio = comprovaNomUsuari(port, myPort);

        if(connexio != NULL) //comprovem que no es mostri el nostre port
            write(1, connexio, strlen(connexio));

        //llegeixo el que queda
        extra = readUntil(pipe, '\n', '\n');
        free(extra);

        nbytes = read(pipe, &c, 1);
    }
}