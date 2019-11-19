#include "network.h"

int *conn_clients;
Conn_serv *conn_serv;
int qClients, qServ, mySock;

void setSockfd(int fd){ //guardo el sockfd del meu servidor
    mySock = fd;
}

void afegeixClient(int newsock, char* user){
    printf("\nNou client %d user: %s\n", newsock, user);
    
    conn_clients = (int*)realloc(conn_clients, sizeof(int) * (qClients + 1));
    conn_clients[qClients] = newsock;
    qClients++;
    write(newsock, user, strlen(user));
    write(newsock, "\n", 1);
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

    conn_clients = (int*) malloc (sizeof(int));
    qClients = 0;
    conn_serv = (Conn_serv*) malloc (sizeof(Conn_serv));
    qServ = 0;

    return sockfd;
}

int connectClient(int port, char * ip){
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
        write (1, "No es una adrea IP valida\n", strlen("No es una adrea IP valida\n"));
        return -1;
    }

    if (connect (sockc, (void *) &s_addr, sizeof (s_addr)) < 0){
        strcpy(okMessage, "");
        sprintf(okMessage, "Error a la connexió de l'adreça %s en el port %d\n", ip, port);
        write(1, okMessage, strlen(okMessage));
        return 1;
    }

    conn_serv = (Conn_serv*)realloc(conn_serv, sizeof(Conn_serv) * (qServ + 1));
    conn_serv[qServ].port = port;
    conn_serv[qServ].sockfd = sockc;
    
    user = readUntil(sockc, '\n', '\n'); 
    conn_serv[qServ].user = (char *) malloc(sizeof(char) * strlen(user));
    strcpy(conn_serv[qServ].user, user); 

    qServ++;

    strcpy(okMessage, "");
    sprintf (okMessage, "%d connected: %s", port, user);
    write (1, okMessage, strlen(okMessage));
    return 0;
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
                sprintf(missatge, "%d %s", p, conn_serv[i].user);
                break;
            }
        }
    }
    return missatge;
}

void closeConnections(){
    write(1, "Closing all connections...\n", strlen("Closing all connections...\n"));
    close(mySock);
    for (int i = 0; i < qServ; i++){
        close(conn_serv[i].sockfd);
        free(conn_serv[i].user);
    }
    free(conn_serv);
    
    for (int i = 0; i < qClients; i++){
        close(conn_clients[i]);
    }
    free(conn_clients);
}

