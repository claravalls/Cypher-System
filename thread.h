#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "network.h"
#include "logica.h"

typedef struct {
    int port;
    int sockfd;
    char *user;
}Conn_serv;

typedef struct {
    int sockfd;
    char *user;
}Conn_cli;

void iniciaThreadServidor(Config *config);
void iniciaThreadClient(Conn_cli *client);

#endif