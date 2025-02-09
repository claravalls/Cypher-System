#include "network.h"

Conn_cli *conn_clients;     //els que s'han conectat
Conn_serv *conn_serv;       //a qui m'he connectat
int qClients, qServ;        //comptador dels clients i servidors connectats
int mySock;                 //valor del meu socket

extern Config config;      	//valors del fitxer de configuració

void setSockfd(int fd){ 
    mySock = fd;
}

void afegeixClient(int newsock, char* user, char *clientName){  
    char *aux;  
    if(qClients == 0){
        //creem l'array on guardarem els clients que s'han connectat
        conn_clients = (Conn_cli*) malloc (sizeof(Conn_cli));
    }
    else{
        //ampliem l'espai de memòria de l'array i guardem els valors de la nova connexió
        conn_clients = (Conn_cli*)realloc(conn_clients, sizeof(Conn_cli) * (qClients + 1));
    }
    
    conn_clients[qClients].sockfd = newsock;
    conn_clients[qClients].user = clientName;
    qClients++;

    //notifiquem al client que la connexió ha funcionat
    enviaPaquet(newsock, 0x01, "[CONOK]", strlen(user), user);

    asprintf(&aux, NOU_CLIENT, clientName);
    escriuTerminal(aux);
    free(aux);

    imprimeixPrompt();

    //creem el thread del client
    iniciaThreadClient(&conn_clients[qClients - 1], user);
}

int connectServer(const char* ip, int port){
    //iniciem la connexió del servidor
    int sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in s_addr;

    memset (&s_addr, 0, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (port);
    if (inet_aton (ip, &s_addr.sin_addr) == 0) {
        return -1;
    }

    if (bind (sockfd, (void *) &s_addr, sizeof (s_addr)) < 0) {
        return -1;
    }

    listen (sockfd, 3);

    qClients = 0;
    qServ = 0;

    return sockfd;
}

int connectClient(int port, char *ip, char *myUsername){
    char* okMessage;
    Protocol p;

    //ens assegurem que no es connecti a ell mateix
    if (port == config.port){
        escriuTerminal(AUTO_CONNECT);
        return -1;
    }

    //mirem que no estigui ja connectat
    for (int i = 0; i < qServ; i++)
    {
        if(conn_serv[i].port == port){
            escriuTerminal(ALREADY_CONN);
            return -1;
        }
    }
    
    int sockc = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockc < 0) {
        return -1;
    }

    //connect
    struct sockaddr_in s_addr;
    memset (&s_addr, 0, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (port);

    if (inet_aton (ip, &s_addr.sin_addr) == 0) {
        escriuTerminal(ERR_IP);
        return -1;
    }

    if (connect (sockc, (void *) &s_addr, sizeof (s_addr)) < 0){
        okMessage = (char *) malloc(strlen(ERR_CON_PORT) + strlen(ip) + 4);
        sprintf(okMessage, ERR_CON_PORT, ip, port);
        escriuTerminal(okMessage);
        free(okMessage);
        return -1;
    }
    
    //enviem el paquet de petició de connexió
    enviaPaquet(sockc, 0x01, "[TR_NAME]", strlen(myUsername), myUsername);

    if(qServ == 0){
        //creem l'array on guardarem els servidors als que ens hem connectat
        conn_serv = (Conn_serv*) malloc (sizeof(Conn_serv));
    }
    else{
        //afegim una nova connexió a l'array
        conn_serv = (Conn_serv*)realloc(conn_serv, sizeof(Conn_serv) * (qServ + 1));
    }
    
    conn_serv[qServ].port = port;
    conn_serv[qServ].sockfd = sockc;

    //esperem a rebre el nom paquet amb el nom d'usuari del servidor
    p = llegeixPaquet(sockc);
    
    if (p.data != NULL){
        //guardem el nom d'usuari
        asprintf(&(conn_serv[qServ].user), "%s", p.data);
        
        //iniciem un thread que escoltara al servidor
        iniciaThreadServidor(&conn_serv[qServ], myUsername);
        qServ++;

        //mostrem el missatge de connexió OK
        okMessage = (char *) malloc(strlen(ERR_CON_PORT) + strlen(p.data) + 5); 
        sprintf (okMessage, OK_CONN, port, p.data);
        escriuTerminal(okMessage);
        free(okMessage);

        alliberaPaquet(p); 
        return 0;
    }

    return -1;
}

char * comprovaNomUsuari(char *port, int myPort){
    int p = atoi(port);         //variable amb el port 
    char *missatge;             //missatge que mostrarà pel terminal

    //si és el meu servidor no el vull mostrar
    if(p == myPort){
        return NULL;
    }
    else{
        //posem el missatge a mostrar com al número del port
        missatge = (char *) malloc(strlen(port) + 2); //+2 pel \n que afegirem i el \0 que afegeix el strcpy
        strcpy(missatge, port);     
        strcat(missatge, "\n");

        //recorrem l'array de connexions per veure si ja està connectada i tenim el nom d'usuari guardat
        for (int i = 0; i < qServ; i++){     
            
            //si ja està connectat, mostrarem el missatge amb el port i el nom d'usuari 
            if (conn_serv[i].port == p){
                //sobreescriurem el missatge a mostrar
                missatge = (char *) realloc(missatge, strlen(CONN_WITH_NAME) + strlen(conn_serv[i].user) + 4); //+ 4 pel port
                sprintf(missatge, CONN_WITH_NAME, p, conn_serv[i].user);
                
                break;
            }
        }
    }
    return missatge;
}

void enviaPaquet(int fd, char type, char* header, int length, char* data){
    //enviem el paquet camp a camp
    write(fd, &type, 1);
    write(fd, header, strlen(header));
    write(fd, &length, 2);

    if(length > 0){
        write(fd, data, length);
    }
}

Protocol llegeixPaquet(int fd){
    ssize_t nbytes;                 //guardarà el número de bytes que ha llegit
    Protocol p;

    nbytes = read(fd, &p.type, 1);

    //si no hem rebut cap missatge, retornarem un paquet amb data NULL
    if (nbytes == 0){
        p.data = NULL;
        return p;
    }

    p.header = readUntil(fd, ']', ']');
    
    read(fd, &p.length, 2);
    
    p.data = (char *) malloc(p.length + 1);

    if(p.length != 0){
        read(fd, p.data, p.length);
        p.data[p.length] = '\0';
    }

    return p;
}

void freeConnections(){ 
    escriuTerminal(CLOSING);                   
    for (int i = 0; i < qServ; i++){
        free(conn_serv[i].user);
    }
    free(conn_serv);
    
    for (int i = 0; i < qClients; i++){
        free(conn_clients[i].user);
    }
    free(conn_clients);			

    alliberaMemoriaConfig(&config);
}

void imprimeixMissatge(char *missatge, char* user){
    char *aux;          //cadena del missatge

    //eliminem les ""
    if(missatge[0] == '"' && missatge[strlen(missatge) - 1] == '"'){
        strcpy(&missatge[0], &missatge[1]); //traiem el " de la cadena
        missatge[strlen(missatge) - 1] = '\0';
    }
    asprintf(&aux, MESSAGE, user, missatge);
    escriuTerminal(aux);
    free(aux);
}

void enviaMissatge(char *user, char *missatge){
    int sockfd = 0;
    //busquem a quin sockfd correspon el nom d'usuari
    for (int i = 0; i < qServ; i++){
        if(strcasecmp(conn_serv[i].user, user) == 0){
            sockfd = conn_serv[i].sockfd;
            enviaPaquet(sockfd, 0x02, "[MSG]", strlen(missatge), missatge);
            break;
        }
    }
    //comprovem si el nom d'usuari introduit és incorrecte
    if(sockfd == 0)
        escriuTerminal(ERR_USER);
}

void sendBroadcast(char * message){
    for (int i = 0; i < qServ; i++)
    {
        enviaPaquet(conn_serv[i].sockfd, 0x03, "[BROADCAST]", strlen(message), message);
    }
}

void tancaConnexions(){
    int i = 0;

    //avisem a tots els clients connectats que es tancarà la connexió
    while(i < qClients) {
        enviaPaquet(conn_clients[i].sockfd, 0x06, "[]", strlen(config.user), config.user);
        i++;
    }

    i = 0;
    //avisem a tots els servidors que es tancarà la connexió
    while(i < qServ) {
        enviaPaquet(conn_serv[i].sockfd, 0x06, "[]", strlen(config.user), config.user);
        i++;
    }
    joinUserThread(config.user);
}

void eliminaConnexioCli(char *user){
    int s = 0, b;
    //busquem al client a l'array
    for (b = 0; b < qClients; b++){
        if(strcmp(user, conn_clients[b].user) == 0){
            s = b + 1;
            free(conn_clients[b].user);
            
            if(s < qClients){ //si hem d'eliminar l'últim valor no cal shiftar
                
                //shiftem els valors a l'esquerra
                for (int i = b; i < qClients; i++){
                    conn_clients[b] = conn_clients[s];
                    b++;
                    s++;
                }
            }

            if(qClients == 1){ //es l'ultim client
                //free(conn_clients[0].user);
                free(conn_clients);
            }
            else{
                //redimensionem la mida de l'array
                conn_clients = (Conn_cli *) realloc(conn_clients, sizeof(Conn_cli) * (qClients - 1));
            }
            qClients--;
            break;
        }
    }
}

void eliminaConnexioServ(char *user){
    int s = 0, b;
    //busquem al client a l'array
    for (b = 0; b < qServ; b++){
        if(strcmp(user, conn_serv[b].user) == 0){
            s = b + 1;
            free(conn_serv[b].user);
            
            if(s < qServ){ //si hem d'eliminar l'últim valor no cal shiftar
                //shiftem els valors a l'esquerra
                for (int i = b; i < qServ; i++){
                    conn_serv[b] = conn_serv[s];
                    b++;
                    s++;
                }
            }

            if(qServ == 1){ //es l'ultim servidor
                //free(conn_serv[0].user);
                free(conn_serv);
            }
            else{
                //redimensionem la mida de l'array
                conn_serv = (Conn_serv *) realloc(conn_serv, sizeof(Conn_serv) * (qServ - 1));
            }
            qServ--;
            
            break;
        }
    }
}

void alliberaPaquet(Protocol p){  
    free(p.header);
    free(p.data);
}

void enviaShowAudios(char *user){
    int sockfd = 0;
    //busquem a quin sockfd correspon el nom d'usuari
    for (int i = 0; i < qServ; i++){
        if(strcasecmp(conn_serv[i].user, user) == 0){
            sockfd = conn_serv[i].sockfd;
            enviaPaquet(sockfd, 0x04, "[SHOW_AUDIOS]", 0, NULL);
            break;
        }
    }
    //comprovem si el nom d'usuari introduit és incorrecte
    if(sockfd == 0)
        escriuTerminal(ERR_USER);
}

void enviaDownloadAudio(char *user, char *audio){
    int sockfd = 0;
    //busquem a quin sockfd correspon el nom d'usuari
    for (int i = 0; i < qServ; i++){
        if(strcasecmp(conn_serv[i].user, user) == 0){
            sockfd = conn_serv[i].sockfd;
            enviaPaquet(sockfd, 0x05, "[AUDIO_RQST]", strlen(audio), audio);
            break;
        }
    }
    //comprovem si el nom d'usuari introduit és incorrecte
    if(sockfd == 0)
        escriuTerminal(ERR_USER);
}

void enviaAudio(char* path, char *audioName, int sockfd){
    int f = open(path, O_RDONLY);
    int q = 0;
    char *audio, c, *checksum;

    if(f < 0){
        enviaPaquet(sockfd, 0x05, "[AUDIO_KO]", 0, NULL);
    }

    else{
        size_t nbytes;
        //envio nom de l'audio a crear per sabem com s'ha de dir el fitxer on guardar la info
        asprintf(&audio, "./%s", audioName);
        enviaPaquet(sockfd, 0x05, "[AUDIO_RSPNS]", strlen(audio), audio);
        
        do{ 
            char *data = (char *)malloc(sizeof(char) * 512);
            //llegeixo 512 bytes
            for (q = 0; q < 512; q++)
            {
                nbytes = read(f, &c, 1);
                if(nbytes <= 0) break;
                data[q] = c;
            }
            //envio
            enviaPaquet(sockfd, 0x05, "[AUDIO_RSPNS]", q, data);
            free(data);
        }while(nbytes > 0);

        checksum = calculaChecksum(path);
        enviaPaquet(sockfd, 0x05, "[EOF]", strlen(checksum), checksum);
        free(checksum);
        free(audio);
    }
}