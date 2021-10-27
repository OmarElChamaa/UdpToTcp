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

// int id =0;

// int etablissementConnexion(int s,int ipDistante,int portLocal,
// int portEcoute,struct sockaddr_in ecoute,struct sockaddr_in envoie)
// {

//     int a = generateRandInt(5000);
//     char buff [DEFAULTSIZE] ;

//     struct packet* p=init_packet() ;
    
//     p->id=id++ ;
//     p->type.SYN=1;
//     p->seq=a;

//     const char * packetToSend=generatePacket(*p);

//     fd_set fd_monitor;
//     struct timeval tv;
//     int retval;

//     FD_ZERO(&fd_monitor);
//     FD_SET(s, &fd_monitor);

//     tv.tv_sec = 5;
//     tv.tv_usec = 0;


//     while(1){
//         send_to_establish(s,packetToSend,DEFAULTSIZE,0,(struct sockaddr*)&envoie,
//         sizeof(portEcoute));

//         retval = select(1, &fd_monitor, NULL, NULL, &tv);
//             if(retval==-1){
//                 printf("select etablissement\n");
//             }

//         if(FD_ISSET(s,&fd_monitor)){
//                 printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

//                 //recevfrom
//                 socklen_t size=sizeof(envoie);
//                 int r =recvfrom(s,buff,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
//                 if(r==-1){
//                     raler("recvfrom 1\n");
//                 }
//                 //deroulement de test
                
//                 p=generatePacketFromBuf(buff);//on genere le paquet du buf recu
//                 if(p->type.ACK==a+1){
//                     p->acq=p->seq+1;//ack = b+1
//                     p->id++;//id++
//                     //envoyer le dernier paquet en confirmant avoir recu l'ack
//                     send_to_establish(s,packetToSend,DEFAULTSIZE,0,(struct sockaddr * )&envoie,
//                     sizeof(portEcoute));
                   
//                 }
//                 else{
//                     printf("mauvais comportement du serveur en 3 way-shakehand\n");
//                     exit(EXIT_FAILURE);
//                 }


//                 printf("connexion établie!");
//                 exit(EXIT_SUCCESS);
//         }else{
//             continue;
//         }
//     }
// }
// /*
// void goBack(int ipDistante,int portLocal,int portEcoute){

//     //int fenetre = ;

//     int s = 0; 
//     creationSocket(s);

//     struct sockaddr_in ecoute ;
//     struct sockaddr_in envoie ;

//     ecoute.sin_family = AF_INET ;
//     ecoute.sin_port = htons(portEcoute);
//     ecoute.sin_addr.s_addr=INADDR_ANY;

//     envoie.sin_family =AF_INET;
//     envoie.sin_port = htons(portLocal);
//     envoie.sin_addr.s_addr=INADDR_ANY;

//     binding(s,ecoute);


// }

// void stopNwait(int ipDistante,int portLocal,int portEcoute){

// }


// int main(int argc, char * argv[]){

//     if(argc!=5){
//         raler("Mauvais nombre d'arguments \n");
//     }
//     char *mode=argv[1];
//     char *ipDistante=argv[2];
//     char *portLocal=argv[3];
//     char *portEcoute=argv[4];

//     int ipDistInt=atoi(ipDistante);
//     int portLocalInt=atoi(portLocal);
//     int portEcouteInt=atoi(portEcoute);
//     int modeInt = atoi(mode); 

//     if(modeInt==1){

//         printf("Vous avez choisi le mode stop-wait \n");
//         stopNwait(ipDistInt,portLocalInt,portEcouteInt);  
      
//     }else{

//         printf("Vous avez choisi le mode go-back-in \n");
//         goBack(ipDistInt,portLocalInt,portEcouteInt);
//     }
//     return 0 ;
// }
// */
// int main(){
//     return 0;
// }


// #include "packet.h"

int main(){
    char buf[416] = "0000111100000001000000010000000100000001000000010000000100000001";
    packet * p = init_packet(*p);
    p=generatePacketFromBuf(buf);

    printf("ID : %d\n",p->id);
    printf("Type ACK : %d\n",p->type.ACK);
    printf("Type FIN : %d\n",p->type.FIN);
    printf("Type RST : %d\n",p->type.RST);
    printf("Type SYN : %d\n",p->type.SYN);
    printf("acq : %d\n",p->acq);
    printf("ECN : %d\n",p->ecn);
    printf("Fenetre : %d\n",p->fenetre);
    printf("seq : %d\n",p->seq);
    

    free(p);
    return 0;
}