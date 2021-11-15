    #include "packet.h"



int main (){

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    socklen_t address_len = sizeof(struct sockaddr_in);

    ecoute.sin_family = AF_INET; 
    ecoute.sin_port = htons(3333); 
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    bind(s, (struct sockaddr *)&ecoute,sizeof(ecoute));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(4444); 
    inet_pton(AF_INET,"127.0.0.1",&(envoie.sin_addr));
    int x =etablissementConnexionSource(s,ecoute,envoie);
    printf("%d \n", x);
    return 0;
}