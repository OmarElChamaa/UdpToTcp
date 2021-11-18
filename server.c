    #include "packet.h"



int main (int argc, char * argv[]){

    if(argc!=4){
        raler("Mauvais nombre d'arguments \n");
    }
    char *ipDistante=argv[1];
    char *portLocal=argv[2];
    char *portEnvoie=argv[3];

    int portLocalInt=atoi(portLocal);//6666
    int portEcouteInt=atoi(portEnvoie); //5555

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    
    s= creationSocket(s);

    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(portLocalInt);
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    
    bind(s,(struct sockaddr*)&ecoute,sizeof(struct sockaddr_in));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(portEcouteInt); 
    inet_pton(AF_INET,ipDistante,&(envoie.sin_addr));//"127.0.0.1"

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
