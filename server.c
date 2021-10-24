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
#include "packet.h"


int etablissementConnexion (int s,int ipDistante,int portLocal,int portEcoute,struct sockaddr_in client,socklen_t size,char *buf){
    int retour=0;
    struct packet * p = init_packet();
    SOCKET sock = creationSocket(sock);//creation de socket
    struct sockaddr_in me = prepaAddrLoc();//preparation de l'adresse et le port
    binding(sock,me);//lier le port
    int alea_b = generateRandInt(500);//nb aléatoire

    char buf[DEFAULTSIZE];
    memset(buf, '\0',DEFAULTSIZE);



    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;


//on peut ajouter une section de test temporaire pour tester si nos adresse et port sont justes
    while(1){

    retval = select(1, &fd_monitor, NULL, NULL, &tv);
    if(retval==-1){
        printf("select etablissement\n");
    }
    if(FD_ISSET(s,&fd_monitor)){
                printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

    if((retour=recvfrom(s,buf,DEFAULTSIZE,0,(struct sockaddr*)&client,&size))==-1){
        raler("recvfrom");
    }

    char*inter_buf = buf;
    //analyse de données reçu :
    

    //preparation de donnée (ACK à envoyer)


    //envoie d'ACK

    
    //Attendre la confirmation de reçu de l'ACK de la part de source 

    }
    //Connexion établie
    printf("connexion établie\n");


    }


}




//fct main
int main(int argc, char const *argv[]) {
    if(argc != 2)
    raler("USAGE ...");

   // senderUDP();
     SOCKET sock = 0;
     sock = creationSocket(sock);


    char buf[1024];// a modifer !
    const char * pointer;
    char *dst = malloc(100);

    struct sockaddr_in sin; //preparation de l'adresse locale
    struct sockaddr_in client;

    sin.sin_addr.s_addr=htonl(INADDR_ANY); //initialisation de l'adresse = IP de la machine locale
    sin.sin_family=AF_INET;  // IPV4
    sin.sin_port=htons(6666); //Port
    printf("%s\n",inet_ntop(AF_INET,&sin.sin_addr,dst ,100));

    memset(buf,'\0',1024);
    
    int verif;
    socklen_t size=sizeof(sin);
    if((verif= bind(sock, (struct sockaddr*)&sin, size))==-1){
        raler("bind");
    }



int ipDistante,portLocal,portEcoute;
    if ((etablissementConnexion(sock,ipDistante,portLocal,portEcoute,client,strlen(buf),buf))==-1){
        raler("connexion non établie");
        exit(-1);
    };

///Connexion établie :)

    close(sock);
    return 0;
}
