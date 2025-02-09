#include "thread.h"

char apaga = 1;
pthread_mutex_t mtxC = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxS = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mtxArrayS = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxArrayC = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mtxqS = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxqC = PTHREAD_MUTEX_INITIALIZER;

UserThread *tServ;                  
UserThread *tCli;
int qTServ;
int qTCli;
pthread_t escolta;
int mySockfd;                       //Sockfd del meu servidor

static void *threadEscolta (void *config){
    int newsock;                    //socket que vol connectar-se
    char *clientName;               //nom del client que es connecta
    Config *c = (Config *) config;  //valors del fitxer de configuració
    Protocol p;                     //protocol de comunicació
    
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof (c_addr);
    mySockfd = c->sockfd;
        
    while (apaga){
        newsock = accept (mySockfd, (void *) &c_addr, &c_len);
        if (newsock < 0) {
            apaga = 0;
        }
        else{
            //esperem el missatge del client que s'ha connectat
            p = llegeixPaquet(newsock);
        
            if (p.data == NULL){
                //És una connexió falsa. La tanquem
                close(newsock);
            }
            else {
                asprintf(&clientName, "%s", p.data);
                afegeixClient(newsock, c->user, clientName);
                alliberaPaquet(p);
            }
        }
    }
    return NULL;
}

void apagaServidor(){
    apaga = 0;
    shutdown(mySockfd, SHUT_RDWR);

    pthread_join(escolta, NULL);

    //eliminem els semàfors quan ja s'han aturat tots els threads
    pthread_mutex_destroy(&mtxC);
    pthread_mutex_destroy(&mtxS);

    pthread_mutex_destroy(&mtxqC);
    pthread_mutex_destroy(&mtxqS);

    pthread_mutex_destroy(&mtxArrayS);
    pthread_mutex_destroy(&mtxArrayC);
}

static void *threadServ (void *servidor){
    Conn_serv *aux = (Conn_serv *) servidor; //informació del client que s'ha connectat
    Conn_serv *c;
    Protocol p;                              //protocol de comunicació
    char connectatS = 1;                     //variable que indica quan aturar el thread
    int audioFile;
    char primer = 1, *cadena, *audioName, *checksum, *path;

    c = (Conn_serv*)malloc(sizeof(Conn_serv));
    c->user = (char*)malloc(sizeof(char)*strlen(aux->user) + 1);
    strcpy(c->user, aux->user);
    c->sockfd = aux->sockfd;

    while (connectatS){
        //escoltem si el servidor ens envia un missatge
        p = llegeixPaquet(c->sockfd);
            
        switch(p.type){
            case 0x02:
                if(strcmp(p.header, "[MSG]") == 0){
                    imprimeixMissatge(p.data, c->user);
                    imprimeixPrompt();
                }
                alliberaPaquet(p);
                break;

            case 0x03:
                if(strcmp(p.header, "[MSGOK]") == 0){
                    asprintf(&cadena, COOL, c->user);
                    escriuTerminal(cadena);
                    free(cadena);
                    imprimeixPrompt();
                }
                alliberaPaquet(p);
                break;

            case 0x04:                
                if(strcmp(p.header, "[LIST_AUDIOS]") == 0){
                    escriuTerminal(p.data);
                    imprimeixPrompt();
                }
                alliberaPaquet(p);
                break;

            case 0x05:
                if(strcmp(p.header, "[AUDIO_RSPNS]") == 0){
                    if(primer){
                        //obrim el fitxer
                        audioFile = open(p.data, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                        primer = 0;

                        asprintf(&audioName, "%s", p.data); //p.data sera ./nomAudio

                        strcpy(&audioName[0], &audioName[1]); //traiem el . de la cadena
                        strcpy(&audioName[0], &audioName[1]); //traiem el / de la cadena
                        escriuTerminal(DOWNLOADING);
                    }
                    else{
                        write(audioFile, p.data, p.length);
                    }
                }
                else if(strcmp(p.header, "[EOF]") == 0){
                    //calculem el checksum de ./ + audioName
                    asprintf(&path, "./%s", audioName);
                    checksum = calculaChecksum(path);

                    if(strcmp(checksum, p.data) == 0){
                        enviaPaquet(c->sockfd, 0x05, "[MD5OK]", 0, NULL);
                        
                        asprintf(&cadena, TRANS_END, c->user, audioName);
                        escriuTerminal(cadena);
                        free(cadena);
                    }
                    else{
                        enviaPaquet(c->sockfd, 0x05, "[MD5KO]", 0, NULL);
                    }
                    
                    free(path);
                    free(checksum);
                    free(audioName);

                    primer = 1;
                    close(audioFile);
                    imprimeixPrompt();
                }
                
                else if(strcmp(p.header, "[AUDIO_KO]") == 0){
                    //no existeix l'audio 
                    escriuTerminal(AUDIOKO);
                    imprimeixPrompt();
                }
                alliberaPaquet(p);
                break;

            case 0x06:
                if(strcmp(p.header, "[]") == 0){
                    //enviar el paquet de desconnexió OK
                    enviaPaquet(c->sockfd, 0x06, "[CONOK]", 0, "");

                    asprintf(&cadena, ADEU_SERV, c->user);
                    escriuTerminal(cadena);
                    free(cadena);
                    imprimeixPrompt();

                    //S'ha de modificar l'array de conn_serv i restar una qServ;
                    pthread_mutex_lock(&mtxS);
                    eliminaConnexioServ(c->user);
                    pthread_mutex_unlock(&mtxS);

                    //aturem el thread
                    connectatS = 0;
                    close(c->sockfd);
                    free(c->user);
                    free(c);

                }else if(strcmp(p.header, "[CONOK]") == 0){
                    connectatS = 0;
                    close(c->sockfd);
                    free(c->user);
                    free(c);
                }
                alliberaPaquet(p);
                break;
        }
    }

    return NULL;
}

static void *threadCli (void *client){
    Conn_cli *aux= (Conn_cli *) client;  
    Conn_cli *c;                            //informació del client que s'ha connectat
    Protocol p;                             //protocol de comunicació
    char connectatC = 1;                    //variable que indica quan aturar el thread
    char *audiosShow;
    char *missatge;

    c = (Conn_cli*)malloc(sizeof(Conn_cli));
    c->user = (char*)malloc(sizeof(char)*strlen(aux->user) + 1);
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
                alliberaPaquet(p);
                break;

            case 0x03:
                if(strcmp(p.header, "[BROADCAST]") == 0){
                    imprimeixMissatge(p.data, c->user);
                    imprimeixPrompt();
                    enviaPaquet(c->sockfd, 0x03, "[MSGOK]", 0, NULL);
                }
                alliberaPaquet(p);
                break;

            case 0x04:
                if(strcmp(p.header, "[SHOW_AUDIOS]") == 0){
                    enviaPaquet(c->sockfd, 0x02, "[MSG]", strlen(SHOW_REBUT), SHOW_REBUT);
                    free(missatge);
                    audiosShow = buscaAudios();
                    enviaPaquet(c->sockfd, 0x04, "[LIST_AUDIOS]", strlen(audiosShow), audiosShow);
                    free(audiosShow);
                }
                alliberaPaquet(p);
                break;

            case 0x05:
                if(strcmp(p.header, "[AUDIO_RQST]") == 0){
                    //solicitud audio
                    buscaDownload(p.data, c->sockfd);
                   
                }else if(strcmp(p.header, "[MD5KO]") == 0){
                    enviaPaquet(c->sockfd, 0x02, "[MSG]", strlen(MD5KO), MD5KO);
                }
                alliberaPaquet(p);
                break;

            case 0x06:
                if(strcmp(p.header, "[]") == 0){
                    //enviar el paquet de desconnexió OK
                    enviaPaquet(c->sockfd, 0x06, "[CONOK]", 0, "");

                    asprintf(&missatge, ADEU_CLIENT, c->user);
                    escriuTerminal(missatge);
                    free(missatge);
                    imprimeixPrompt();

                    //S'ha de modificar l'array de conn_serv i restar una qServ;
                    pthread_mutex_lock(&mtxC);
                    eliminaConnexioCli(c->user);
                    pthread_mutex_unlock(&mtxC);

                    //aturem el thread
                    connectatC = 0;

                    close(c->sockfd);
                    free(c->user);
                    free(c);
                }
                else if(strcmp(p.header, "[CONOK]") == 0){
                    connectatC = 0;
                    
                    close(c->sockfd);
                    free(c->user);
                    free(c);
                }
                alliberaPaquet(p);
                break;
        }

    }
    return NULL;
}

void iniciaThreadServidor(Conn_serv* servidor, char *user){
    pthread_mutex_lock(&mtxqS);
    //augmentem la mida de l'array
    if(qTServ == 0){
        tServ = (UserThread*)malloc(sizeof(UserThread));
    }
    else{
        tServ = (UserThread*)realloc(tServ, sizeof(UserThread)*(qTServ+1));
    }
    
    tServ[qTServ].user = user;
    tServ[qTServ].listener = servidor->sockfd;

    pthread_create(&(tServ[qTServ].t), NULL, threadServ, servidor);
    qTServ++;
    pthread_mutex_unlock(&mtxqS);
}

void iniciaThreadClient(Conn_cli* client, char *user){
    pthread_mutex_lock(&mtxqC);
    //augmentem la mida de l'array
    if(qTCli == 0){
        tCli = (UserThread*)malloc(sizeof(UserThread));
    }
    else{
        tCli = (UserThread*)realloc(tCli, sizeof(UserThread)*(qTCli+1));
    }
    
    tCli[qTCli].user = user;
    tCli[qTCli].listener = client->sockfd;

    pthread_create(&(tCli[qTCli].t), NULL, threadCli, client);
    qTCli++;
    pthread_mutex_unlock(&mtxqC);
}

void iniciaThreadEscolta(Config* config){
    pthread_create(&escolta, NULL, threadEscolta, config);
    
    pthread_mutex_lock(&mtxqS);
    qTServ = 0;
    pthread_mutex_unlock(&mtxqS);
    pthread_mutex_lock(&mtxqC);
    qTCli = 0;
    pthread_mutex_unlock(&mtxqC);
}

void joinUserThread(char *user){
    pthread_mutex_lock(&mtxqS);
    for (int i = 0; i < qTServ; i++)
    {
        pthread_mutex_unlock(&mtxqS);
        if (strcmp(user, tServ[i].user) == 0)
        {
            //esperem que el thread acabi
            pthread_join(tServ[i].t, NULL);
            pthread_mutex_lock(&mtxArrayS);
            pthread_mutex_lock(&mtxqS);
            //l'eliminem de l'array
            shiftJoins(tServ, user, qTServ);   
            qTServ--;
            i--;
            pthread_mutex_unlock(&mtxqS);
            pthread_mutex_unlock(&mtxArrayS);
        }
    }

    pthread_mutex_lock(&mtxqC);
    for (int i = 0; i < qTCli; i++)
    {
        pthread_mutex_unlock(&mtxqC);
        if (strcmp(user, tCli[i].user) == 0)
        {
            //esperem que el thread acabi
            pthread_join(tCli[i].t, NULL);
            pthread_mutex_lock(&mtxArrayC);
            pthread_mutex_lock(&mtxqC);
            //l'eliminem de l'array
            shiftJoins(tCli, user, qTCli);
            qTCli--;
            i--;
            pthread_mutex_unlock(&mtxqC);
            pthread_mutex_unlock(&mtxArrayC);
        }
    }
}

void shiftJoins(UserThread *tThread, char *user, int lenght){
    int s = 0, b;
    //busquem al client a l'array
    for (b = 0; b < lenght; b++){
        if(strcmp(user, tThread[b].user) == 0){
            s = b + 1;
            
            if(s < lenght){ //si hem d'eliminar l'últim valor no cal shiftar
                //shiftem els valors a l'esquerra
                for (int i = b; i < lenght; i++){
                    tThread[b] = tThread[s];
                    b++;
                    s++;
                }
            }

            if(lenght == 1){ //es l'ultim client
                free(tThread);
            }
        
            break;
        }
    }
}