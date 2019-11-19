#ifndef _NETWORK_H_
#define _NETWORK_H_


#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#include "thread.h"
#include "manager.h"

#define ERR_ACCEPT "Error. No se pueden aceptar conexiones\n"
#define ERR_SOCKET "Error creando el socket.\n"

typedef struct {
    int port;
    int sockfd;
    char *user;
}Conn_serv;

int connectServer(const char* ip, int port);
int connectClient(int port, char * ip);
void afegeixClient(int newsock, char * user);
void setSockfd(int fd);
void closeConnections();
char * comprovaNomUsuari(char *port, int myPort);

#endif
