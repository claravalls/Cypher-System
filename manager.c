#include "manager.h"


char ** c;              //comanda entrada
char sizeofc;           //quantitat de paraules de la comanda
Config config;          //valors del fitxer de configuració


void imprimeixPrompt(){
    char *aux;          //variable que contindrà el prompt
    aux = (char*) malloc(strlen(PROMPT) + strlen(config.user));
    sprintf(aux, PROMPT, config.user);
    write(1, aux, strlen(aux));
    free(aux);
}
char* readUntil(int fd, char end, char lastChar) {
    int i = 0;       //variable que recorre la cadena
    char c = '\0';     //caràcter que llegim

    char* string = (char*)malloc(sizeof(char));

    while (c != end) {
        read(fd, &c, sizeof(char));
        if (c != end) {
            string = (char*)realloc(string, sizeof(char) * (i + 2));
            string[i] = c;
        }
        i++;
    }

    //posem com a últim caràcter el valor introduit
	string[i - 1] = lastChar;

    //Si l'últim caràcter no és \0 pot donar problemes al utilitzar funcions de cadenes
    if(lastChar != '\0'){
        i++;
        string = (char*)realloc(string, sizeof(char) * (i + 2));
        string[i - 1] = '\0';
    }
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

    //em salto l'espai entre paraules
    if(comanda[i] == ' '){ 
        i++;
    }

    //mirem que la comanda no estigui buida
    if(comanda[i] == '\n'){
        write(1, ERR_ARGS, strlen(ERR_ARGS));
        c[casella][0] = '\0';
    }
    else{
        //llegim fins al límit
        while(comanda[i] != limit && comanda[i] != '\n'){
            c[casella][j] = comanda[i];
            i++;
            j++;
            c[casella] = (char *)realloc(c[casella], sizeof(char) * (j+1));
        }

        c[casella][j] = '\0';

        //si la paraula és SHOW, la comanda introduida tindrà espais. Seguim llegint
        if (strcasecmp(c[casella], "SHOW") == 0){
            c[casella][j] = ' ';
            i++;
            j++;
            c[casella] = (char *)realloc(c[casella], sizeof(char) * (j+1));
            
            while(comanda[i] != ' ' && comanda[i] != '\n'){
                c[casella][j] = comanda[i];
                i++;
                j++;
                c[casella] = (char *)realloc(c[casella], sizeof(char) * (j+1));
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

    c = (char **)malloc(sizeof(char*)*1);
    c[0] = (char *)malloc(sizeof(char));

    //guardem a la variable c[0] la paraula de la comanda
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
        //llegim a la variable c[1] el nom de l'usuari
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

        //llegim a la variable c[1] el número del port
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

        //llegim a la variable c[1] el nom de l'usuari
        c[1] = (char *)malloc(sizeof(char));
        separaComanda(comanda, ' ', strlen("SAY"), 1);

        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
        else{
            c = (char **)realloc(c, sizeof(char*) * 3);
            c[2] = (char *)malloc(sizeof(char));
            //llegim a la variable c[2] el text a enviar
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
        //llegim a la variable c[1] el missatge a enviar
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
        //llegim a la variable c[1] el nom de l'usuari
        separaComanda(comanda, ' ', strlen("DOWNLOAD"), 1);

        //ens assegurem que hi hagi els arguments necessaris
        if(c[1][0] == '\0'){
            opcio = 0;
        }
        else{
            //audio
            c = (char **)realloc(c, sizeof(char*) * 3);
            c[2] = (char *)malloc(sizeof(char));
            //llegim a la variable c[1] el nom de l'audio
            separaComanda(comanda, '\n', strlen("DOWNLOAD") + strlen(c[1]) + 1, 2);

            //ens assegurem que hi hagi els arguments necessaris
            if(c[2][0] == '\0'){
                opcio = 0;
            }
        }
    }else if (strcasecmp(c[0], "EXIT") == 0){
        opcio = 7;
        sizeofc = 0;
    }
    else{
        sizeofc = -1;
        opcio = 0;
    }
    
    return opcio;
}

Config lecturaFitxer(const char *fitxer){
    int f;                  //file descriptor del fitxer    
    char * aux;             //cadena auxiliar per llegir valors enters

	//obrim el fitxer
    f = open(fitxer, O_RDONLY);
    if (f < 0)
    {
    	write(1, ERR_FILE, strlen(ERR_FILE));
		config.user = NULL;
	}

    config.user = readUntil(f, '\n', '\0');
    config.dirAudios = readUntil(f, '\n', '\0');
    config.ip = readUntil(f, '\n', '\0');

    aux = readUntil(f, '\n', '\0'); 
    config.port = atoi(aux);
    free(aux);

    config.ipWeb = readUntil(f, '\n', '\0');
    config.sysports = (char **)malloc(sizeof(char*) * 2);
    config.sysports[0] = readUntil(f, '\n', '\0');
    config.sysports[1] = readUntil(f, '\n', '\0');       

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
    free(config->sysports[0]);
    free(config->sysports[1]);
    free(config->sysports);
    free(config->ipWeb);
}

void buscaPorts(int pipe, int myPort){
    ssize_t nbytes;                         //número de bytes llegit
	unsigned char c;                        //caràcter que guardarà el que llegim del pipe                     
    char *port;                             //número del port en cadena
    char *extra;                            //caràcters que no necessito del que m'envien
    char *missatge;                         //línia que es mostrarà amb el número i l'usuari, si el sé
    char *aux;                              //cadena del missatge a msotrar
    char **connexions;                      //variable que guardarà tots els ports que estan oberts
    int nConn = 0;                          //número de connexions disponibles

    connexions = (char **) malloc(sizeof(char*));
    nbytes = read(pipe, &c, 1);

    while (nbytes > 0){
        //llegeixo "port"
        extra = readUntil(pipe, ' ', '\0');
        free(extra);
        
        //llegeixo el numero del port
        port = readUntil(pipe, ' ', '\0');

        //comprovo si se el nom de l'usuari
        missatge = comprovaNomUsuari(port, myPort);
        free(port);

        //comprovem que no es mostri el nostre port
        if(missatge != NULL){ 
            //redimensionem per afegir una nova connexió
            connexions = (char **) realloc(connexions, sizeof(char *) * (nConn + 1));
            connexions[nConn] = (char*)malloc(strlen(missatge) + 1);
            
            strcpy(connexions[nConn], missatge);
            nConn++;
        } 

        free(missatge);

        //llegeixo el que queda
        extra = readUntil(pipe, '\n', '\n');
        free(extra);

        nbytes = read(pipe, &c, 1);
    }
    
    //mostrem el missatge de la quantitat de connexions disponibles
    aux = (char *) malloc(sizeof(char) * strlen(CONN_AVAIL));
    sprintf(aux, CONN_AVAIL, nConn);
    write(1, aux, strlen(aux));
    free(aux);

    //mostrem les connexions disponibles
    for (int i = 0; i < nConn; i++){
        write(1, connexions[i], strlen(connexions[i]));
        free(connexions[i]);
    }
    free(connexions);
}

char * buscaAudios(){
    char * llista;

    asprintf(&llista, SHOWAUDIOS, config.user, config.dirAudios);
   
    //busquem arxius al directori  
    struct dirent **arxius;
    char *path;
    asprintf(&path, "./%s", config.dirAudios);
    int q_arxius = scandir(path, &arxius, NULL, alphasort);
    free(path);

    if (arxius == NULL)
        write(1, ERR_AUDIOS, strlen(ERR_AUDIOS));
    
    while (q_arxius--)
    {
        if ((strcmp (arxius[q_arxius]->d_name, ".") != 0) && (strcmp (arxius[q_arxius]->d_name, "..") != 0)){

        	llista = (char*)realloc(llista, strlen(llista)+strlen(arxius[q_arxius]->d_name)+2);
        	strcat(llista,arxius[q_arxius]->d_name);
        	strcat(llista, "\n");
            //asprintf(&llista, "%s%s\n", audios, arxius[q_arxius]->d_name);
            free (arxius[q_arxius]);
        }
    }

    free(arxius); 

    return llista; 
}

void buscaDownload(char * audio, int sockfd){
    //busquem arxius al directori  
    struct dirent **arxius;
    char *path, *aux, noTrobat = 1; 
    asprintf(&path, "./%s", config.dirAudios);
    int q_arxius = scandir (path, &arxius, NULL, alphasort);

    if (arxius == NULL)
        write(1, ERR_AUDIOS, strlen(ERR_AUDIOS));
    
    while (q_arxius--)
    {
        if (strcmp (arxius[q_arxius]->d_name, audio) == 0){
            noTrobat = 0;
            asprintf(&aux, "%s/%s", path, arxius[q_arxius]->d_name);
            enviaAudio(aux, arxius[q_arxius]->d_name, sockfd);
            free(aux);
            free(path);
        }
        free(arxius[q_arxius]);
    }
    if(noTrobat)
        enviaPaquet(sockfd, 0x05, "[AUDIO_KO]", 0, NULL);

    free (arxius);  
}

char * calculaChecksum (char *path){
    char *checksum;
    int fd[2];
    char * argv[3] = {"md5sum", path, NULL};

    if (pipe(fd) == -1){
            write(1, ERR_PIPE, strlen (ERR_PIPE));
            exit(-1);
    }
    
    pid_t pid = fork ();
    switch (pid){
        case 0: //fill
            close(fd[0]);
            dup2(fd[1], 1);

            //executem md5sum
            execvp(argv[0], argv);
            break;
        case -1:
            write(1, ERR_CONN, strlen(ERR_CONN));
            break;
        default: //pare
            close(fd[1]);
            //esperem que acabi d'escriure
            wait(NULL);
            //llegim del checksum i enviem 
            checksum = readUntil(fd[0], ' ', '\0');
            close(fd[0]);
            break;
    }
    return checksum;
}
