    #include "packet.h"



int main (){

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    
    s= creationSocket(s);

    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(6666);
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
   //inet_pton(AF_INET,"127.0.0.1",&(ecoute.sin_addr));
    
    bind(s,(struct sockaddr*)&ecoute,sizeof(struct sockaddr_in));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(5555); 
    inet_pton(AF_INET,"127.0.0.1",&(envoie.sin_addr));

    // if(connect(s, (struct sockaddr *)&envoie, sizeof(struct sockaddr)) == -1 ) {
    //     close(s);
    //     fprintf(stderr, "Failed to connect to remote server!\n");
    //     exit(EXIT_FAILURE);
    // }
    

    //inet_pton(AF_INET,argv[1] ,&client.sin_addr);
    int x =etablissementConnexionServer(s,ecoute,envoie);

    printf("%d \n", x);
return 0;

}
