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



typedef struct Type
{
    int ACK :2; 
    int RST :2;
    int FIN :2; 
    int SYN :2; 
  
}Type;

typedef struct packet
{
    int id ; 
    struct Type type  ; 
    int seq : 16 ;
    int acq : 16 ; 
    int ecn : 8 ;
    int fenetre :8 ; 
    char * data; 
}packet;

/////////////////////FCT inititialisation d'un paquet//////////////////////////////////////
struct packet  init_packet(){
    //struct packet  p=malloc(sizeof(struct packet ));
    struct packet p ; 
    p.acq=2;
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
