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
#include <sys/select.h>

int id =0;

int etablissementConnexion(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){

    int a = generateRandInt(5000);
    char buff [DEFAULTSIZE] ;

    struct packet p=init_packet() ;
    
    p.id=id++ ;
    p.type.SYN=1;
    p.seq=a;

    char * packetToSend = malloc(sizeof(char)*DEFAULTSIZE) ;
    memset(packetToSend,'\0',DEFAULTSIZE);
    generatePacket(p,packetToSend) ;


    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;


    while(1){
        send_to_establish(s,packetToSend,DEFAULTSIZE,0,(struct sockaddr*)&envoie,
        sizeof(envoie));

        retval = select(1, &fd_monitor, NULL, NULL, &tv);
            if(retval==-1){
                printf("select etablissement\n");
            }

        if(FD_ISSET(s,&fd_monitor)){
                printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

                //recevfrom
                socklen_t size=sizeof(envoie);
                int r =recvfrom(s,buff,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                //deroulement de test
                
                p=generatePacketFromBuf(buff);//on genere le paquet du buf recu
                if(p.type.ACK==a+1){
                    p.acq=p.seq+1;//ack = b+1
                    p.id++;//id++
                    //envoyer le dernier paquet en confirmant avoir recu l'ack
                    send_to_establish(s,packetToSend,DEFAULTSIZE,0,(struct sockaddr * )&envoie,
                    sizeof(envoie));
                   
                }
                else{
                    printf("mauvais comportement du serveur en 3 way-shakehand\n");
                    exit(EXIT_FAILURE);
                }
                printf("connexion établie!");
                id=0;
                exit(EXIT_SUCCESS);
        }else{
            continue;
        }
    }
}

// void goBack(int s,struct sockaddr_in envoie,struct sockaddr_in ecoute){

   

// }

void stopNwait(int s,struct sockaddr_in envoie,
struct sockaddr_in ecoute,struct packet p){
    
    //binding(s,ecoute);

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char * packetToSend = malloc(sizeof(char)*DEFAULTSIZE) ;
    memset(packetToSend,'\0',DEFAULTSIZE);
    generatePacket(p,packetToSend) ;
    

    char * buffpacketToRecv = malloc(sizeof(char)*DEFAULTSIZE) ;
    memset(buffpacketToRecv,'\0',DEFAULTSIZE);

    int altern = 0 ; 
    
    struct packet tmp = p ;

    while(1){
        retval=select(FD_SETSIZE+1,&fd_monitor,NULL,NULL,&tv);
        switch (retval)
        {
        case -1 :
            close(s);
            raler("select GO BACK \n");
            break;
        
        default:
            int x=0;
            x = sendto(s,packetToSend,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,
            sizeof(envoie));    
            if(x==-1){
                close(s);
                raler("Sendto");
            }
            if(FD_ISSET(s,&fd_monitor)){

                recvfrom(s,buffpacketToRecv,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,
                sizeof(ecoute));
                if(x==-1){
                    close(s);
                    printf("recv from \n");
                }
                p=generatePacketFromBuf(buffpacketToRecv);
                if(p.acq!=tmp.seq){
                    break;
                }
                
            }
            
        }
    }

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


    struct sockaddr_in ecoute ;
    struct sockaddr_in envoie ;

    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(portLocalInt);
    ecoute.sin_addr.s_addr = INADDR_ANY ;

    envoie.sin_family = AF_INET;
    envoie.sin_port = htons(portEcouteInt);
    envoie.sin_addr.s_addr = INADDR_ANY ;

    int s = 0;
    s=creationSocket(s);

    if(etablissementConnexion(s,ecoute,envoie)==0){
        printf("Connexion etablie avec Succes \n ");
        printf("/******************************/\n");
    }else{
        printf("Erreur Lors de L'etablissement de connexion\n");
        printf("/******************************/\n");
        return EXIT_FAILURE;
    }

    struct packet p=init_packet();

    if(modeInt==1){

        printf("Vous avez choisi le mode stop-wait \n");
        stopNwait(s,envoie,ecoute,p);  
      
    }else{

        printf("Vous avez choisi le mode go-back-in \n");
        goBack(s,envoie,ecoute);
    }
    return 0 ;
}

// #include "packet.h"

// int main(){
//     char buf[416] = "0000111100000001000000010000000100000001000000010000000100000001";
//     packet p = init_packet(p);
//     p=generatePacketFromBuf(buf);

//     printf("ID : %d\n",p.id);
//     printf("Type ACK : %d\n",p.type.ACK);
//     printf("Type FIN : %d\n",p.type.FIN);
//     printf("Type RST : %d\n",p.type.RST);
//     printf("Type SYN : %d\n",p.type.SYN);
//     printf("acq : %d\n",p.acq);
//     printf("ECN : %d\n",p.ecn);
//     printf("Fenetre : %d\n",p.fenetre);
//     printf("seq : %d\n",p.seq);
    
//     return 0;
// }