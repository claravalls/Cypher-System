#include "network.h"

Conn_cli *conn_clients;
Conn_serv *conn_serv;
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

    user = llegeixPaquet(sockc);
    
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
    p.header = header;
    p.length = length;
    p.data = (char*) malloc(sizeof(char) * length);
    p.data = data;

    write(fd, &p.type, 1);
    write(fd, p.header, strlen(p.header));
    write(fd, &p.length, 2);
    write(fd, p.data, strlen(p.data));
}

char* llegeixPaquet(int fd){
    char type, *header, *missatge;
    int length=0;
    ssize_t nbytes;

    nbytes = read(fd, &type, 1);
    if (nbytes == 0){
        return NULL;
    }

    header = readUntil(fd, ']', ']');
    
    if(strcasecmp(header, "[CONKO]") == 0){
        return NULL;
    }
    read(fd, &length, 2);

    missatge = (char *) malloc(sizeof(char) * length);
    read(fd, missatge, length);

    free(header);
    return missatge;
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

