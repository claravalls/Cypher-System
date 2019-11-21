#include "network.h"

Conn_cli *conn_clients; //els que s'han conectat
Conn_serv *conn_serv; //a qui m'he connectat
int qClients, qServ, mySock;

void setSockfd(int fd){ //guardo el sockfd del meu servidor
    mySock = fd;
}

void afegeixClient(int newsock, char* user, char *clientName){    
    conn_clients = (Conn_cli*)realloc(conn_clients, sizeof(Conn_cli) * (qClients + 1));
    conn_clients[qClients].sockfd = newsock;
    conn_clients[qClients].user = clientName;
    qClients++;
    enviaPaquet(newsock, 0x01, "[CONOK]", strlen(user), user);

    //creem el thread del client
    iniciaThreadClient(&conn_clients[qClients - 1]);
}

int connectServer(const char* ip, int port){
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

    conn_clients = (Conn_cli*) malloc (sizeof(Conn_cli));
    qClients = 0;
    conn_serv = (Conn_serv*) malloc (sizeof(Conn_serv));
    qServ = 0;

    return sockfd;
}

int connectClient(int port, char *ip, char *myUsername){
    char okMessage[50];
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
        strcpy(okMessage, "");
        sprintf(okMessage, ERR_CON_PORT, ip, port);
        write(1, okMessage, strlen(okMessage));
        return -1;
    }
    
    enviaPaquet(sockc, 0x01, "[TR_NAME]", strlen(myUsername), myUsername);

    conn_serv = (Conn_serv*)realloc(conn_serv, sizeof(Conn_serv) * (qServ + 1));
    conn_serv[qServ].port = port;
    conn_serv[qServ].sockfd = sockc;

    p = llegeixPaquet(sockc);
    user = p.data;
    
    if (user != NULL){
        conn_serv[qServ].user = user; 

        qServ++;

        strcpy(okMessage, "");
        sprintf (okMessage, OK_CONN, port, user);
        write (1, okMessage, strlen(okMessage));
        return 0;
    }
    return 1;
}

char * comprovaNomUsuari(char *port, int myPort){
    int p = atoi(port);
    char *missatge;

    missatge = (char *) malloc(sizeof(char) * (strlen(port)));

    //si és el meu servidor no el vull mostrar
    if(p == myPort){
        return NULL;
    }
    else{
        strcpy(missatge, port);
        strcat(missatge, "\n");

        for (int i = 0; i < qServ; i++)
        {
            if (conn_serv[i].port == p){
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
    p.type = type;
    p.header = (char*) malloc(sizeof(char) * strlen(header));
    strcpy (p.header, header);
    p.length = length;
    p.data = (char*) malloc(sizeof(char) * length);
    strcpy(p.data, data);

    write(fd, &p.type, 1);
    write(fd, p.header, strlen(p.header));
    write(fd, &p.length, 2);
    write(fd, p.data, strlen(data));
}

Protocol llegeixPaquet(int fd){
    char type, *header, *data;
    int length;
    ssize_t nbytes;
    Protocol p;

    nbytes = read(fd, &type, 1);
    if (nbytes == 0){
        p.data = NULL;
        return p;
    }

    header = readUntil(fd, ']', ']');

    read(fd, &length, 2);

    data = (char *) malloc(sizeof(char) * length);
    read(fd, data, length);

    p.type = type;
    p.header = header;
    p.length = length;
    p.data = data;
    return p;
}

void closeConnections(){
    write(1, CLOSING, strlen(CLOSING));
    close(mySock);
    for (int i = 0; i < qServ; i++){
        close(conn_serv[i].sockfd);
        free(conn_serv[i].user);
    }
    free(conn_serv);
    
    for (int i = 0; i < qClients; i++){
        close(conn_clients[i].sockfd);
        free(conn_clients[i].user);
    }
    free(conn_clients);
}

void imprimeixMissatge(char *missatge, char* user){
    char *aux;
    aux = (char*) malloc(sizeof(char) * strlen(MESSAGE));
    sprintf(aux, MESSAGE, user, missatge);
    write(1, aux, strlen(aux));
    free(aux);
}

void enviaMissatge(char *user, char *missatge){
    int sockfd = 0;
    for (int i = 0; i < qServ; i++)
    {
        if(strcmp(conn_serv[i].user, user) == 0){
            sockfd = conn_serv[i].sockfd;
            enviaPaquet(sockfd, 0x02, "[MSG]", strlen(missatge), missatge);
            break;
        }
    }
    if(sockfd == 0){
        write(1, ERR_USER, strlen(ERR_USER));
    }
}