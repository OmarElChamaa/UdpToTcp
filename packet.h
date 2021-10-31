#include <time.h>
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

#define DEFAULTSIZE 416
int ID=0;


////////////////////FCT generation d'un int aleatoir///////////////////////////////////////
int generateRandInt(int max){
    srand(time(NULL));
    int r = rand() % max;
    return r;
} 
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


typedef struct Type
{
    int ACK ; 
    int RST ;
    int FIN ; 
    int SYN ; 
  
}Type;

typedef struct packet
{
    int id ; 
    struct Type type  ; 
    int seq ;
    int acq  ; 
    int ecn ;
    int fenetre  ; 
    char * data; 
}packet;

/////////////////////FCT inititialisation d'un paquet//////////////////////////////////////
struct packet  init_packet(){
    struct packet p ; 
    p.acq=0;
    p.ecn=0;
    p.fenetre=42;
    p.id=2;
    p.seq=0;
    p.type.ACK=0;
    p.type.FIN=0;
    p.type.RST=0;
    p.type.SYN=0;
    p.data=malloc(sizeof(char)*p.fenetre);
    memset(p.data, '\0',p.fenetre);
    return p;
}
//////////////////////////////////END OF FUNCTION//////////////////////////////////////////


/////////////////////////FCT raler/////////////////////////////////////////////////////////
void raler(char *message) 
{
	perror(message);
	exit(EXIT_FAILURE); //dans le cas d'un probleme -> retour 1 comme demandé
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


/////////////////////////////////////FCT AJOUT N YERO //////////////////
char  * ajoutNZero(int x,int nbZero){

    char  * str = malloc(sizeof(char)*nbZero);
    memset(str, '\0',nbZero);
    sprintf(str,"%d",x);
    int numDigits = strlen(str);
    int aAjouter = nbZero - numDigits; 
    if(aAjouter<0){
        raler("ajoutNZero");
    } 

    char * v = malloc(sizeof(char)*aAjouter);
    memset(v,'\0',aAjouter);

    for(int i = 0 ; i<aAjouter ; i++){
        strcat(v,"0");
    }
    strcat(v,str);
    free(str);
    return v;
}
///////////////////////////////////END OF FUNCTION///////////////////



/////////////////////FCT concertire un nb decimal en binaire de format int////////////////////////////////
int dec_to_Intbin(int dec){

    int tab[16], i; 
    char buf[416];
    char * str=malloc(sizeof(char *));
    memset(buf,'\0',416);   
    for(i=0; dec > 0; i++)  
    {  
        tab[i] = dec%2;  
        dec = dec/2;  
    }   
    for(i=i-1; i >= 0; i--)  
    {  
        sprintf(str,"%d",tab[i]);
        strcat(buf,str);
    } 

    int x = atoi(buf);

    free(str);
    return x ;
}
///////////////////////////////////END OF FUNCTION//////////////



char* generatePacket(struct packet p ){

    char * packetHeader2= malloc(sizeof(char)*64);
    memset(packetHeader2, '\0',64);


    char *id=ajoutNZero(dec_to_Intbin(p.id),8);
    
    int x = p.type.ACK+p.type.RST+p.type.FIN+p.type.SYN ; 
    char * type=ajoutNZero( dec_to_Intbin(x),8);
    char *seq=ajoutNZero(dec_to_Intbin(p.seq),16);
    char *acq=ajoutNZero(dec_to_Intbin(p.acq),16);
    char * ecn=ajoutNZero( dec_to_Intbin(p.ecn),8);
    char *fen=ajoutNZero(dec_to_Intbin(p.fenetre),8);
    

    strcat(packetHeader2,id);
    printf("PACKET HEADER IS   : %s , taille : %ld \n ID IS %s \n",packetHeader2,strlen(id),id);
    strcat(packetHeader2,type);
    printf("PACKET HEADER IS   : %s \n",packetHeader2);
    strcat(packetHeader2,seq);
    printf("PACKET HEADER IS   : %s \n",packetHeader2);
    strcat(packetHeader2,acq);
    printf("PACKET HEADER IS   : %s \n",packetHeader2);
    strcat(packetHeader2,ecn);
    printf("PACKET HEADER IS   : %s \n",packetHeader2);
    strcat(packetHeader2,fen);
    printf("PACKET HEADER IS   : %s \n",packetHeader2);
    strcat(packetHeader2,p.data);
    printf("PACKET HEADER IS   : %s \n",packetHeader2);

    free(fen);
    free(seq);
    free(id);
    free(ecn);
    free(acq);
    free(type);

   
    return packetHeader2;
} 

///////////////////////////////////END OF FUNCTION//////////////////////////////////////////





//////////////////////FCT concertire un nb decimal en binaire////////////////////////////////
void dec_to_bin(int dec, char * buf){

    int tab[16], i; 
    char c;   
    for(i=0; dec > 0; i++)  
    {  
        tab[i] = dec%2;  
        dec = dec/2;  
    }   
    for(i=i-1; i >= 0; i--)  
    {  
        c= tab[i] + '0';
        strcat(buf,&c);
    } 
    return ;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////
//////////////////////FCT calculer la puissance/////////////////////////////////////////////
int puissance ( int a, int b ){//calculer a^b
    int resultat =1;
        for(int i=0;i<b;i++){
            resultat*=a;
        }
    return resultat;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


//////////////////////FCT creation d'une socket/////////////////////////////////////////////
int creationSocket (int desc){
    if ((desc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        raler("socket");
    }
return desc;
}
///////////////////////////////////END 

//////////////////////FCT binaire en decimal/////////////////////////////////////////////
int bin_to_dec ( char * bin ){//convertir un nombre binaire en décimal
    int i,size;
    size=strlen(bin);
    int resultat=0;
    int puiss=0;
    for(i=size-1;i>=0;i--){
        if(bin[i]=='1'){
            puiss=size-i-1;
        resultat=resultat + puissance(2,puiss);
        }
    }
    return resultat;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////



//////////////////////FCTconvert les n premiers chars/////////////////////////////////////////////
int convert_premiers_char (char * string,int size){
    int i=0, resultat=0;
    char * c= malloc(sizeof(char *));

    while(string[i] != '\0'){
        c=strncpy(c, string, size);//extraire les 4 premier bits	
        i++;
    }

    for(i = 0 ; string[i] ; i++)//supprimer la partie extrait
        string[i] = string[i+size];

    resultat= bin_to_dec(c);//partie entier de la partie extrait
    free(c);
    return resultat;
}
///////////////////////////////////////////////////////////////////////


///////////////////////FCT GenerateStructFromPacket////////////////////////////////////////////////////////////
struct packet  generatePacketFromBuf(char * buf){
    char* temp=buf;
    struct packet p=init_packet() ; // =malloc(sizeof(struct packet ));

    p.id=convert_premiers_char(temp,8);
    int tmp=convert_premiers_char(temp,8);
    //tester la presence de ACK
    if(tmp-16>=0){
        p.type.ACK=16;
        tmp-=16;
    }
    //tester la presence de RST
    if(tmp-4>=0){
        p.type.RST=4;
        tmp-=4;
    }
    if(tmp-2>=0){
        p.type.FIN=2;
        tmp-=2;
    }
    if(tmp-1>=0){
    p.type.SYN=1;
    tmp-=1;
    }
    p.seq=convert_premiers_char(temp,16);
    p.acq=convert_premiers_char(temp,16);
    p.ecn=convert_premiers_char(temp,8);
    p.fenetre=convert_premiers_char(temp,8);
    //p->data=NULL;
    return p;
} 
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


//////////////////////////////FCT ETABLISSEMENT DE CONNEXION COTÉ SOURCE /////////////////


int etablissementConnexionSource(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){

    int a = generateRandInt(100);
    char buff [DEFAULTSIZE] ;

    struct packet p=init_packet() ;
    socklen_t size=sizeof(envoie);

    // p.id=id++ ;
    // p.type.SYN=1;
    // p.acq = 16 ; 
    // p.seq=a;
    p.id=ID++ ;
    p.type.SYN=1;
    p.seq=a;
    printf("ETAPE1 : J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);

    char * packetToSend =generatePacket(p) ;

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

   

    while(1){
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        sendto(s,packetToSend,DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
            if(retval==-1){
                printf("select etablissement\n");
            }

        if(FD_ISSET(s,&fd_monitor)){
                printf("data ready\n");//Je receive et je test et si tout va bien je renvois avec les nouvelles valeurs
                //recevfrom
                int r =recvfrom(s,buff,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                //deroulement de test
                p=generatePacketFromBuf(buff);//on genere le paquet du buf recu
                printf("ETAPE2 : J'ai recu seq à %d et ack à %d\n",p.seq,p.acq);

                if(p.acq==a+1){
                    p.acq=p.seq+1;//ack = b+1
                    p.seq=a+1;
                    p.type.SYN=0;
                    p.id=ID++;//id++               
                    printf("ETAPE3 : J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);
                    //envoyer le dernier paquet en confirmant avoir recu l'ack
                    char * packetToSend2 =generatePacket(p) ;
                    r=sendto(s,packetToSend2,DEFAULTSIZE,0,(struct sockaddr * )&envoie,size);
                    if(r==-1){
                        raler("send paquet");
                    }
                    printf("J'ai envoyé mon paquet final\n");
                    free(packetToSend2);
                }
                else{
                    printf("mauvais comportement du serveur en 3 way-shakehand\n");
                    exit(EXIT_FAILURE);
                }
                printf("connexion établie!\n");
                ID=0;
                exit(EXIT_SUCCESS);
        }else{
            continue;
        }
    }
    free(packetToSend);

}
//////////////////////////////////END FUNCTION ///////////////////////////////////////////

//////////////////////////////////FCT ETABLISSEMENT DE CONNEXION COTÉ SERVEUR ////////////////
int etablissementConnexionServer (int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){
   
    int b = generateRandInt(100);
    int retour=0;

    char buf[DEFAULTSIZE];
    memset(buf, '\0',DEFAULTSIZE);

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

  

    while(1){
         tv.tv_sec = 10;
         tv.tv_usec = 0;
        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            close(s);
            raler("select etablissement\n");
        }
        if(FD_ISSET(s,&fd_monitor)){
            printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

            if((retour=recvfrom(s,buf,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            }

            char * inter_buf = buf;
            //analyse de données reçu :
            struct packet p=generatePacketFromBuf(inter_buf);
            printf("ETAPE1 : J'ai recu seq à %d et ack à %d\n",p.seq,p.acq);

            //preparation de donnée (ACK à envoyer)
            p.type.ACK=16;
            p.acq=p.seq+1;
            p.seq=b;
            p.type.SYN=1;
            p.id=ID++; 
            printf("ETAPE2 :J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);

            char *inter2_buf= generatePacket(p);

            //envoie d'ACK
            int sen=sendto(s,inter2_buf, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai envoyé l'ack \n");
            free(inter2_buf);
            //Attendre la confirmation de reçu de l'ACK de la part de source 
            char inter3_buf[DEFAULTSIZE];
            memset(inter3_buf, '\0',DEFAULTSIZE);
            sen=recvfrom(s,inter3_buf,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai recu la confirmation de l'ack \n");
            
            p=generatePacketFromBuf(inter3_buf);

            printf("ETAPE3: RECU seq = %d et l'ack =%d\n",p.seq,p.acq);
            if(p.acq==b+1){
                printf("connexion établie :)\n");
                return 1;
            }
            else{
                return-1;
            }
        }
        else {//rien sur le socket?
            printf("rien reçu .. \nDeuxième tentative en cours\nMerci de patienter\n");
            continue;
        }
    }
    return -1;
}
//////////////////////////////////END FUNCTION ///////////////////////////////////////////





//////////////////////////////////FCT STOP AND WAIT COTÉ SERVEUR /////////////////////////

    void stopNwaitServer (int s,struct sockaddr_in ecoute,
    struct sockaddr_in envoie){

        int numAck=0,retour=0;

        char*buf=malloc(sizeof(char)*DEFAULTSIZE);
        memset(buf,'\0',DEFAULTSIZE);

        fd_set fd_monitor;
        struct timeval tv;
        int retval;

        FD_ZERO(&fd_monitor);
        FD_SET(s, &fd_monitor);
        socklen_t size=sizeof(ecoute);

  
        while(1){
            tv.tv_sec = 4;
            tv.tv_usec = 0;
            retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
            if(retval==-1){
                if(close(s)==-1){
                raler("close");
            }            
            raler("select etablissement\n");
            }
            if(FD_ISSET(s,&fd_monitor)){
            printf("data ready");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

            if((retour=recvfrom(s,buf,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            }
            char * inter_buf = buf;
            //recuperer les données recues:
            struct packet p=generatePacketFromBuf(inter_buf);
            free(inter_buf);
            printf("ETAPE1 : J'ai recu un paquet son seq est à %d\n",p.seq);
            //Afficher le msg reçu :
            if(p.seq==numAck){
            printf("Données reçues : %s\n",p.data);
           // numSeq++%2;
            numAck=(numAck+1)%2;
            printf("je recoit un nouveau paquet et donc j'incremente ACK :%d \n",numAck);
            }
            struct packet p2= init_packet();
            p2.acq=numAck;
            char *inter2_buf= generatePacket(p2);
            //envoie d'ACK
            int sen=sendto(s,inter2_buf, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("Etape 2: J'ai envoyé l'ack \n");
            free(inter2_buf);
            continue;
        }
        printf("Rien n'est recu\nNouvlelle tentative en cours../..\n");
        continue;
        }

        free(buf);
        return;
    }
    //////////////////////////////////END FUNCTION ///////////////////////////////////////////
