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

#include "logica.h"

#define ERR_SOCKET "Error creando el socket.\n"

int connectSocket(const char* ip, int port);
int connect(int port, char * ip, char * user);


#endif
