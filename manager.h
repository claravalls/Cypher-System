#ifndef _MANAGER_H_
#define _MANAGER_H_

//Llibreries del sistema

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <pthread.h>

#include "logica.h"
#include "network.h"

#define ERR_FILE "Error opening file...\n"
#define CONN_AVAIL "%d connections available\n"
#define PROMPT "\r$%s: "
#define SHOWAUDIOS "\r[%s] %s\n"
#define ERR_AUDIOS "\rError. Audio file inexistent\n"

/***********************************************
* @Finalitat:   mostrar el prompt amb el nom de l'usuari pel terminal
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void imprimeixPrompt();

/***********************************************
* @Finalitat:   llegir el fitxer de configuració i extreure'n la informació
* @Parametres:  fitxer: cadena que conté el nom del fitxer a llegir
* @Retorn:      variable que conté tota la informació del fitxer
************************************************/
Config lecturaFitxer(const char *fitxer);

/***********************************************
* @Finalitat:   separar les paraules de la comanda introduïda per l'usuari
* @Parametres:  comanda: cadena que conté la comanda sencera
                limit: caràcter final que indica fins a on partir la comanda
                i: valor per on començar a separar
                casella: casella on es guardarà la paraula separada
* @Retorn:      ---
************************************************/
void separaComanda(char *comanda, char limit, int i, int casella);

/***********************************************
* @Finalitat:   interpretar la comanda introduïda per l'usuari
* @Parametres:  comanad: cadena que conté la comanda sencera
* @Retorn:      caràcter amb un valor del 1 al 7 que indicarà quina opció s'ha
                d'executar
************************************************/
char llegeixComanda(char * comanda);

/***********************************************
* @Finalitat:   alliberar la memòria de l'estructura utilitzada per emmagatzemar
                el valor de la comanda introduïda per l'usuari
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void alliberaMemoriaC();

/***********************************************
* @Finalitat:   alliberar la memòria de l'estructura que guarda la informació del
                fitxer de configuració
* @Parametres:  config: variable que conté camps a alliberar
* @Retorn:      ---
************************************************/
void alliberaMemoriaConfig(Config * config);

/***********************************************
* @Finalitat:   llegir un valor introduït fins a un caràcter en concret
* @Parametres:  fd: file descriptor on s'ha de fer la lectura
                end: caràcter que indicarà final de lectura
                lastChar: caràcter que s'ha de posar al final de la cadena llegida
* @Retorn:
************************************************/
char* readUntil(int fd, char end, char lastChar);

/***********************************************
* @Finalitat:   parsejar l'output de l'execució del fitxer show_connections.sh per 
                obtenir les ports disponibles
* @Parametres:  pipe: file descriptor que conté el pipe per llegir/escriure
                myPort: valor del port del servidor actual
* @Retorn:      ---
************************************************/
void buscaPorts(int pipe, int myPort);

/***********************************************
* @Finalitat:   Trobar els fitxers que es troben en un path conegut i afegir-los en 
                una cadena separats per \n
* @Parametres:  ---
* @Retorn:      cadena amb la llista de tots els audios
************************************************/
char * buscaAudios();

/***********************************************
* @Finalitat:   Buscar el fitxer a descarregar 
* @Parametres:  audio: nom del fitxer a descarregar
                sockfd: socket a qui enviar el fitxer
* @Retorn:      ---
************************************************/
void buscaDownload(char * audio, int sockfd);

/***********************************************
* @Finalitat:   Calcular el checksum d'un fitxer
* @Parametres:  path: ubicació del fitxer
* @Retorn:      cadena amb el valor del checksum
************************************************/
char * calculaChecksum (char *path);

/***********************************************
* @Finalitat:   Escriu, amb exclusió mútua, per pantalla el missatge rebut
* @Parametres:  missatge: cadena a imprimir per pantalla
* @Retorn:      ---
************************************************/
void escriuTerminal (char * missatge);

/***********************************************
* @Finalitat:   Destruir el semàfor que fa exclusió mútua de l'escriptura
                per pantalla
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void noMoreWrite();

#endif
