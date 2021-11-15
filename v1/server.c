    #include "packet.h"



int main (){

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    
    s= creationSocket(s);

    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(6666);
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    
    bind(s,(struct sockaddr*)&ecoute,sizeof(struct sockaddr_in));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(5555); 
    inet_pton(AF_INET,"127.0.0.1",&(envoie.sin_addr));

    int x =etablissementConnexionServer(s,ecoute,envoie);
    
    if(x==1){//probleme de connexion
        printf("Pb de connexion retour 1\n")   ;
    }

    if(x==0){//mode stop and wait
        stopNwaitServer(s,ecoute,envoie);
    }
     if(x==2){//mode Go back N
        go_back_N_serevr(s,ecoute,envoie);
    }

    printf("%d \n", x);
return 0;

}
