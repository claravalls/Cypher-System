#include "thread.h"

char apaga = 1;
pthread_mutex_t mtxC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxS = PTHREAD_MUTEX_INITIALIZER;

UserThread *tServ;
UserThread *tCli;
int qServ;
int qCli;

static void *threadEscolta (void *config){
    int newsock;                    //socket que vol connectar-se
    int socket;                     //socket del meu servidor
    char *clientName;               //nom del client que es connecta
    Config *c = (Config *) config;  //valors del fitxer de configuració
    Protocol p;                     //protocol de comunicació
    
    while (apaga){
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        socket = c->sockfd;
        newsock = accept (socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            return (void *) -1;
        }

        //esperem el missatge del client que s'ha connectat
        p = llegeixPaquet(newsock);
        clientName = p.data;
        if (clientName == NULL){
            //És una connexió falsa. La tanquem
            close(newsock);
        }
        else {
            afegeixClient(newsock, c->user, clientName);
        }
    }
    return (void *) c;
}

void apagaServidor(){
    apaga = 0;
}

static void *threadServ (void *servidor){
    Conn_serv *aux = (Conn_serv *) servidor;   //informació del client que s'ha connectat
    Conn_serv *c;
    Protocol p;                             //protocol de comunicació
    char connectat = 1;                     //variable que indica quan aturar el thread
    
    c = (Conn_serv*)malloc(sizeof(Conn_serv));

    c->user = (char*)malloc(sizeof(char)*strlen(aux->user));

    strcpy(c->user, aux->user);
    c->sockfd = aux->sockfd;

    while (connectat){
        //escoltem si el servidor ens envia un missatge
        p = llegeixPaquet(c->sockfd);
        switch(p.type){
            case 0x02:
                break;

            case 0x03:
                break;

            case 0x04:
                break;

            case 0x05:
                break;

            case 0x06:
            printf("id C: %d\n", c->sockfd);
                if(strcmp(p.header, "[]") == 0){
                    printf("rebem el paquet amb header %s del servidor %s\n", p.header, c->user);
                    //enviar el paquet de desconnexió OK
                    printf("El servidor %s es vol desconnectar\n", c->user);
                    enviaPaquet(c->sockfd, 0x06, "[CONOK]", 0, "");

                    printf("Paquet de desconnexio enviat al sock %d\n", c->sockfd);

                    //S'ha de modificar l'array de conn_serv i restar una qServ;
                    pthread_mutex_lock(&mtxS);
                    eliminaConnexioServ(c->user);
                    pthread_mutex_unlock(&mtxS);

                    connectat = 0;
                }else if(strcmp(p.header, "[CONOK]") == 0){
                    printf("L'usuari %s ha rebut el missatge\n", c->user);
                    //VIGILAR AMB SEMÀFORS
                    connectat = 0;
                    //close(c->sockfd);
                }
                //No llegirem aquí perquè sino no estarà sincronitzat
                break;
        }
    }

    pthread_mutex_destroy(&mtxS);

    return (void *) c;
}

static void *threadCli (void *client){
    Conn_cli *aux= (Conn_cli *) client;  
    Conn_cli *c;                            //informació del client que s'ha connectat
    Protocol p;                             //protocol de comunicació
    char connectat = 1;                     //variable que indica quan aturar el thread

    c = (Conn_cli*)malloc(sizeof(Conn_cli));

    c->user = (char*)malloc(sizeof(char)*strlen(aux->user));

    strcpy(c->user, aux->user);
    c->sockfd = aux->sockfd;

    while (connectat){
        //escoltem si el client ens envia un missatge

        p = llegeixPaquet(c->sockfd);
        switch(p.type){
            case 0x02:
                if(strcmp(p.header, "[MSG]") == 0){
                    imprimeixMissatge(p.data, c->user);
                    imprimeixPrompt();
                }
                break;

            case 0x03:
                break;

            case 0x04:
                break;

            case 0x05:
                break;

            case 0x06:
            printf("id C: %d\n", c->sockfd);
                if(strcmp(p.header, "[]") == 0){
                    printf("rebem el paquet amb header %s del client %s\n", p.header, c->user);
                    //enviar el paquet de desconnexió OK
                    printf("L'usuari %s es vol desconnectar\n", c->user);
                                printf("id D: %d\n", c->sockfd);

                    enviaPaquet(c->sockfd, 0x06, "[CONOK]", 0, "");

                    printf("Paquet de desconnexio enviat al sock %d\n", c->sockfd);
                    //vigilar amb semàfors de no tancar abans que ho llegeixi
                    //close(c->sockfd);

                    //S'ha de modificar l'array de conn_serv i restar una qServ;
                    pthread_mutex_lock(&mtxC);
                    eliminaConnexioCli(c->user);
                    //close(c->sockfd);
                    pthread_mutex_unlock(&mtxC);
                }
                else if(strcmp(p.header, "[CONOK]") == 0){
                    printf("L'usuari %s ha rebut el missatge\n", c->user);
                    //VIGILAR AMB SEMÀFORS
                    connectat = 0;
                    //close(c->sockfd);
                }
                break;
        }
    }

    pthread_mutex_destroy(&mtxC);
    return (void *) c;
}

void iniciaThreadServidor(Conn_serv* servidor){
    tServ = (UserThread*)realloc(tServ, sizeof(UserThread)*(qServ+1));
    tServ[qServ].user = servidor->user;

    pthread_create(&(tServ[qServ].t), NULL, threadServ, servidor);
    qServ++;
}

void iniciaThreadClient(Conn_cli* client){
    tCli = (UserThread*)realloc(tCli, sizeof(UserThread)*(qCli+1));
    tCli[qCli].user = client->user;

    pthread_create(&(tCli[qCli].t), NULL, threadCli, client);
    qCli++;
}

void iniciaThreadEscolta(Config* config){
    pthread_t t1;
    pthread_create(&t1, NULL, threadEscolta, config);

    tServ = (UserThread*)malloc(sizeof(UserThread));
    tCli = (UserThread*)malloc(sizeof(UserThread));
    qServ = 0;
    qCli = 0;
}




void joinUserThread(char *user){

    for (int i = 0; i < qServ; ++i)
    {
        if (strcmp(user, tServ[i].user) == 0)
        {
            pthread_join(tServ[i].t, NULL);
        }
    }

    for (int i = 0; i < qCli; ++i)
    {
        if (strcmp(user, tCli[i].user) == 0)
        {
            pthread_join(tCli[i].t, NULL);
        }
    }

}



