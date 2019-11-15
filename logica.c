#include "logica.h"
#include "network.h"

void optionExit(int socket){
    write(1,"Disconnecting Trinity...\n", strlen("Disconnecting Trinity...\n"));
    
    //desconnectem sockets
    close(socket);
    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
}

void optionConnect(int port, char *ip, char *user, int sockfd){
    write(1,"Connecting...\n", strlen("Connecting...\n"));
    connectClient(port, ip, user, sockfd);

}


void optionSay(){


}

void optionShowC(){
	int *connAvailable;

	connAvailable = malloc(sizeof(int));
	
    write(1,"Testing...\n", strlen("Testing...\n"));
    //ejecutar show_connections.sh y recoger SOLO los numeros de los puertos abiertos

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

