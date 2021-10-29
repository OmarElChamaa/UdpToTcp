#include "packettest.h"




int main (int argc, char const *argv[]){

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    s= creationSocket(s);

    envoie.sin_family = AF_INET;
    envoie.sin_port = htons(5000);
    inet_pton(AF_INET,argv[1] ,&envoie.sin_addr);

    int x =etablissementConnexionSource(s,ecoute,envoie);
printf("%d \n", x);
return 0;
}