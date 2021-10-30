#include "packet.h"

int main(){

    struct packet p = init_packet();
    
    printf("ID : %X\n",p.id);
    printf("Type ACK : %u\n",p.type.ACK);
    printf("Type FIN : %u\n",p.type.FIN);
    printf("Type RST : %u\n",p.type.RST);
    printf("Type SYN : %u\n",p.type.SYN);
    printf("acq : %u\n",p.acq);
    printf("ECN :%u\n",p.ecn);
    printf("Fenetre : %u\n",p.fenetre);
    printf("seq : %u\n",p.seq);


    unsigned int size = sizeof(unsigned int);

    for(int i = 0;i<8;++i){
    // print last bit and shift left.
    printf("%u ",p.acq);
        p.acq = p.acq<<1;
    }


    return 0 ;
}