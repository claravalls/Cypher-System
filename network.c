#include "network.h"

int *clients;
int quants;

void afegeixClient(int newsock){
    clients = (int*)realloc(clients, sizeof(int) * (quants + 1));
    clients[quants] = newsock;
    quants++;
}

static void *threadFunc (void *sockfd){
    int newsock;
    
    while (1){
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        int *socket = (int*) malloc(sizeof(int));
        socket = (int*) (sockfd);
        newsock = accept (*socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            break;
        }
        free(socket);
        afegeixClient(newsock);
    }
    return (void *) sockfd;
}

int connectServer(const char* ip, int port){
    int sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in s_addr;

    memset (&s_addr, 0, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (port);
    if (inet_aton (ip, &s_addr.sin_addr) == 0) {
        return -1;
    }

    if (bind (sockfd, (void *) &s_addr, sizeof (s_addr)) < 0) {
        return -1;
    }

    listen (sockfd, 3);

    clients = (int*) malloc (sizeof(int));
    quants = 0;

    return sockfd;
}

int connectClient(int port, char * ip, char * user, int sockfd){
    char okMessage[35];
    //connect
    struct sockaddr_in s_addr;
    memset (&s_addr, 0, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (port);
    if (inet_aton (ip, &s_addr.sin_addr) == 0) {
        write (1, "No es una adrea IP valida\n", strlen("No es una adrea IP valida\n"));
        return -1;
    }

    if (connect (sockfd, (void *) &s_addr, sizeof (s_addr)) < 0){
        return 1;
    }

    sprintf (okMessage, "%d connected: %s", port, user);
    write (1, okMessage, strlen(okMessage));
    return 0;
}