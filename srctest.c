#include "packettest.h"




int main (int argc, char const *argv[]){

    int s=0;

    if (argc==1){
        
    }
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    s= creationSocket(s);

    int envoieIP=atoi(argv[1]);
    int ecouteIP=atoi(argv[2]);
    int ipDistInt=atoi(argv[3]);


    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(ecouteIP);
    ecoute.sin_addr.s_addr = "0.0.0.1" ;


    //inet_pton(AF_INET,argv[3] ,&envoie.sin_addr);

    envoie.sin_family = AF_INET;
    envoie.sin_port = htons(envoieIP);
    envoie.sin_addr.s_addr = "0.0.0.0" ;
    printf("On ecoute sur %d \n on envoie sur %d \n",ecouteIP,envoieIP);


    int x =etablissementConnexionSource(s,ecoute,envoie);
    printf("%d \n", x);
    return 0;
}