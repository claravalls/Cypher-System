#include "thread.h"

char apaga = 1;
pthread_mutex_t mtxC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxS = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mtxArrayS = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxArrayC = PTHREAD_MUTEX_INITIALIZER;

UserThread *tServ;                  //array de threads iniciats per escoltar un Servidor
UserThread *tCli;                   //array de threads iniciats per escoltar un Client
int qTServ;                         //quantitat de threads de Servidors
int qTCli;                          //quantitat de threads de Clients


static void *threadEscolta (void *config){
    int newsock;                    //socket que vol connectar-se
    int socket;                     //socket del meu servidor
    char *clientName;               //nom del client que es connecta
    Config *c = (Config *) config;  //valors del fitxer de configuració
    Protocol p;                     //protocol de comunicació
    
    while (apaga){
        //Preparem per acceptar un nou client
        struct sockaddr_in s_addr;
        socklen_t len = sizeof (s_addr);
        socket = c->sockfd;
        newsock = accept (socket, (void *) &s_addr, &len);
        if (newsock < 0) {
            //No s'ha pogut acceptar
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
            //Afegim el client a l'array de connexions
            afegeixClient(newsock, c->user, clientName);
        }
    }
    return (void *) c;
}

void apagaServidor(){
    apaga = 0;
}

static void *threadServ (void *servidor){
    Conn_serv *aux = (Conn_serv *) servidor;    //informació del client que s'ha connectat
    Conn_serv *c;                               
    Protocol p;                                 //protocol de comunicació
    char connectatS = 1;                        //variable que indica quan aturar el thread
    
    c = (Conn_serv*)malloc(sizeof(Conn_serv));
    c->user = (char*)malloc(strlen(aux->user) + 1);
    strcpy(c->user, aux->user);
    c->sockfd = aux->sockfd;

    while (connectatS){
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
                if(strcmp(p.header, "[]") == 0){
                    //enviar el paquet de desconnexió OK
                    enviaPaquet(c->sockfd, 0x06, "[CONOK]", 0, "");

                    //Actualitzem l'array de connexions eliminant la corresponent a aquest thread
                    pthread_mutex_lock(&mtxS);
                    eliminaConnexioServ(c->user);
                    pthread_mutex_unlock(&mtxS);

                }else if(strcmp(p.header, "[CONOK]") == 0){
                    //aturem el thread i tanquem el file descriptor
                    connectatS = 0;  
                    close(c->sockfd);

                    //alliberem la memòria de c:
                    free(c->user);
                    free(c);
                }
                break;
        }
    }
    //eliminem el semàfor utilitzat
    pthread_mutex_destroy(&mtxS);

    return NULL;
}

static void *threadCli (void *client){
    Conn_cli *aux= (Conn_cli *) client;     //informació del client que s'ha connectat
    Conn_cli *c;                            
    Protocol p;                             //protocol de comunicació
    char connectatC = 1;                    //variable que indica quan aturar el thread

    //Demanem memòria per guardar la informació del client
    c = (Conn_cli*)malloc(sizeof(Conn_cli));
    c->user = (char*)malloc(sizeof(char)*strlen(aux->user));
    strcpy(c->user, aux->user);
    c->sockfd = aux->sockfd;

    while (connectatC){
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
                if(strcmp(p.header, "[]") == 0){
                    //enviar el paquet de desconnexió OK
                    enviaPaquet(c->sockfd, 0x06, "[CONOK]", 0, "");

                    //Actualitzem l'array de connexions eliminant la corresponent a aquest thread
                    pthread_mutex_lock(&mtxC);
                    eliminaConnexioCli(c->user);
                    pthread_mutex_unlock(&mtxC);
                }
                else if(strcmp(p.header, "[CONKO]") != 0){
                    //aturem el thread i tanquem el file descriptor
                    connectatC = 0;
                    close(c->sockfd);

                    //alliberem la memòria de c:
                    free(c->user);
                    free(c);
                }
                break;
        }
    }
    //eliminem el semàfor utilitzat
    pthread_mutex_destroy(&mtxC);
    return NULL;
}

void iniciaThreadServidor(Conn_serv* servidor, char *user){
    //redimensionem l'array de threads de Servidors
    tServ = (UserThread*)realloc(tServ, sizeof(UserThread)*(qTServ+1));
    
    //guardem la informació del servidor
    tServ[qTServ].user = user;
    tServ[qTServ].listener = servidor->sockfd;
    
    //creem el thread
    pthread_create(&(tServ[qTServ].t), NULL, threadServ, servidor);
    qTServ++;
}

void iniciaThreadClient(Conn_cli* client, char *user){
    //redimensionem l'array de threads de Clients
    tCli = (UserThread*)realloc(tCli, sizeof(UserThread)*(qTCli+1));

    //Guardem la infomració del client
    tCli[qTCli].user = user;
    tCli[qTCli].listener = client->sockfd;

    //creem el thread
    pthread_create(&(tCli[qTCli].t), NULL, threadCli, client);
    qTCli++;
}

void iniciaThreadEscolta(Config* config){
    pthread_t t1;
    pthread_create(&t1, NULL, threadEscolta, config);

    //inicialitzem l'array de threads de Servidor i Client i els comptadors
    tServ = (UserThread*)malloc(sizeof(UserThread));
    tCli = (UserThread*)malloc(sizeof(UserThread));
    qTServ = 0;
    qTCli = 0;
}


void joinUserThread(char *user){
    //Recorrerem l'array de threads creats escoltant a Servidors
    for (int i = 0; i < qTServ; ++i){   
        //Mirem si el thread pertany a l'usuari que es vol desconnectar
        if (strcmp(user, tServ[i].user) == 0){
            //aquest enviament aturarà el thread del Servidor que escolta al Client que es vol desconnectar
            enviaPaquet(tServ[i].listener, 0x06, "[CONOK]", strlen(user), user);

            //esperem que el thread acabi
            pthread_join(tServ[i].t, NULL);

            //actualitzem l'array de threads del Servidor
            pthread_mutex_lock(&mtxArrayS);
            shiftJoins(tServ, user, qTServ);                    
            qTServ--;
            pthread_mutex_unlock(&mtxArrayS);
        }
    }

    //Recorrerem l'array de threads creats escoltant a Clients
    for (int i = 0; i < qTCli; ++i){
        //Mirem si el thread pertany a l'usuari que es vol desconnectar
        if (strcmp(user, tCli[i].user) == 0){
            //enviem el paquet que aturarà el thread del Client que escolta al Servidor que es vol desconnectar
            enviaPaquet(tCli[i].listener, 0x06, "[CONOK]", strlen(user), user);

            //esperem que el thread acabi
            pthread_join(tCli[i].t, NULL);

            //actualitzem l'array de threads del Client
            pthread_mutex_lock(&mtxArrayC);
            shiftJoins(tCli, user, qTCli);
            qTCli--;
            pthread_mutex_unlock(&mtxArrayC);
        }
    }

    //destruïm els semàfors
    pthread_mutex_destroy(&mtxArrayS);
    pthread_mutex_destroy(&mtxArrayC);
}


void shiftJoins(UserThread *tThread, char *user, int lenght){
    int s = 0, b;

    //busquem al client a l'array
    for (b = 0; b < lenght; b++){
        if(strcmp(user, tThread[b].user) == 0){
            s = b + 1;
            //shiftem els valors a l'esquerra
            for (int i = b; i < lenght; i++){
                tThread[b] = tThread[s];
                b++;
                s++;
            }
            tThread = (UserThread *) realloc(tThread, sizeof(UserThread) * (lenght - 1));
        }
    }
}



