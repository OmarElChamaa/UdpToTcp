#include "packet.h"
#include <unistd.h>



int main(int argc, char * argv[]){

    if(argc!=5){
        raler("Mauvais nombre d'arguments \n");
    }
    char *mode=argv[1];
    char *ipDistante=argv[2];
    char *portLocal=argv[3];
    char *portEnvoie=argv[4];

    unsigned short portLocalInt=atoi(portLocal);//3333
    unsigned short portEcouteInt=atoi(portEnvoie); //4444

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;

    s = socket(AF_INET, SOCK_DGRAM, 0);

    ecoute.sin_family = AF_INET; 
    ecoute.sin_port = htons(portLocalInt); 
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    bind(s, (struct sockaddr *)&ecoute,sizeof(ecoute));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(portEcouteInt); 
    inet_pton(AF_INET,ipDistante,&(envoie.sin_addr));
    
    
    if(strcmp(mode,"0")==0){
        int x =etablissementConnexionSource(s,ecoute,envoie,'0');
        if(x==0){
            stopNwait(s,ecoute,envoie);
        }
    }else{
        int x =etablissementConnexionSource(s,ecoute,envoie,'1');
        if(x==0){
            go_back_N_source(s,ecoute,envoie);
        }
    }
    return 0;
}

