#include "logica.h"
#include "network.h"

void optionExit(){
    write(1,"Disconnecting Trinity...\n", strlen("Disconnecting Trinity...\n"));
    
    //desconnectem sockets

    //reconfigurem signals
    signal(SIGINT, SIG_DFL);
}

void optionConnect(int port, char *ip, char *user){
    write(1,"Connecting...\n", strlen("Connecting...\n"));
    connectClient(port, ip, user);

}


void optionSay(){


}

void optionShowC(){
    write(1,"Testing...\n", strlen("Testing...\n"));

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

