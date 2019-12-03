#include "network.h"

Conn_cli *conn_clients;     //els que s'han conectat
Conn_serv *conn_serv;       //a qui m'he connectat
int qClients, qServ;        //comptador dels clients i servidors connectats
int mySock;                 //valor del meu socket

void setSockfd(int fd){ 
    mySock = fd;
}

void afegeixClient(int newsock, char* user, char *clientName){    
    //ampliem l'espai de memòria de l'array i guardem els valors de la nova connexió
    conn_clients = (Conn_cli*)realloc(conn_clients, sizeof(Conn_cli) * (qClients + 1));
    conn_clients[qClients].sockfd = newsock;
    conn_clients[qClients].user = clientName;
    qClients++;


    //notifiquem al client que la connexió ha funcionat
    enviaPaquet(newsock, 0x01, "[CONOK]", strlen(user), user);

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

    //creem l'array on guardarem els clients que s'han connectat
    conn_clients = (Conn_cli*) malloc (sizeof(Conn_cli));
    qClients = 0;
    //creem l'array on guardarem els servidors als que ens hem connectat
    conn_serv = (Conn_serv*) malloc (sizeof(Conn_serv));
    qServ = 0;

    return sockfd;
}

int connectClient(int port, char *ip, char *myUsername){
    char* okMessage;
    char * user;
    Protocol p;

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
        write (1, ERR_IP, strlen(ERR_IP));
        return -1;
    }

    if (connect (sockc, (void *) &s_addr, sizeof (s_addr)) < 0){
        okMessage = (char *) malloc(sizeof(char) * (strlen(ERR_CON_PORT) + strlen(ip) + 4)); //Cal sumar-li 4 pel port

        sprintf(okMessage, ERR_CON_PORT, ip, port);
        write(1, okMessage, strlen(okMessage));
        free(okMessage);
        return -1;
    }
    
    //enviem el paquet de petició de connexió
    enviaPaquet(sockc, 0x01, "[TR_NAME]", strlen(myUsername), myUsername);

    //afegim una nova connexió a l'array
    conn_serv = (Conn_serv*)realloc(conn_serv, sizeof(Conn_serv) * (qServ + 1));
    conn_serv[qServ].port = port;
    conn_serv[qServ].sockfd = sockc;

    //esperem a rebre el nom paquet amb el nom d'usuari del servidor
    p = llegeixPaquet(sockc);
    user = p.data;
    
    if (user != NULL){
        //guardem el nom d'usuari i augmentem el número de servidors als que m'he connectat
        conn_serv[qServ].user = user; 

        iniciaThreadServidor(&conn_serv[qServ], myUsername);
        qServ++;

        //mostrem el missatge de connexió OK
        okMessage = (char *) malloc(strlen(ERR_CON_PORT) + strlen(ip) + strlen(user));

        sprintf (okMessage, OK_CONN, port, user);
        write (1, okMessage, strlen(okMessage));
        free(okMessage);
        return 0;
    }
    return -1;
}

char * comprovaNomUsuari(char *port, int myPort){
    int p = atoi(port);         //variable amb el port 
    char *missatge;             //missatge que mostrarà pel terminal
    missatge = (char *) malloc(strlen(port) + 2);

    //si és el meu servidor no el vull mostrar
    if(p == myPort){
        return NULL;
    }
    else{
        //posem el missatge a mostrar com al número del port
        strcpy(missatge, port);     
        strcat(missatge, "\n");

        //recorrem l'array de connexions per veure si ja està connectada i tenim el nom d'usuari guardat
        for (int i = 0; i < qServ; i++){     
            //si ja està connectat, mostrarem el missatge amb el port i el nom d'usuari 
            if (conn_serv[i].port == p){
                //sobreescriurem el missatge a mostrar
                missatge = (char *) realloc(missatge, (sizeof(char) * 7));
                strcpy(missatge, "");
                sprintf(missatge, "\n%d %s\n", p, conn_serv[i].user);
                break;
            }
        }
    }
    return missatge;
}

void enviaPaquet(int fd, char type, char* header, int length, char* data){
    Protocol p;

    //creem el paquet a enviar
    p.type = type;
    p.header = (char*) malloc(strlen(header) + 1);
    strcpy(p.header, header);

    p.length = length;

    if(length !=  0){
        p.data = (char*) malloc(sizeof(char) * length);
    }
    else{
        p.data = (char*) malloc(sizeof(char));
    }
    p.data = data;

    //enviem el paquet camp a camp
    write(fd, &p.type, 1);
    write(fd, p.header, strlen(p.header));
    write(fd, &p.length, 2);
    write(fd, p.data, strlen(p.data));
}

Protocol llegeixPaquet(int fd){
    char type, *header, *data;      //variables on guardarem els camps del paquet
    int length = 0;

    ssize_t nbytes;                 //guardarà el número de bytes que ha llegit
    Protocol p;

    nbytes = read(fd, &type, 1);
    //si no hem rebut cap missatge, retornarem un paquet amb data NULL
    if (nbytes == 0){
        p.data = NULL;
        return p;
    }

    header = readUntil(fd, ']', ']');

    //si el header indica KO, retornarem un paquet amb data NULL
    if(strcmp(header, "[CONKO]") == 0){
        p.data = NULL;
        return p;
    }
    
    read(fd, &length, 2);

    data = (char *) malloc(sizeof(char) * length);
    read(fd, data, length);

    p.type = type;
    p.header = header;
    p.length = length;
    p.data = data;
    return p;
}

void freeConnections(){                    
    write(1, CLOSING, strlen(CLOSING));
    for (int i = 0; i < qServ; i++){
        free(conn_serv[i].user);
    }
    free(conn_serv);
    
    for (int i = 0; i < qClients; i++){
        free(conn_clients[i].user);
    }
    free(conn_clients);

    close(mySock);
}

void imprimeixMissatge(char *missatge, char* user){
    char *aux;          //cadena del missatge
    aux = (char*) malloc(sizeof(char) * (strlen(MESSAGE) + strlen(missatge) + strlen(user))); //SUMAR MIDA DE USER I MISSATGE
    sprintf(aux, MESSAGE, user, missatge);
    write(1, aux, strlen(aux));
    free(aux);
}

void enviaMissatge(char *user, char *missatge){
    int sockfd = 0;
    //busquem a quin sockfd correspon el nom d'usuari
    for (int i = 0; i < qServ; i++){
        if(strcmp(conn_serv[i].user, user) == 0){
            sockfd = conn_serv[i].sockfd;
            enviaPaquet(sockfd, 0x02, "[MSG]", strlen(missatge), missatge);
            break;
        }
    }
    //comprovem si el nom d'usuari introduit és incorrecte
    if(sockfd == 0){
        write(1, ERR_USER, strlen(ERR_USER));
    }
}

void tancaConnexions(){
    int i = 0;
    //agafem el nom de l'usuari
    Config config = getConfig();


    //avisem a tots els clients connectats que es tancarà la connexió
    while(i < qClients) {
        enviaPaquet(conn_clients[i].sockfd, 0x06, "[]", strlen(config.user), config.user);

        i++;
    }
    i = 0;
    //avisem a tots els servidors que es tancarà la connexió
    while(i < qServ) {
        enviaPaquet(conn_serv[i].sockfd, 0x06, "[]", strlen(config.user), config.user);

        //close(conn_serv[i].sockfd);
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
            //shiftem els valors a l'esquerra
            for (int i = b; i < qClients; i++){
                conn_clients[b] = conn_clients[s];
                b++;
                s++;
            }

            //redimensionem la mida de l'array
            conn_clients = (Conn_cli *) realloc(conn_clients, sizeof(Conn_cli) * (qClients - 1));
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
            //shiftem els valors a l'esquerra
            for (int i = b; i < qServ; i++){
                conn_serv[b] = conn_serv[s];
                b++;
                s++;
            }

            //redimensionem la mida de l'array
            conn_serv = (Conn_serv *) realloc(conn_serv, sizeof(Conn_serv) * (qServ - 1));
            qServ--;
            break;
        }
    }
}