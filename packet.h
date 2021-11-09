#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <sys/wait.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define DEFAULTSIZE 42
#define ACQ 16
#define TAILLEFEN 42


char ID=0;


/**
 * @brief genere un int aleatoire entre 0 et max 
 * 
 * @param max 
 * @return int 
 */
int generateRandInt(int max){
    srand(time(NULL));
    int r = rand() % max;
    return r;
} 

/**
 * @brief structure du message ou packet 
 * @struct packet 
 */
typedef struct packet
{
    char id ; 
    char type ;
    short seq  ;
    short acq  ; 
    char ecn ;
    char fenetre  ;
    char data[42] ;  
}packet;

/**
 * @brief initiale les champs du packet 
 * 
 * @return struct packet 
 */
struct packet  init_packet(){
    struct packet p ; 
    p.id = 0 ;
    p.type = 0 ;
    p.seq =0 ;
    p.ecn = 0 ;
    p.acq=0;
    p.fenetre=42;
    memset(p.data,'\0',42);
    return p;
}

/**
 * @brief Renvoie la cause de l'erreur + exit a 1 
 * 
 * @param message 
 */
void raler(char *message) 
{
	perror(message);
	exit(EXIT_FAILURE); 
}


/**
 * @brief Calcul pa puissance de a par b 
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int puissance ( int a, int b ){
    int resultat =1;
        for(int i=0;i<b;i++){
            resultat*=a;
        }
    return resultat;
}

/**
 * @brief Creer un socket et verifier si erreur, exit si erreur 
 * 
 * @param desc 
 * @return int 
 */
int creationSocket (int desc){
    if ((desc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        raler("socket");
    }
return desc;
}



/**
 * @brief Procede du 4 way handshake du cote serveur 
 * 0 si bonne fermeture, 1 sinon
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int fermeture_connection_serveur(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie)
{
    socklen_t size=sizeof(ecoute);
    struct packet p = init_packet();
    p.type = 16 ; 

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    while(1){
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
        if(sen==-1){
            raler("send etablisemment \n ");
        }

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
            if(retval==-1){
                printf("select fermeture\n");
            }

        if(FD_ISSET(s,&fd_monitor)){
            p.type = 2 ;
            int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
            if(sen==-1){
                raler("send etablisemment \n ");
            }

            if(FD_ISSET(s,&fd_monitor)){
                int r =recvfrom(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                if(p.type == 16){
                    printf("ACQ bien recu \n");
                    if(close(s)==-1){
                        raler("close");
                    }
                    exit(0);  
                }
            }
        }
    }

}








/**
 * @brief Procede du 4 way handshake du cote source(client) 
 * 0 si bonne fermeture, 1 sinon 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int fermeture_connection_source(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie)
{
    socklen_t size=sizeof(ecoute);
    struct packet p = init_packet();
    p.type=2; 

    


    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    while(1){
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        
        int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
        if(sen==-1){
            raler("send etablisemment \n ");
        }

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            printf("select etablissement\n");
        }

        if(FD_ISSET(s,&fd_monitor)){
            int r =recvfrom(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,&size);
            if(r==-1){
                raler("recvfrom 1\n");
            }
            if(p.type == 16){
                printf("Premier acquittement recu \n");
            }
            if(p.type == 2){

                printf("jai recu mon message de fin, jenvoie mon acq \n");
                p.type = 16 ; 
                sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
                if(sen==-1){
                    raler("send etablisemment \n ");
                }
                printf("je ferme ma connexion \n");
                if(close(s)==-1){
                    raler("close");
                }  
                exit(0);
            }
            
        }else{
            continue;
        }
    }
    exit(1);

} 



/**
 * @brief Procede du 3 way handshake du cote source(client) 
 * 0 si bonne fermeture, 1 sinon 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */

int etablissementConnexionSource(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){

    unisgned short a = generateRandInt(100);
    struct packet p=init_packet() ;
    socklen_t size=sizeof(ecoute);

    
    p.id=0;
    p.seq=a;
    printf("ETAPE1 : J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    while(1){
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        
        int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
        if(sen==-1){
            raler("send etablisemment \n ");
        }

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            printf("select etablissement\n");
        }

        if(FD_ISSET(s,&fd_monitor)){
                printf("data ready\n");//Je receive et je test et si tout va bien je renvois avec les nouvelles valeurs
                //recevfrom
                int r =recvfrom(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                //deroulement de test
                printf("ETAPE2 : J'ai recu seq à %d et ack à %d\n",p.seq,p.acq);

                if(p.acq == a+1){
                    p.acq=p.seq+1;//ack = b+1
                    p.seq=a+1;
                    p.type=16;
                    p.id=ID++;//id++               
                    printf("ETAPE3 : J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);
                    //envoyer le dernier paquet en confirmant avoir recu l'ack
                    r=sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr * )&envoie,sizeof(struct sockaddr));
                    if(r==-1){
                        raler("send paquet");
                    }
                    printf("J'ai envoyé mon paquet final\n");
                }
                else{
                    printf("mauvais comportement du serveur en 3 way-shakehand\n");
                    exit(EXIT_FAILURE);
                }
                printf("connexion établie!\n");
                ID=0;              
                return 1 ;
        }else{
            continue;
        }
    }
    

}


/**
 * @brief Procede du 3 way handshake du cote destination(server) 
 * 0 si bonne fermeture, 1 sinon 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int etablissementConnexionServer (int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){
   
    int b = generateRandInt(100);
    int retour=0;

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

    struct packet p=init_packet() ;


    while(1){
         tv.tv_sec = 10;
         tv.tv_usec = 0;
        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            close(s);
            raler("select etablissement\n");
        }
        if(FD_ISSET(s,&fd_monitor)){
            printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

            if((retour=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            }

            printf("ETAPE1 : J'ai recu seq à %d et ack à %d\n",p.seq,p.acq);

            //preparation de donnée (ACK à envoyer)
            p.type+=16;
            p.acq=p.seq+1;
            p.seq=b;
            p.id=ID++; 
            printf("ETAPE2 :J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);

            //envoie d'ACK
            int sen=sendto(s,&p, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai envoyé l'ack \n");

            //Attendre la confirmation de reçu de l'ACK de la part de source 
            sen=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai recu la confirmation de l'ack \n");
            
            printf("ETAPE3: RECU seq = %d et l'ack =%d\n",p.seq,p.acq);
            if(p.acq==b+1){
                printf("connexion établie :)\n");
                return 0;
                ID=0;              

            }
            else{
                ID=0;              
                return 1;
            }
        }
        else {//rien sur le socket?
            printf("rien reçu .. \nDeuxième tentative en cours\nMerci de patienter\n");
            continue;
        }
    }
    ID=0;              
    return 1;
}





/**
 * @brief Fonction implementant la procedure stop n wait,cote server 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int stopNwaitServer (int s,struct sockaddr_in ecoute,
    struct sockaddr_in envoie){


    char *fn = "testColle.txt";
    int numAck=0,retour=0;
    struct packet p=init_packet();

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

    FILE *fp = fopen(fn, "w");
    
    if (fp == NULL)
    {
        raler("Fopen server");
    }

    while(1){
        tv.tv_sec = 4;
        tv.tv_usec = 0;
        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            if(close(s)==-1){
                raler("close");
            }            
            raler("select stop and wait server\n");
        }
        if(FD_ISSET(s,&fd_monitor)){
            printf("data ready\n");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

            if((retour=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            } 

                // fermeture connection avec FIN 
            if(p.type==2){
                printf("Jai recu mon message de fin \n");
                fclose(fp); 
                return fermeture_connection_serveur(s,ecoute,envoie);
            }

            //recuperer les données recues:
            printf("ETAPE1 : J'ai recu un paquet son seq est à %d\n",p.seq);
            printf("Données reçues : %s\n",p.data);
            //Afficher le msg reçu :
            if(p.seq==numAck){
                fprintf(fp,"%s",p.data);
                printf("Données reçues : %s\n",p.data);
                numAck=(numAck+1)%2;
                printf("je recoit un nouveau paquet et donc j'incremente ACK :%d \n",numAck);
            }
            

            p.acq=numAck;

            //envoie d'ACK
            int sen=sendto(s,&p, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }

            printf("Etape 2: J'ai envoyé l'ack %d \n",p.acq);
            continue;
        }
        printf("Rien n'est recu\nNouvlelle tentative en cours../..\n");
        continue;
    }      
    return 1;
}
