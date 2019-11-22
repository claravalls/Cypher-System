#ifndef _MANAGER_H_
#define _MANAGER_H_

//Llibreries del sistema
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "logica.h"
#include "network.h"

#define ERR_FILE "Error opening file...\n"
#define CONN_AVAIL "%d connections available\n"
#define PROMPT "\n$%s: "

/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void setConfig(Config c);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void imprimeixPrompt();
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
Config lecturaFitxer(const char *fitxer);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void separaComanda(char *comanda, char limit, int i, int casella);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
char llegeixComanda(char * comanda);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void alliberaMemoriaC();
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void alliberaMemoriaConfig(Config * config);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
char ** getValues();
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
char* readUntil(int fd, char end, char lastChar);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void buscaPorts(int pipe, int myPort);

#endif
