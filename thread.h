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

/***********************************************
* @Finalitat:   crear un thread que mantindrà al servidor a 
                l'espera de noves connexions
* @Parametres:  config: informació extreta del fitxer de configuració
* @Retorn:      ---
************************************************/
void iniciaThreadServidor(Config *config);
/***********************************************
* @Finalitat:   crear un thread que estarà escoltant constantment
                al client per si envia algun missatge
* @Parametres:  clients: informació del client a qui està escoltant
* @Retorn:      ---
************************************************/
void iniciaThreadClient(Conn_cli *client);
/***********************************************
* @Finalitat:   aturar el thread del servidor que espera noves connexions
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void apagaServidor();

#endif