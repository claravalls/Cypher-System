#include "network.h"

//retorna 1 si no s'ha pogut connectar
int connect(int port, char * ip, char * user){
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
    wrtie (1, okMessage, strlen(okMessage));
    return 0;
}