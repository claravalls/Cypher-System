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

int connectServer(const char *ip, int port);
int connectClient(int port, char *ip, char *username);
void afegeixClient(int newsock, char * user, char *clientName);
void setSockfd(int fd);
void closeConnections();
char * comprovaNomUsuari(char *port, int myPort);
void enviaPaquet(int fd, char type, char *header, int length, char *data);
Protocol llegeixPaquet(int fd);
void imprimeixMissatge(char *missatge, char* user);
void enviaMissatge(char *user, char *missatge);

#endif
