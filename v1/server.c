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
#include "packet.h"


int etablissementConnexionServer (int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){
   
    int a = generateRandInt(5000);
    char buff [DEFAULTSIZE] ;

    int retour=0;
    
    

    binding(s,ecoute);//lier le port


    char buf[DEFAULTSIZE];
    memset(buf, '\0',DEFAULTSIZE);



    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;


//on peut ajouter une section de test temporaire pour tester si nos adresse et port sont justes
    while(1){

        retval = select(1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            close(s);
            raler("select etablissement\n");
        }
        if(FD_ISSET(s,&fd_monitor)){
            printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

            if((retour=recvfrom(s,buf,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,
            sizeof(ecoute)))==-1){
                raler("recvfrom");
            }

            char * inter_buf = buf;
            //analyse de données reçu :
            struct packet p=generatePacketFromBuf(inter_buf);
            //preparation de donnée (ACK à envoyer)

            p.type.ACK=p.type.SYN+1;
            p.type.SYN=a;
            //p.id ++; 
            char * inter2_buf = malloc(sizeof(char)*DEFAULTSIZE) ;
            memset(inter2_buf,'\0',DEFAULTSIZE);
            generatePacket(p,inter2_buf);
            //envoie d'ACK
            //socklen_t socklen = sizeof(struct sockaddr_in); Cette ligne sert a quoi ?
            send_to_establish(s,inter2_buf, DEFAULTSIZE,0,
            (struct sockaddr*)&envoie,sizeof(envoie));

            //Attendre la confirmation de reçu de l'ACK de la part de source 
            char inter3_buf[DEFAULTSIZE];
            memset(inter3_buf, '\0',DEFAULTSIZE);

            int r =recvfrom(s,inter3_buf,DEFAULTSIZE+1,0,(struct sockaddr*)&client,&size);
            if(r==-1){
                raler("recvfrom 1\n");
            }
            p=generatePacketFromBuf(inter3_buf);
            if(p.acq==p.seq+1){
                printf("connexion établie :)\n");
                return 1;
            }
            else{
                return-1;
            }

        }

        else {//rien sur le socket?
            printf("rien reçu .. \nDeuxième tentative en cours\nMerci de patienter\n");
            continue;

        }
    }

    return -1;
}




//fct main
int main(int argc, char const *argv[]) {
    if(argc != 4)
    raler("USAGE ...");

   // senderUDP();
    int s = 0;
    s=creationSocket(s);


    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;

    char *ipDistante=argv[1];
    char *portLocal=argv[2];
    char *portEcoute=argv[3];

    int ipDistInt=atoi(ipDistante);
    int portLocalInt=atoi(portLocal);
    int portEcouteInt=atoi(portEcoute);

    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(portLocalInt);
    ecoute.sin_addr.s_addr = INADDR_ANY ;

    envoie.sin_family = AF_INET;
    envoie.sin_port = htons(portEcouteInt);
    envoie.sin_addr.s_addr = INADDR_ANY ;
    
    int verif;
    if((verif= bind(sock, (struct sockaddr*)&ecoute, 
    sizeof(ecoute)))==-1){
        close(s);
        raler("bind");
    }
    

    if ((etablissementConnexionServer(s,ecoute,envoie))==-1){
        close(s);
        raler("connexion non établie");
    };

///Connexion établie :)

    close(s);
    return 0;
}
