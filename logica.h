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

typedef struct{
    char *user;
    char *dirAudios;
    char *ip;
    int port;
    char *ipWeb;
    char **sysports;
    int sockfd;
}Config;

void optionExit();
void optionConnect(char* port, char *ip, char *username);
void optionDownload();
void optionSay();
void optionShowC(char ** sysports, int myPort);
void optionShowA();
void optionBroadcast();
void stopAll();

#endif