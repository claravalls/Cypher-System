#ifndef _NETWORK_H_
#define _NETWORK_H_

#define _GNU_SOURCE 1

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
#define CONN_WITH_NAME "\n%d %s\n"
#define NOU_CLIENT "New connection. User: %s\n"
#define ADEU_CLIENT "Client %s has disconnected\n"


typedef struct{
    unsigned char type;
    char * header;
    short length;
    char * data;
}Protocol;

/***********************************************
* @Finalitat:   Iniciar la connexió del servidor
* @Parametres:  ip: valor de la IP on s'ha de realitzar la connexió
                port: valor del port del servidor a connectar
* @Retorn:      retornarà el file descriptor del socket si s'ha pogut
                connectar o -1 si no s'ha pogut establir la connexió
************************************************/
int connectServer(const char *ip, int port);
/***********************************************
* @Finalitat:   Establir la connexió amb un altre servidor
* @Parametres:  ip: valor de la IP on s'ha de realitzar la connexió
                port: valor del port on es vol connectar
                myUsername: nom del client que vol connectar-se
* @Retorn:      retorna -1 en cas d'error o 0 si s'ha connectat correctament
************************************************/
int connectClient(int port, char *ip, char *myUsername);
/***********************************************
* @Finalitat:   Afegir un nou client a l'array de clients connectats
* @Parametres:  newsock: valor del file descriptor del socket del nou client
                user: nom d'usuari del servidor
                clientName: nom d'usuari del client que s'ha connectat
* @Retorn:      ---
************************************************/
void afegeixClient(int newsock, char * user, char *clientName);
/***********************************************
* @Finalitat:   Guardar el nom del file descriptor del meu socket
* @Parametres:  fd: file descriptor amb el valor del meu socket
* @Retorn:      ---
************************************************/
void setSockfd(int fd);
/***********************************************
* @Finalitat:   Elimina la memòria demanada per guardar les connexions
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void freeConnections();
/***********************************************
* @Finalitat:   Comprovar si se sap el nom de l'usuari que correspon al 
                port introduit, és a dir, si ja hi ha una connexió establerta
* @Parametres:  port: valor del port que busquem
                myPort: valor del port del propi servidor
* @Retorn:      missatge a mostrar pel terminal amb els ports disponibles i el
                nom de l'usuari al que correspon, en cas de saber-lo
************************************************/
char * comprovaNomUsuari(char *port, int myPort);
/***********************************************
* @Finalitat:   enviar un paquet al file descriptor indicat seguint el protocol
                de comunicació especificat
* @Parametres:  fd: file descriptor amb el destí del paquet
                type: valor del camp type de la trama a enviar
                header: valor del camp header de la trama a enviar
                length: longitud del campt data
                data: dades a enviar
* @Retorn:      ---
************************************************/
void enviaPaquet(int fd, char type, char *header, int length, char *data);
/***********************************************
* @Finalitat:   rebre i interpretar un paquet rebut
* @Parametres:  fd: file descriptor des d'on llegir el paquet
* @Retorn:      Protocol amb els valors de la trama llegida
************************************************/
Protocol llegeixPaquet(int fd);
/***********************************************
* @Finalitat:   Mostrar per pantalla un missatge rebut des d'una connexió
* @Parametres:  missatge: contingut del missatge rebut
                user: usuari que ha enviat el missatge
* @Retorn:      ---
************************************************/
void imprimeixMissatge(char *missatge, char* user);
/***********************************************
* @Finalitat:   enviar un missatge a un usuari amb qui s'ha establert una
                connexió
* @Parametres:  user: usuari a qui es vol enviar el missatge
                missatge: contingut del missatge a enviar
* @Retorn:      ---
************************************************/
void enviaMissatge(char *user, char *missatge);
/***********************************************
* @Finalitat:   eliminar el valor de l'array de connexions i shiftar els
                valors a l'esquerra
* @Parametres:  user: nom de l'usuari que es tanca
* @Retorn:      ---
************************************************/
void eliminaConnexioCli(char *user);
/***********************************************
* @Finalitat:   eliminar el valor de l'array de connexions i shiftar els
                valors a l'esquerra
* @Parametres:  user: nom de l'usuari que es tanca
* @Retorn:      ---
************************************************/
void eliminaConnexioServ(char *user);
/***********************************************
* @Finalitat:   enviar un paquet a totes les connexions establertes per 
                notificar que el servidor s'apaga i s'han de tancar les
                connexions
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void tancaConnexions();

void alliberaPaquet(Protocol p);

void enviaShowAudios(char *user);

void enviaDownloadAudio(char *user, char *audio);

void enviaAudio(char *path, char *audioName, int sockfd);

#endif
