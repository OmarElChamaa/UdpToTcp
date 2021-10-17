#include "packet.h"
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
#include <sys/select.h>

#define DEFAULTSIZE 52


int etablissementConnexion(int s,int ipDistante,int portLocal,
int portEcoute,struct sockaddr_in ecoute,struct sockaddr_in envoie)
{

    int a = generateRandInt(5000);
    char buff [DEFAULTSIZE] ;

    struct packet p ;
    p.id=a ;

    const char * packetToSend=generatePacket(p);

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;


    while(1){
        ssize_t n = sendto(s,packetToSend,8,0,(struct sockaddr * )&portLocal,
        sizeof(portLocal));
        if(n==-1){
            raler("Premier sendto etablisssement \n");
        }

        retval = select(1, &fd_monitor, NULL, NULL, &tv);
            if(retval==-1){
                printf("select etablissement\n");
            }

        if(FD_ISSET(s,&fd_monitor)){
                printf("data ready");//Je receve et je test et si tou va bien je renvois avec les nouvelles valeurs

                //recevfrom
                socklen_t size=sizeof(envoie);
                int r =recvfrom(s,buff,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                //deroulement de test
                
                int b=convert_premiers_char(buff,8);
                int a1=convert_premiers_char(buff,8);
                if(a1==a+1){
                    a1=a1+1;
                    p.id=a1;
                    //!!!!! que mettre pour b 
                    n = sendto(s,packetToSend,8,0,(struct sockaddr * )&portLocal,
                    sizeof(portLocal));
                    if(n==-1){
                        raler("second sendto etablisssement \n");
                    }
                }

                //send_to_establish

                printf("connexion établie!");
                exit(EXIT_SUCCESS);
        }else{
            continue;
        }
    }
}

void goBack(int ipDistante,int portLocal,int portEcoute){

    //int fenetre = ;

    int s = 0; 
    creationSocket(s);

    struct sockaddr_in ecoute ;
    struct sockaddr_in envoie ;

    ecoute.sin_family = AF_INET ;
    ecoute.sin_port = htons(portEcoute);
    ecoute.sin_addr.s_addr=INADDR_ANY;

    envoie.sin_family =AF_INET;
    envoie.sin_port = htons(portLocal);
    envoie.sin_addr.s_addr=INADDR_ANY;

    binding(s,ecoute);


}

void stopNwait(int ipDistante,int portLocal,int portEcoute){

}

int main(int argc, char * argv[]){

    if(argc!=5){
        raler("Mauvais nombre d'arguments \n");
    }
    char *mode=argv[1];
    char *ipDistante=argv[2];
    char *portLocal=argv[3];
    char *portEcoute=argv[4];

    int ipDistInt=atoi(ipDistante);
    int portLocalInt=atoi(portLocal);
    int portEcouteInt=atoi(portEcoute);
    int modeInt = atoi(mode); 

    if(modeInt==1){

        printf("Vous avez choisi le mode stop-wait \n");
        stopNwait(ipDistInt,portLocalInt,portEcouteInt);  
      
    }else{

        printf("Vous avez choisi le mode go-back-in \n");
        goBack(ipDistInt,portLocalInt,portEcouteInt);
    }
    return 0 ;
}
