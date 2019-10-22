#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#define SHOW_CONNECTIONS 1
#define CONNECT 2
#define SAY 3
#define BROADCAST 4 
#define SHOW_AUDIOS 5
#define DOWNLOAD 6 
#define EXIT 7

typedef struct{
    char user[50];
    char dirAudios[50];
    char ip[16];
    int port;
    char ipWeb[50];
    int *sysports;
}Config;


char llegeixComanda(char * comanda);
void optionExit();
void optionConnect();
void optionSay();
void optionShowC();
void optionShowA();
void optionBroadcast();