#include "logica.h"
#include "network.h"

<<<<<<< HEAD

void optionExit(){
    write(1,"Disconnecting Trinity...\n", strlen("Disconnecting Trinity...\n"));
    
    //desconnectem sockets
    closeConnections();
=======
void optionExit(int socket){
    write(1,"Disconnecting Trinity...\n", strlen("Disconnecting Trinity...\n"));
    
    //desconnectem sockets
    close(socket);
>>>>>>> 482b8d103e219943f1de3531571662df521a76d8
    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
}

void optionConnect(char* port, char *ip){
    write(1,"Connecting...\n", strlen("Connecting...\n"));
    connectClient(atoi(port), ip);
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

