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

#define CONN_AVAIL "%d connections available\n"

Config lecturaFitxer(const char *fitxer);
void separaComanda(char *comanda, char limit, int i, int casella);
char llegeixComanda(char * comanda);
void alliberaMemoriaC();
void alliberaMemoriaConfig(Config * config);
char ** getValues();
char* readUntil(int fd, char end, char lastChar);
void buscaPorts(int pipe, int myPort);

#endif
