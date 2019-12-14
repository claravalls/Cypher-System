#ifndef _THREAD_H_
#define _THREAD_H_

#define _GNU_SOURCE 1

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

#define TRANS_END "\n[%s] %s downloaded"
 
typedef struct {
    int port;
    int sockfd;
    char *user;
}Conn_serv;

typedef struct {
    int sockfd;
    char *user;
}Conn_cli;

typedef struct{
	pthread_t t;
	char *user;
	int listener;
}UserThread;

/***********************************************
* @Finalitat:   crear un thread que mantindrà al servidor a 
                l'espera de noves connexions
* @Parametres:  config: informació extreta del fitxer de configuració
* @Retorn:      ---
************************************************/
void iniciaThreadEscolta(Config* config);
/***********************************************
* @Finalitat:   crear un thread que estarà escoltant constantment
                al servidor a qui s'ha connectat per si envia algun missatge
* @Parametres:  servidor: informació del servidor a qui està escoltant
* @Retorn:      ---
************************************************/
void iniciaThreadServidor(Conn_serv *servidor, char *user);
/***********************************************
* @Finalitat:   crear un thread que estarà escoltant constantment
                al client per si envia algun missatge
* @Parametres:  client: informació del client a qui està escoltant
* @Retorn:      ---
************************************************/
void iniciaThreadClient(Conn_cli *client, char *user);
/***********************************************
* @Finalitat:   aturar el thread del servidor que espera noves connexions
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void apagaServidor();


void joinUserThread(char *user);

void shiftJoins(UserThread *tThread, char *user, int lenght);

#endif