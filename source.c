#include "packet.h"


void stopNwait(int s,struct sockaddr_in envoie,
struct sockaddr_in ecoute,struct packet p){
    
    

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    FILE *fp=fopen("test.txt","r");
    fgets(p.data,p.fenetre, fp );
    fseek(fp, p.fenetre, SEEK_CUR);
    printf("donnees lu sont %s \n",p.data);
    

    char * packetToSend = generatePacket(p);
    

    char * buffpacketToRecv = malloc(sizeof(char)*DEFAULTSIZE) ;
    memset(buffpacketToRecv,'\0',DEFAULTSIZE);

    int altern = 0 ; 
    
    struct packet tmp = p ;
    int x=0;
    socklen_t size=sizeof(ecoute);
    
    

    while(1){
        retval=select(FD_SETSIZE+1,&fd_monitor,NULL,NULL,&tv);

        switch (retval)
        {
        case -1 :
            if(close(s)==-1){
                raler("close");
            }
            raler("select GO BACK \n");
            break;
        default :

            x = sendto(s,packetToSend,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,
            sizeof(envoie)); 

            if(x==-1){
                close(s);
                raler("Sendto");
            }

            if(FD_ISSET(s,&fd_monitor)){  
                x=recvfrom(s,buffpacketToRecv,DEFAULTSIZE,0,
                (struct sockaddr*)&ecoute,&size);

                if(x==-1){
                    close(s);
                    printf("recv from \n");
                }
                p=generatePacketFromBuf(buffpacketToRecv);
                if(p.seq!=tmp.seq || p.acq!= 0){
                    // retransmettre le message
                    break;
                }else{
                    // on passe au prochain a transmettre 
                }
                
            }
            
        }
    }

}









int main (){

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;

    s = socket(AF_INET, SOCK_DGRAM, 0);

    ecoute.sin_family = AF_INET; 
    ecoute.sin_port = htons(3333); 
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    bind(s, (struct sockaddr *)&ecoute,sizeof(ecoute));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(4444); 
    //envoie.sin_addr.s_addr = htonl(INADDR_ANY) ;
    inet_pton(AF_INET,"127.0.0.1",&(envoie.sin_addr));

    //Make connection to server socket so we can use send() and recv() to read and write the server
    // if(connect(s, (struct sockaddr *) &envoie, sizeof(struct sockaddr)) == -1 ) {
    //     close(s);
    //     fprintf(stderr, "Failed to connect to remote server!\n");
    //     exit(EXIT_FAILURE);
    // }
    struct packet p = init_packet();


    FILE *fp=fopen("test.txt","r");
    fgets(p.data,p.fenetre, fp );
    fseek(fp, p.fenetre, SEEK_CUR);
    printf("donnees lu sont %s \n",p.data);
    

    char * packetToSend = generatePacket(p);
    
    int x =etablissementConnexionSource(s,ecoute,envoie);
    printf("%d \n", x);

    
    
    return 0;
}