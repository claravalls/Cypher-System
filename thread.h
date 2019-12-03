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
 
//Estructura qeu guarda la informació de la connexió a un servidor
typedef struct {
    int port;           //número del port
    int sockfd;         //socket del servidor
    char *user;         //nom del servidor
}Conn_serv;

//Estructura qeu guarda la informació de la connexió amb un client
typedef struct {
    int sockfd;         //socket al que pertany el client
    char *user;         //nom del client
}Conn_cli;

//Estructura que guarda la informació del thread
typedef struct{
	pthread_t t;        //thread
	char *user;         //nom de l'usuari que ha creat el thread
	int listener;       //socket que escolta el thread
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

/***********************************************
* @Finalitat:   aturar els threads pertanyents a l'usuari que es vol desconnectar
* @Parametres:  user: nom de l'usuari que es desconnecta
* @Retorn:      ---
************************************************/
void joinUserThread(char *user);

/***********************************************
* @Finalitat:   eliminar de l'array passat per paràmetre aquell thread que té com a propietari
                l'usuari indicat
* @Parametres:  tThread: array a actualitzar
                user: nom de l'usuari a eliminar de l'array
                length: longitud total de l'array a actualitzar
* @Retorn:      ---
************************************************/
void shiftJoins(UserThread *tThread, char *user, int lenght);

#endif