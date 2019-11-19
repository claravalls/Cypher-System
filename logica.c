#include "logica.h"
#include "network.h"

void optionExit(){
    write(1,"Disconnecting Trinity...\n", strlen("Disconnecting Trinity...\n"));
    
    //desconnectem sockets
    closeConnections();

    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
}

void optionConnect(char* port, char *ip){
    write(1,"Connecting...\n", strlen("Connecting...\n"));
    connectClient(atoi(port), ip);
}


void optionSay(){

}

void optionShowC(char ** sysports, int myPort){
    char * argv[4];
    int fd[2];
    /*int *connAvailable;

	connAvailable = malloc(sizeof(int));*/
	
    write(1,"Testing...\n", strlen("Testing...\n"));

    if (pipe(fd) == -1){
        write(1, "Can not create pipe\n", strlen ("Can not create pipe\n"));
        exit(-1);
    }
    
    pid_t pid = fork ();
    switch (pid){
        case 0:
            close(fd[0]);
            //creem els arguments
            argv[0] = (char *)malloc(sizeof(char)*22);
            strcpy(argv[0], "./show_connections.sh");
            
            argv[1] = (char *)malloc(sizeof(char)*strlen(sysports[0]));
            strcpy(argv[1], sysports[0]);

            argv[2] = (char *)malloc(sizeof(char)*strlen(sysports[1]));
            strcpy(argv[2], sysports[1]);
            
            //L'últim argument ha de ser NULL. L'afegim
            argv[3] = NULL;

            //redireccionem la sortida al pipe
            //output = open("output", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            dup2(fd[1], 1);

            //executem show_connections.sh
            if(execvp(argv[0], argv) < 0){
                dup2(1, fd[1]);
                write(1, "Can not show connections\n", strlen("Can not show connections\n"));
            } 

            //HAIG DE TANCAR FD[1]??
            break;
        case -1:
            write(1, "Can not show connections\n", strlen("Can not show connections\n"));
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

