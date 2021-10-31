#include "packet.h"
#include <unistd.h>
int id = 0 ;

void stopNwait(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){
    
    struct packet p=init_packet() ;
    int altern = 0 ; 
    p.seq=altern ;
    p.id=id ; 
    p.fenetre=42;

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    FILE *fp=fopen("test.txt","r");

    if(fp==NULL){
        if(close(s)==-1){
            raler("close");
        }
        raler("fopen");
    }

    fgets(p.data,p.fenetre, fp );
    if( feof(fp) ) {
        fclose(fp); 
        //endConnexion ; 
        free(p.data);
        return ;
    }

    fseek(fp, p.fenetre, SEEK_CUR);
    printf("donnees lu sont %s \n",p.data);

    char * packetToSend = generatePacket(p);
    
    
    char * buffpacketToRecv = malloc(sizeof(char)*DEFAULTSIZE) ;
    memset(buffpacketToRecv,'\0',DEFAULTSIZE);
    
    
    int x=0;
    socklen_t size=sizeof(ecoute);
   

    while(1){

        x = sendto(s,packetToSend,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,
        sizeof(envoie)); 

        if(x==-1){
            if(close(s)==-1){
                raler("close");
            }
            raler("Sendto");
        }
        
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
                if(close(s)==-1){
                    raler("close");
                }
                raler("Sendto");
            }
            if(FD_ISSET(s,&fd_monitor)){  
                x=recvfrom(s,buffpacketToRecv,DEFAULTSIZE,0,
                (struct sockaddr*)&ecoute,&size);

                //test fin connexion->4 way handshake ;

                if(x==-1){
                    if(close(s)==-1){
                        raler("close");
                    }
                    raler("recv from \n");
                }
                p=generatePacketFromBuf(buffpacketToRecv);
                if(p.acq==altern){
                    altern =(altern+1)%2;
                    printf("Jai recu in je modif altern \n ");
                    p.seq=altern ;
                    id++;
                    p.id=id ;

                    
                    fgets(p.data,p.fenetre, fp );
                    if( feof(fp) ) {
                        fclose(fp); 
                        //endConnexion ; 
                        free(p.data);
                        return ;
                    }
                    fseek(fp, p.fenetre, SEEK_CUR);
                    printf("donnees lu sont %s \n",p.data);
                    break;
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
    inet_pton(AF_INET,"127.0.0.1",&(envoie.sin_addr));
    

    int x =etablissementConnexionSource(s,ecoute,envoie);
    if(x==1){
        stopNwait(s,ecoute,envoie);
    }
    printf("%d \n", x);

    return 0;
}