#include "packet.h"


int main(){

struct noeud* newNode = (struct noeud*) malloc(sizeof(struct noeud)) ; 
newNode->num=1;
struct noeud* tete = newNode;
for(int i=0; i<10;i++){
packet p = init_packet();
struct noeud* newNode = (struct noeud*) malloc(sizeof(struct noeud)) ; 
push(&newNode,p);

printf("node->p->seq: %d\n",newNode->p->seq);
printf("node->num: %d\n",newNode->num);
newNode=newNode->suivant;

}
int compteur =0;
newNode=tete;
while (1){
if(newNode->suivant!=NULL){
    newNode=newNode->suivant;
    compteur++;
}
else{
    printf("%d  est le compteur\n",compteur);
    exit(1);
}

}
    return 0;
}