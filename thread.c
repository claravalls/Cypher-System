#include "thread.h"

static void *threadFunc (void *config){
    int newsock;
    int socket;
    Config *c = (Config *) config;
    
    while (1){
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        socket = c->sockfd;
        newsock = accept (socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            return (void *) -1;
        }
        afegeixClient(newsock, c->user);
    }
    return (void *) c;
}

void iniciaThread(Config* config){
    pthread_t t1;
    pthread_create(&t1, NULL, threadFunc, config);
}