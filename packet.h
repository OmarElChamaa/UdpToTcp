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
#define DEFAULTSIZE 416



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





/////////////////////FCT inititialisation d'un paquet//////////////////////////////////////
struct packet  init_packet(){
    //struct packet  p=malloc(sizeof(struct packet ));
    struct packet p ; 
    p.acq=0;
    p.ecn=0;
    p.fenetre=0;
    p.id=0;
    p.seq=0;
    p.type.ACK=0;
    p.type.FIN=0;
    p.type.RST=0;
    p.type.SYN=0;
    //il faut free(p) apres l'appelle a cette fct
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



//////////////////////FCT calculer la puissance/////////////////////////////////////////////
int puissance ( int a, int b ){//calculer a^b
    int resultat =1;
        for(int i=0;i<b;i++){
            resultat*=a;
        }
    return resultat;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


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


//////////////////////FCT concertire un nb decimal en binaire de format int////////////////////////////////
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

    //free(buf);
    free(str);
    return x ;
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



//////////////////////FCT preparation d'une addr locale avec port local/////////////////////
struct sockaddr_in prepaAddrLoc(){

    struct sockaddr_in sock ;
    sock.sin_family=AF_INET;
    sock.sin_addr.s_addr= htonl(INADDR_ANY);
    sock.sin_port=htons(3200);

    return sock;
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


/////////////////////FCT addPrefix////////////////////////////////////////////
//pour ajouter un 0 avant le nombre x/////////////////
const char * addZeroPrefix(int x){
    char * str=NULL;
    sprintf(str,"%d",x);
    //int numDigits = strlen(str);
    char v1[]="0";

    return strcat(v1,str);
}
//////////////////////////////////END OF FUNCTION//////////////////////////////////////////


////////////////////FCT intToChar////////////////////////////////////////////
char intToChar(int x){
    char str=x+'0';
    return str;
}
//////////////////////////////////END OF FUNCTION//////////////////////////////////////////




///////////////////////////////FCT intToEightBit//////////////////////////////////////////
const char  * intToEightBit(int x){

    char * str=malloc(sizeof(char *));
    sprintf(str,"%d",x);
    int numDigits = strlen(str);

    //printf("str is %s and num digits is %d\n",str,numDigits);

    char v1[]="00000";
    char v2[]="000000";
    char v3[]="0000000";
    char v4[]="0000";
    char v5[]="000";
    char v6[]="00";
    char v7[]="0";

    switch (numDigits)
    {
        case 0:
            return strcpy(str,"00000000");
        case 1:
            return strcat(v3,str) ;
        case 2:
            return strcat(v2,str);
        case 3:
            return strcat(v1,str);
        case 4:
            return strcat(v4,str);
        case 5:
            return strcat(v5,str);
        case 6:
            return strcat(v6,str);
        case 7:
            return strcat(v7,str);
        default:
            printf("cas inconnu");
    }
    return NULL ;
    
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////


/////////////////////////////////////FCT AJOUT N YERO //////////////////
char  * ajoutNZero(int x,int nbZero){

    char * str=malloc(sizeof(char *));
    sprintf(str,"%d",x);
    int numDigits = strlen(str);

    //printf("str is %s and num digits is %d\n",str,numDigits);

    int aAjouter = nbZero - numDigits; 
    if(aAjouter<0){
        //raler("ajoutNZero");
    } 

    char * v=malloc(sizeof(char *));

    for(int i = 0 ; i<aAjouter ; i++){
        strcat(v,"0");
    }
    return strcat(v,str);
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////



///////////////////////FCT GeneratePacket////////////////////////////////////////////////////////////

//je dois encore concatener les donnees


void generatePacket(struct packet p,char * packetHeader2){

    const char *id = ajoutNZero( dec_to_Intbin(p.id),8);
    
    printf("ID IS %s \n ",id);
    int x = p.type.ACK+p.type.RST+p.type.FIN+p.type.SYN ; 

    const char *type = intToEightBit(dec_to_Intbin(x));
    printf("type is %s \n ",type);

    char *seq = ajoutNZero(dec_to_Intbin(p.seq),16);

    printf("seq is %s \n ",seq);


    char *acq = ajoutNZero(dec_to_Intbin(p.acq),16);

    printf("acq is %s \n ",acq);
    const char *ecn = intToEightBit(dec_to_Intbin(p.ecn));

    printf("ecn is %s \n ",ecn);
    const char *fen = intToEightBit(dec_to_Intbin(p.fenetre));
    printf("fen is %s \n ",fen);

    //char * packetHeader2=malloc(sizeof(char)*416);
    //memset(packetHeader2,'\0',DEFAULTSIZE);
    //char packetHeader2[416];


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

    
    return ;
} 

///////////////////////////////////END OF FUNCTION//////////////////////////////////////////




/////////////////////////////////FCT convert_premiers_char/////////////////////////////////
//Fonction qui sert à recuperer les n premier octets d'un string de entiers
//et renvoi le numéro 
//ex pour la cheine "1111 1111" et n =4 il renvoie 15
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
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////






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
//Il faut faire un free(p) apres l'appelle a cette fct
    return p;
} 
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////





/////////////////////////////////FCT send_to_establish/////////////////////////////////
void send_to_establish (int fd, const void *buf, size_t size, int flags,
 const struct sockaddr *addr, socklen_t addr_len){
ssize_t n = sendto(fd,buf,size,flags,(struct sockaddr*)&addr,
       addr_len);
            if(n==-1){
                perror("sendto etabllissement \n");
                if(close(fd)==-1){
                    raler("close s");
                }
            }
return ;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////





