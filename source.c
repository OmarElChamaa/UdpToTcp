#include "packet.h"


#define DEFAULTSIZE 52

void raler(char *message){
	perror(message);
	exit(EXIT_FAILURE);
} 



int etablissementConnexion(int s,int ipDistante,int portLocal,int portEcoute,struct sockaddr_in ecoute,struct sockaddr_in envoie)
{

    int a = generateRandInt(5000);
    char buff [4] = a +'0';
    
    ssize_t n = sendto(s,buff,4,0,(struct sockaddr*)&portLocal,
            sizeof(portLocal));
    if(n==-1){
        perror("sendto etabllissement \n");
        if(close(s)==-1){
            raler("close s");
        }
    }


}

void goBack(int ipDistante,int portLocal,int portEcoute){

    //int fenetre = ;


    int s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (s==0) {
        perror("socket");
        return EXIT_FAILURE ;
    }

    struct sockaddr_in ecoute ;
    struct sockaddr_in envoie ;

    ecoute.sin_family = AF_INET ;
    ecoute.sin_port = htons(portEcoute);
    ecoute.sin_addr.s_addr=INADDR_ANY;

    envoie.sin_family =AF_INET;
    envoie.sin_port = htons(portLocal);
    envoie.sin_addr.s_addr=INADDR_ANY;

    if(bind(s,(const struct sockaddr *)&ecoute,
    sizeof( struct sockaddr_in))==-1){
        raler("Bind go back \n");
    }


}

void stopNwait(int ipDistante,int portLocal,int portEcoute){

}

int main(int argc, char * argv[]){

    if(argc!=5){
        raler("Mauvais nombre d'arguments \n");
    }
    char *mode[]=argv[1];
    char *ipDistante[]=argv[2];
    char *portLocal[]=argv[3];
    char *portEcoute[]=argv[4];

    int ipDistInt=atoi(ipDistante);
    int portLocalInt=atoi(portLocal);
    int portEcouteInt=atoi(portEcoute);
    int modeInt = atoi(mode); 

    if(modeInt==1){

        printf("Vous avez choisi le mode stop-wait \n");
        stopNwait(ipDistInt,portLocalInt,portEcouteInt);  
      
    }else{

        printf("Vous avez choisi le mode go-back-in \n");
        goBack(ipDistInt,portLocalInt,portEcouteInt);
    }
    return 0 ;
}
