#include <time.h>
#include <stdlib.h>
typedef int SOCKET;


 struct packet
{
    int id ; 
    int type ; 
    int seq ;
    int acq; 
    int ecn ;
    int fenetre ; 
    void * data ; 
};


//////////////////////FCT creation d'une socket/////////////////////////////////////////////
int creationSocket (int desc){
    if ((desc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        raler("socket");
    }
return desc;
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////







/////////////////////////FCT raler/////////////////////////////////////////////////////////
void raler(char *message) 
{
	perror(message);
	exit(EXIT_FAILURE); //dans le cas d'un probleme -> retour 1 comme demandé
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

    char str[4];
    sprintf(str,"%d",x);
    int numDigits = strlen(str);

    printf("str is %s and num digits is %d\n",str,numDigits);

    char v1[]="0";
    char v2[]="00";
    char v3[]="000";

    switch (numDigits)
    {
    case 0:
        strcpy(str,"0000");
        printf("x is %s \n",str);
        break;
    case 1:
        strcat(v3,str);
        printf("x is %s \n",v3);
        return v3 ;
        break ; 
    case 2:
        strcat(v2,str);
        printf("x is %s \n",v2);
        return v2 ;
        break ;
    case 3:
        strcat(v1,str);
        printf("x is %s \n",v1);
        return v1 ;
        break ;
    
    default:
        break;
    }
    
    return str ; 
}
///////////////////////////////////END OF FUNCTION//////////////////////////////////////////





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




