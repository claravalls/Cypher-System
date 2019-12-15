#include "logica.h"
#include "network.h"

void optionExit(){
    write(1,DISCONNECT, strlen(DISCONNECT));

    //avisem als clients
    tancaConnexions();

    //alliberem la memòria de les connexions
    freeConnections();

    //deixem d'escoltar connexions
    apagaServidor();

    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
}

void optionConnect(char* port, char *ip, char *username){
    write(1, CONNECTING, strlen(CONNECTING));
    connectClient(atoi(port), ip, username);
}

void optionSay(char ** comanda){
    enviaMissatge(comanda[1], comanda[2]);
}

void optionShowC(char ** sysports, int myPort, char *ip){
    int fd[2];          //variable que farà de pipe

    printf("IP: %s\n",ip);
    //creem els arguments a passar-li al procés show_connections.sh
    char * argv[5] = {"./show_connections_v2.sh", sysports[0], sysports[1], ip, NULL};
	
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
            break;
        case -1:
            write(1, ERR_CONN, strlen(ERR_CONN));
            break;
        default:
            close(fd[1]);
            //esperem que acabi d'escriure
            wait(NULL);
            //llegim del pipe i busquem els ports
            buscaPorts(fd[0], myPort);
            close(fd[0]);
            break;
    }
}

void optionShowA(char * username){
    enviaShowAudios(username);
}

void optionBroadcast(){

}

void optionDownload(char *user, char *audio){
    enviaDownloadAudio(user, audio);
}

void stopAll(){
	write(1,"\n", strlen("\n"));
    optionExit();
    int kill = raise(SIGINT);
}