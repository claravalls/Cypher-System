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

void optionShowC(char ** sysports){
    char * argv[4];
    /*int *connAvailable;

	connAvailable = malloc(sizeof(int));*/
	
    write(1,"Testing...\n", strlen("Testing...\n"));
    
    //L'últim argument ha de ser NULL. L'afegim
    argv[0] = (char *)malloc(sizeof(char)*22);
    strcpy(argv[0], "./show_connections.sh");
    
    argv[1] = (char *)malloc(sizeof(char)*strlen(sysports[0]));
    strcpy(argv[1], sysports[0]);

    argv[2] = (char *)malloc(sizeof(char)*strlen(sysports[1]));
    strcpy(argv[2], sysports[1]);

    argv[3] = NULL;

    pid_t pid = fork ();
    switch (pid){
        case 0:
            //ejecutar show_connections.sh y recoger SOLO los numeros de los puertos abiertos
            if(execvp(argv[0], argv) < 0){
                write(1, "Can not show connections\n", strlen("Can not show connections\n"));
            }
            break;
        case -1:
            write(1, "Can not show connections\n", strlen("Can not show connections\n"));
            break;
        default:
            //fer wait
            wait(NULL);
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

