#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

typedef struct{
    char user[50];
    char dirAudios[50];
    char ip[16];
    int port;
    char ipWeb[50];
    int *sysports;
}Config;