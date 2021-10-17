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


#define DEFAULTSIZE 52


int etablissementConnexion(int s,int ipDistante,int portLocal,int portEcoute,struct sockaddr_in ecoute,struct sockaddr_in envoie)
{

    int a = generateRandInt(5000);
    
    struct packet p ;
    p.id=a ;
    p.type=0;
    
    const char * packetToSend=generatePacket(p);
 
    ssize_t n = sendto(s,packetToSend,8,0,(struct sockaddr*)&portLocal,
            sizeof(portLocal));
    if(n==-1){
        perror("sendto etabllissement \n");
        if(close(s)==-1){
            raler("close s");
        }
    }

    fd_set fd_monitor;;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(0, &fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    
    while(1){
        retval = select(1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            printf("select etablissement\n");
        }
        if(FD_ISSET(s,&fd_monitor)){
                //revfrom
                printf("data ready");
        }else{
            n = sendto(s,packetToSend,8,0,(struct sockaddr*)&portLocal,
                sizeof(portLocal));
            if(n==-1){
                perror("sendto etabllissement \n");
                if(close(s)==-1){
                    raler("close s");
                }
            }
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
