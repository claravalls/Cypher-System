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

#define ERR_IP "Invalid IP adress\n"
#define ERR_CON_PORT "Error connecting %s in %d port\n"
#define ERR_ACCEPT "Error. Can not accept connections\n"
#define ERR_SOCKET "Error. Can not create socket.\n"
#define OK_CONN "%d connected: %s"
#define CLOSING "Closing all connections...\n"
#define MESSAGE "\n[%s]: %s\n"
#define ERR_USER "This user does not exists\n"

typedef struct{
    unsigned char type;
    char * header;
    unsigned int length;
    char * data;
}Protocol;

/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
int connectServer(const char *ip, int port);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
int connectClient(int port, char *ip, char *username);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void afegeixClient(int newsock, char * user, char *clientName);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void setSockfd(int fd);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void closeConnections();
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
char * comprovaNomUsuari(char *port, int myPort);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void enviaPaquet(int fd, char type, char *header, int length, char *data);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
Protocol llegeixPaquet(int fd);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void imprimeixMissatge(char *missatge, char* user);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void enviaMissatge(char *user, char *missatge);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void tancaConnexions();

#endif
