#ifndef _LOGICA_H_
#define _LOGICA_H_


#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define SHOW_CONNECTIONS 1
#define CONNECT 2
#define SAY 3
#define BROADCAST 4 
#define SHOW_AUDIOS 5
#define DOWNLOAD 6 
#define EXIT 7

#define DISCONNECT "Disconnecting Trinity...\n"
#define CONNECTING "Connecting...\n"
#define TESTING "Testing...\n"
#define ERR_PIPE "Can not create pipe\n"
#define ERR_CONN "Can not show connections\n"
#define STARTING "\nStarting Trinity...\n"
#define ERR_OPTION "Error, invalid option\n"
#define ERR_ARGS "Not enough arguments\n"

//Estructura de la configuració del fitxer .dat
typedef struct{
    char *user;
    char *dirAudios;
    char *ip;
    int port;
    char *ipWeb;
    char **sysports;
    int sockfd;
}Config;

/***********************************************
* @Finalitat:   Finalitzar el procés assegurant-se que totes les connexions 
                es tanquen i alliberant tota la memòria demanada
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void optionExit();
/***********************************************
* @Finalitat:   Implementar la connexió del client a un servidor
* @Parametres:  port: cadena que conté el port al qual connectar-se
                ip: direcció IP on ens volem connectar
                username: nom del meu usuari
* @Retorn:      ---
************************************************/
void optionConnect(char* port, char *ip, char *username);
/***********************************************
* @Finalitat:   
* @Parametres:
* @Retorn:
************************************************/
void optionDownload();
/***********************************************
* @Finalitat:   Enviar un missatge a un usuari
* @Parametres:  comanda: array de cadenes que contenen les paraules de la 
                comanda introduïda
* @Retorn:      ---
************************************************/
void optionSay(char ** comanda);
/***********************************************
* @Finalitat:   mostrar els ports les connexions disponibles
* @Parametres:  sysports: array de dues caselles que contenen el primer i 
                l'últim rang a escanejar
                myPort: valor del port del propi servidor
* @Retorn:      ---
************************************************/
void optionShowC(char ** sysports, int myPort);
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void optionShowA();
/***********************************************
* @Finalitat:
* @Parametres:
* @Retorn:
************************************************/
void optionBroadcast();
/***********************************************
* @Finalitat:   Funció executada al rebre un Ctrl+C. S'assegura de tancar les
                connexions obertes i alliberar la memòria demanada.
* @Parametres:  ---
* @Retorn:      ---
************************************************/
void stopAll();

#endif