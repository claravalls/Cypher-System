#include "logica.h"
#include "network.h"

void optionExit(){
    write(1,DISCONNECT, strlen(DISCONNECT));
    
    //desconnectem sockets
    closeConnections();

    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
}

void optionConnect(char* port, char *ip, char *username){
    write(1, CONNECTING, CONNECTING);
    connectClient(atoi(port), ip, username);
}


void optionSay(){

}

void optionShowC(char ** sysports, int myPort){
    char * argv[4] = {"./show_connections.sh", sysports[0], sysports[1], NULL};
    int fd[2];
    /*int *connAvailable;

	connAvailable = malloc(sizeof(int));*/
	
    write(1, TESTING, strlen(TESTING));

    if (pipe(fd) == -1){
        write(1, ERR_PIPE, strlen (ERR_PIPE));
        exit(-1);
    }
    
    pid_t pid = fork ();
    switch (pid){
        case 0:
            close(fd[0]);
            dup2(fd[1], 1);

            //executem show_connections.sh
            if(execvp(argv[0], argv) < 0){
                write(1, ERR_CONN, strlen(ERR_CONN));
            } 

            //HAIG DE TANCAR FD[1]??
            break;
        case -1:
            write(1, ERR_CONN, strlen(ERR_CONN));
            break;
        default:
            //esperem que acabi d'escriure
            wait(NULL);
            close(fd[1]);
            //llegim del pipe i busquem els ports
            buscaPorts(fd[0], myPort);
            close(fd[0]);
            break;
    }
}

void optionShowA(){


}


void optionBroadcast(){


}

void optionDownload(){

}



void stopAll(){
	write(1,"\n", strlen("\n"));
    optionExit();
    kill(getpid(), SIGINT);
}

