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
typedef int SOCKET;



typedef struct Type
{
    int ACK ; 
    int RST ;
    int FIN; 
    int SYN; 
  
}Type;


typedef struct packet
{
    int id ; 
    struct Type type ; 
    int seq ;
    int acq; 
    int ecn ;
    int fenetre ; 
    void * data ; 
}packet;

/////////////////////////FCT raler/////////////////////////////////////////////////////////
void raler(char *message) 
{
	perror(message);
	exit(EXIT_FAILURE); //dans le cas d'un probleme -> retour 1 comme demandé
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////



//////////////////////FCT creation d'une socket/////////////////////////////////////////////
int creationSocket (int desc){
    if ((desc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        raler("socket");
    }
return desc;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////



////////////////////FCT generation d'un int aleatoir///////////////////////////////////////
int generateRandInt(int max){
    srand(time(NULL));
    int r = rand() % max;
    return r;
} 
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////






///////////////////////FCT bind////////////////////////////////////////////////////////////
void binding(int sock, struct sockaddr_in addr){
if ((sock=(bind(sock,(struct sockaddr *)&addr,sizeof(struct sockaddr_in)))==-1)){
    raler("bind");
}
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////








///////////////////////////////FCT intToFourChar//////////////////////////////////////////
const char  * intToFourChar(int x){

    char * str=NULL;
    sprintf(str,"%d",x);
    int numDigits = strlen(str);

    printf("str is %s and num digits is %d\n",str,numDigits);

    char v1[]="0";
    char v2[]="00";
    char v3[]="000";

    switch (numDigits)
    {
        case 0:
            return strcpy(str,"0000");
        case 1:
            return strcat(v3,str) ;
        case 2:
            return strcat(v2,str);
        case 3:
            return strcat(v1,str);
        default:
            printf("cas inconnu");
    }
    return NULL ;
    
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


///////////////////////FCT GeneratePacket////////////////////////////////////////////////////////////

//je dois encore concatener les donnees

const char * generatePacket(struct packet p){
    const char *id=intToFourChar(p.id);
    const char *type=intToFourChar(p.type);
    const char *seq=intToFourChar(p.seq);
    const char *acq=intToFourChar(p.acq);
    const char *ecn=intToFourChar(p.ecn);
    const char *fen=intToFourChar(p.fenetre);

    char * packetHeader ;
    packetHeader=malloc(sizeof(id)*6);

    strcat(packetHeader,id);
    strcat(packetHeader,type);
    strcat(packetHeader,seq);
    strcat(packetHeader,acq);
    strcat(packetHeader,ecn);
    strcat(packetHeader,fen);

    return packetHeader;
} 


///////////////////////////////////END OF FUNCTION//////////////////////////////////////////



///////////////////////FCT GenerateStructFromPacket////////////////////////////////////////////////////////////


struct packet * generatePacketFromBuf(char * buf){
    char* temp=buf;
    struct packet *p =malloc(sizeof(struct packet *));

    p->id=convert_premiers_char(temp,8);
    p->type.ACK=convert_premiers_char(temp,2);
    p->type.RST=convert_premiers_char(temp,2);
    p->type.FIN=convert_premiers_char(temp,2);
    p->type.SYN=convert_premiers_char(temp,2);
    p->seq=convert_premiers_char(temp,16);
    p->acq=convert_premiers_char(temp,16);
    p->ecn=convert_premiers_char(temp,8);
    p->fenetre=convert_premiers_char(temp,8);
    //p->data=NULL;


//0000000116040201000000000000000100000000000000010000000500000001

    return p;
} 



/////////////////////////////////FCT convert_premiers_char/////////////////////////////////
//Fonction qui sert à recuperer les 4 premier octets d'un string de entiers
//et renvoi le numéro 
//ex pour la cheine "4584200" il renvoie 4584
int convert_premiers_char (char * string,int size){
int i=0, resultat=0;
char * c= malloc(sizeof(char *));

while(string[i] != '\0'){
    c=strncpy(c, string, size);//extraire les 4 premier bits	
    i++;
}

for(i = 0 ; string[i] ; i++)//supprimer la partie extrait
    string[i] = string[i+size];

resultat= atoi(c);//partie entier de la partie extrait
  return resultat;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////




