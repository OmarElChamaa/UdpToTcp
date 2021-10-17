#include <time.h>
#include <stdlib.h>


int generateRandInt(int max){
    srand(time(NULL));
    int r = rand() % max;
    return r;
} 




typedef struct packet
{
    int id ; 
    int type ; 
    int seq ;
    int acq; 
    int ecn ;
    int fenetre ; 
    void * data ; 
};


char * generatePacket(struct packet p){
    char *packet = p.id+p.type+p.seq+p.acq+p.ecn+p.fenetre+p.data+'0'
    return packet;
}
