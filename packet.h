#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
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

#define DEFAULTSIZE 42
#define ACQ 16
#define TAILLEFEN 42
#define N 100


char ID=0;
//variqbles quon utilise pour les Figures au rapport 
double messagesEnvoyes = 0 ; //hors etablissement  
double messagesPerdus = 0 ; //hors etablissement 



/**
 * @brief genere un int aleatoire entre 0 et max 
 * 
 * @param max 
 * @return int 
 */
int generateRandInt(int max){
    srand(time(NULL));
    int r = rand() % max;
    return r;
} 

/**
 * @brief Fonction servant a ecrire les donnees respective pour les tracer 
 * 
 * @param gnuplot 
 * @param envoie 
 * @param perdu 
 * @param temps 
 */
void dessinerFigure(FILE *gnuplot,double envoie, double perdu,double temps){
    fprintf(gnuplot, "plot '-' \n");
    fprintf(gnuplot,"%f %lf\n", temps, envoie);


    fprintf(gnuplot, "plot '-' with points pt \"#\"\n");
    fprintf(gnuplot,"%f %lf\n", temps,  perdu);
    
}

/**
 * @brief Pour setup la legende de la figure stop n wait 
 * 
 * @param gnuplot 
 */
void setupPlotStop(FILE *gnuplot){
    fprintf(gnuplot, "set terminal png size 600,600\nset output'figStopNwait.png'\n");
    fprintf(gnuplot, "set xlabel \"temps en s\"\nset ylabel \"nbr messages\"\n");
    fprintf(gnuplot, "set title \"message perdu et envoye par rapport au temps\"\n");
}

/**
 * @brief structure du message ou packet 
 * @struct packet 
 */
typedef struct packet
{
    char id ; 
    char type ;
    short seq  ;
    short acq  ; 
    char ecn ;
    char fenetre  ;
    char data[42] ;  
}packet;

/**
 * @brief initiale les champs du packet 
 * 
 * @return struct packet 
 */
struct packet  init_packet(){
    struct packet p ; 
    p.id = 0 ;
    p.type = 0 ;
    p.seq =0 ;
    p.ecn = 0 ;
    p.acq=0;
    p.fenetre=52;
    memset(p.data,'\0',42);
    return p;
}

typedef struct noeud {
    int num; 
    struct packet* p;
    struct noeud* suivant;
}noeud;



/**
 * @brief Renvoie la cause de l'erreur + exit a 1 
 * 
 * @param message 
 */
void raler(char *message) 
{
	perror(message);
	exit(EXIT_FAILURE); 
}


/**
 * @brief Calcul pa puissance de a par b 
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int puissance ( int a, int b ){
    int resultat =1;
        for(int i=0;i<b;i++){
            resultat*=a;
        }
    return resultat;
}

/**
 * @brief Creer un socket et verifier si erreur, exit si erreur 
 * 
 * @param desc 
 * @return int 
 */
int creationSocket (int desc){
    if ((desc=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1){
        raler("socket");
    }
return desc;
}



void push(struct noeud** courant, struct packet p)
{
    struct noeud* nouveau_noeud = (struct noeud*) malloc(sizeof(struct noeud));
    nouveau_noeud->p  = &p;
    nouveau_noeud->suivant = (*courant);
    (*courant)    = nouveau_noeud;
}


void deleteNode(struct noeud** courant)
{
    if(courant!=NULL){
        free(courant);
    } 
}

void printList(struct noeud* node)
{
    while (node != NULL) {
        printf(" %d ", node->num);
        node = node->suivant;
    }
}


struct noeud* chercheList(struct noeud* node,int n)
{
    while (node != NULL) {
        if(node->num==n){
            return node;
        }
        node = node->suivant;
    }
    printf("Pas dans la liste \n ");
    return node ;  
}




/**
 * @brief Procede du 4 way handshake du cote serveur 
 * 0 si bonne fermeture, 1 sinon
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int fermeture_connection_serveur(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie)
{
    socklen_t size=sizeof(ecoute);
    struct packet p = init_packet();
    p.type = 16 ; 

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    while(1){
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
        if(sen==-1){
            raler("send etablisemment \n ");
        }

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
            if(retval==-1){
                printf("select fermeture\n");
            }

        if(FD_ISSET(s,&fd_monitor)){
            p.type = 2 ;
            int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
            if(sen==-1){
                raler("send etablisemment \n ");
            }

            if(FD_ISSET(s,&fd_monitor)){
                int r =recvfrom(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                if(p.type == 16){
                    printf("ACQ bien recu \n");
                    printf("---\t Connexion fermée\t--- \n");
                    if(close(s)==-1){
                        raler("close");
                    }
                    exit(0);  
                }
            }
        }
    }

}








/**
 * @brief Procede du 4 way handshake du cote source(client) 
 * 0 si bonne fermeture, 1 sinon 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int fermeture_connection_source(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie)
{
    socklen_t size=sizeof(ecoute);
    struct packet p = init_packet();
    p.type=2; 

    
//printf("je commence la fermeture\n");

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    while(1){
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        
        int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
        if(sen==-1){
            raler("send etablisemment \n ");
        }

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            printf("select etablissement\n");
        }

        if(FD_ISSET(s,&fd_monitor)){
            int r =recvfrom(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,&size);
            if(r==-1){
                raler("recvfrom 1\n");
            }
            if(p.type == 16){
                printf("Premier acquittement recu \n");
            }
            if(p.type == 2){

                printf("jai recu mon message de fin, jenvoie mon acq \n");
                p.type = 16 ; 
                sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
                if(sen==-1){
                    raler("send etablisemment \n ");
                }
                printf("---\t Connexion fermée\t--- \n");
                if(close(s)==-1){
                    raler("close");
                }  
                exit(0);
            }
            
        }else{
            continue;
        }
    }
    exit(1);
} 



/**
 * @brief Procede du 3 way handshake du cote source(client) 
 * 0 si bonne ouverture, 1 sinon .envoie le mode au serveur 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @param mode
 * @return int 
 */

int etablissementConnexionSource(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie,char mode){

    unsigned short a = generateRandInt(100);
    struct packet p=init_packet() ;
    socklen_t size=sizeof(ecoute);

    p.id=0;
    p.seq=a;
    p.type+=1;
    printf("ETAPE1 : J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    while(1){
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        
        int  sen = sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&envoie,sizeof(struct sockaddr));
        if(sen==-1){
            raler("send etablisemment \n ");
        }

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            printf("select etablissement\n");
        }

        if(FD_ISSET(s,&fd_monitor)){
                printf("data ready\n");//Je receive et je test et si tout va bien je renvois avec les nouvelles valeurs
                //recevfrom
                int r =recvfrom(s,&p,DEFAULTSIZE,0,(struct sockaddr*)&ecoute,&size);
                if(r==-1){
                    raler("recvfrom 1\n");
                }
                //deroulement de test
                printf("ETAPE2 : J'ai recu seq à %d et ack à %d\n",p.seq,p.acq);

                if(p.acq == a+1){
                    p.acq=p.seq+1;//ack = b+1
                    p.seq=a+1;
                    p.type=16;
                    p.id=ID++;//id++               
                    p.data[0]=mode;/////mode si 0 stopAndWait si 1 GobackN 
                    printf("ETAPE3 : J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);
                    //envoyer le dernier paquet en confirmant avoir recu l'ack
                    r=sendto(s,&p,DEFAULTSIZE,0,(struct sockaddr * )&envoie,sizeof(struct sockaddr));
                    if(r==-1){
                        raler("send paquet");
                    }
                    printf("J'ai envoyé mon paquet final\n");
                }
                else{
                    printf("mauvais comportement du serveur en 3 way-shakehand\n");
                    exit(EXIT_FAILURE);
                }
                printf("---\t Connexion établie \t---\n");
                ID=0;              
                return 0 ;
        }else{
            continue;
        }
    }
    

}


/**
 * @brief Procede du 3 way handshake du cote destination(server) 
 * 0 si bonne ouverture, 1 sinon 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int etablissementConnexionServer (int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){
   
    unsigned short b = generateRandInt(100);
    int retour=0;

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

    struct packet p=init_packet() ;


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

            if((retour=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            }

            printf("ETAPE1 : J'ai recu seq à %d et ack à %d\n",p.seq,p.acq);

            //preparation de donnée (ACK à envoyer)
            p.type+=16;
            p.acq=p.seq+1;
            p.seq=b;
            p.id=ID++; 
            printf("ETAPE2 :J'ai mit seq à %d et ack à %d\n",p.seq,p.acq);

            //envoie d'ACK
            int sen=sendto(s,&p, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai envoyé l'ack \n");

            //Attendre la confirmation de reçu de l'ACK de la part de source 
            sen=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai recu la confirmation de l'ack \n");
            
            printf("ETAPE3: RECU seq = %d et l'ack =%d\n",p.seq,p.acq);
            if(p.acq==b+1){
                //test de mode 
                if(p.data[0]=='0'){//stopAndWait
                    printf("Mode Stop And Wait \n ---\t Connexion établie \t---\n");
                    return 0;
                }
                else{//GoBackN
                    if(p.data[0]=='1'){
                    printf("Mode GO-Back-N \n ---\t Connexion établie \t---\n");
                        return 2;
                    }
                    else{
                        printf("Mode iconnue - Merci de de passer 0 ou 1 en parametre :)\n");
                        return 1;
                    }
                }
                ID=0;              

            }
            else{
                ID=0;              
                return 1;
            }
        }
        else {//rien sur le socket?
            printf("rien reçu .. \nNouvelle tentative en cours\nMerci de patienter\n");
            continue;
        }
    }
    ID=0;              
    return 1;
}




/**
    @brief Fonction implementant la procedure stop n wait du cote source  
    @param socket
    @param sockaddr_in ecoute
    @param sockaddr_in envoie
    @return int 
*/

int stopNwait(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){

    clock_t begin = clock();

    FILE *gnuplot = fopen("StopWaitFig.p", "w");
    setupPlotStop(gnuplot);

    struct packet p=init_packet() ;
    int altern = 0 ; 
    p.seq=altern ;
    p.id=ID ; 
    p.type+=4;

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    FILE *fp=fopen("test.txt","r");

    if(fp==NULL){
        if(close(s)==-1){
            raler("close");
        }
        raler("fopen");
    }

    fgets(p.data,TAILLEFEN, fp );
    if( feof(fp) ) {
        fclose(fp); 
    }

    fseek(fp, TAILLEFEN, SEEK_CUR);
    int x=0;
    socklen_t size=sizeof(ecoute);

    while(1){
        x = sendto(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,
        sizeof(envoie)); 
        if(x==-1){
            if(close(s)==-1){
                raler("close");
            }
            raler("Sendto");
        }
        
        retval=select(FD_SETSIZE+1,&fd_monitor,NULL,NULL,&tv);
        switch (retval)
        {
        case -1 :
            if(close(s)==-1){
                raler("close");
            }
            raler("select GO BACK \n");
            break;
        default :
            x = sendto(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,
            sizeof(envoie)); 

            if(x==-1){
                if(close(s)==-1){
                    raler("close");
                }
                raler("Sendto");
            }
            messagesEnvoyes ++ ; 
            if(FD_ISSET(s,&fd_monitor)){  
                x=recvfrom(s,&p,DEFAULTSIZE,0,
                (struct sockaddr*)&ecoute,&size);

                if(p.type==2){
                    printf("jai recu type = 2 ");
                    clock_t end = clock();
                    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                    dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);
                    fprintf(gnuplot, "e");
                    fflush(gnuplot);
                    fclose (gnuplot);
                    return fermeture_connection_source(s,ecoute,envoie);
                }

                if(x==-1){
                    if(close(s)==-1){
                        raler("close");
                    }
                    raler("recv from \n");
                }
                //tester si p.type == 16 
                if(p.acq==(altern+1)%2 ){
                    messagesEnvoyes ++ ; 
                    altern =(altern+1)%2;
                    printf("%d \n ",altern);
                    p.seq=altern ;
                    ID++;
                    p.id=ID ;
                    fgets(p.data,TAILLEFEN, fp );
                    if( feof(fp) ) {
                        fclose(fp); 
                        printf("Plus de donnees a lire ;\n");
                        clock_t end = clock();
                        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                        dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);
                        fprintf(gnuplot, "e");
                        fflush(gnuplot);
                        fclose (gnuplot);
                        return fermeture_connection_source(s,ecoute,envoie);
                    }

                    clock_t end = clock();
                    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                    dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);

                    fseek(fp, TAILLEFEN, SEEK_CUR);
                    continue;
                }else{ // message perdu ou acq non recu 
                    messagesPerdus ++ ; 

                    clock_t end = clock();
                    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                    dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);
                    continue ; 
                }
                
            }
            
        }
    }

}




/**
 * @brief Fonction implementant la procedure stop n wait,cote server 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int stopNwaitServer (int s,struct sockaddr_in ecoute,
    struct sockaddr_in envoie){

    int numAck=0,retour=0;
    struct packet p=init_packet();

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

    FILE *fp = fopen("testColle.txt", "w");
    
    if (fp == NULL)
    {
        raler("Fopen server");
    }

    while(1){
        tv.tv_sec = 4;
        tv.tv_usec = 0;
        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            if(close(s)==-1){
                raler("close");
            }            
            raler("select stop and wait server\n");
        }
        if(FD_ISSET(s,&fd_monitor)){
            if((retour=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            } 
                // fermeture connection avec FIN 
            if(p.type==2){
                printf("Jai recu mon message de fin \n");
                fclose(fp); 
                return fermeture_connection_serveur(s,ecoute,envoie);
            }

            
            //Afficher le msg reçu :
            if(p.seq==numAck){
                //recuperer les données recues:
                fprintf(fp,"%s",p.data);
                p.acq=numAck;
                printf("\nMsg = %s\nSEQ = %d\tAQC de ce msg = %d\n",p.data,numAck,p.acq);
                numAck=(numAck+1)%2;

            }
            
            else{
                p.acq=numAck;
            }

            //envoie d'ACK
            int sen=sendto(s,&p, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }

            //printf("Etape 2: J'ai envoyé l'ack %d \n",p.acq);
            continue;
        }
        printf("Rien n'est recu\nNouvlelle tentative en cours../..\n");
        continue;
    }      
    return 1;
}


/**
 * @brief Fonction implementant la procedure go-back-N,cote server 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
 
int go_back_N_serevr (int s,struct sockaddr_in ecoute,
    struct sockaddr_in envoie){

    int resultat =0;
    short dernierSeqRecu = 0 ;


    struct packet p=init_packet();

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

    FILE *fp = fopen("testColle.txt", "w");
    if (fp == NULL)
    {
        raler("Fopen server");
    }

    while(1){
        tv.tv_sec = 4;
        tv.tv_usec = 0;

        retval = select(FD_SETSIZE+1, &fd_monitor, NULL, NULL, &tv);
        if(retval==-1){
            if(close(s)==-1){
                raler("close");
            }            
            raler("select Go-Back-N server\n");
        }

        if(FD_ISSET(s,&fd_monitor)){
            struct packet p=init_packet();

            //printf("data ready\n");//Je receve et je test et si tout va bien je renvois avec les nouvelles valeurs

            if((retval=recvfrom(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            }
            //printf("jai recu mon message seq : %d donnees : %s\n",p.seq,p.data);
            printf(" %s\n",p.data);
            fprintf(fp,"%s",p.data);
            // fermeture connection avec FIN 
            if(p.type==2){
                printf("Jai recu mon message de fin \n");
                fclose(fp);
                return fermeture_connection_serveur(s,ecoute,envoie);
            }


            //accepter en ordre
            if( (dernierSeqRecu + 1) == p.seq){
                dernierSeqRecu=p.seq;
            }
        }
        if(p.ecn>0){
            p=init_packet();
            p.acq=dernierSeqRecu;
            p.type=16;
            p.ecn=1;
        }else{
            p=init_packet();
            p.acq=dernierSeqRecu;
            p.type=16; 
        }
        
        //printf("Jenvoie acq de %d \n",p.acq);
        int x = sendto(s,&p,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,sizeof(envoie)); 
                    if(x==-1){
                        if(close(s)==-1){
                            raler("close");
                        }   
                    raler("Sendto");
                }    
        
    }
    return resultat;
}



void parcoursListe(struct noeud* node, int n)
{
    for(int i = 0 ; i<n;i++){
        if(node->suivant!=NULL){
            node=node->suivant;
        }
    }
}



/**
 * @brief Fonction implementant la procedure go-back-N,cote source 
 * 
 * @param s 
 * @param ecoute 
 * @param envoie 
 * @return int 
 */
int go_back_N_source (int s,struct sockaddr_in ecoute,
    struct sockaddr_in envoie){

     
    int taille_fenetre_congestion =1 ;
    int position = 0 ;

    int nb_places_libres = 1 ;

    int DernierSeqEnv = 0;
    int DernierAcqRecu = 0 ;
    int MPerdusSuite = 0 ;
    

    
    struct noeud* node = (struct noeud*) malloc(sizeof(struct noeud)) ; 
    struct noeud* tete = node;
    struct noeud* teteZero = node;
    struct packet p = init_packet();
    node->p = &p ;

    fd_set fd_monitor;
    struct timeval tv;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 4;
    tv.tv_usec = 0;

    int x=0,compteur=0;
    socklen_t size=sizeof(ecoute);

    FILE *fp=fopen("test.txt","r");

    if(fp==NULL){
        if(close(s)==-1){
            raler("close");
        }
        raler("fopen");
    }
    while(1){
        if( feof(fp) ) {
            fclose(fp); 
            node->p->type = 2;
            return fermeture_connection_source(s,ecoute,envoie);
        }

        while(nb_places_libres>0){
            position ++ ;
            fgets(node->p->data,node->p->fenetre-10, fp );
            fseek(fp,node->p->fenetre, SEEK_CUR);
            //printf("donnees lu sont %s \n",node->p->data);
            DernierSeqEnv++;
            node->p->seq = DernierSeqEnv ; 
            node->p->id = ID++;
            node->p->type=0;
            node->num++;
            if( feof(fp) ) {
                fclose(fp); 
                node->p->type = 2;
                return fermeture_connection_source(s,ecoute,envoie);
            }
            x = sendto(s,node->p,DEFAULTSIZE+1,0,(struct sockaddr*)&envoie,sizeof(envoie)); 
           //printf("on envoie un packet : %s \nSon SEQ = %d\n",node->p->data,node->p->seq);
            // printf("%s \n",node->p->data);
            if(x==-1){
                if(close(s)==-1){
                    raler("close");
                }   
                raler("Sendto");
            } 
            nb_places_libres--;
            if(node->suivant!=NULL){
                node=node->suivant; 
            }else{
                struct noeud *newNode =
                (struct noeud*) malloc(sizeof(struct noeud)) ; 
                push(&newNode,p);
              //  node->suivant=newNode;
            }
            DernierSeqEnv = node->p->seq ;
            messagesEnvoyes ++ ; 
        }
        tete=node ;
        int retval=select(FD_SETSIZE+1,&fd_monitor,NULL,NULL,&tv);
        if(retval==-1){
            if(close(s)==-1){
                raler("close");
            }   
            raler("Sendto");
        }

        if(FD_ISSET(s,&fd_monitor)){  
            x=recvfrom(s,&p,DEFAULTSIZE,0,
            (struct sockaddr*)&ecoute,&size);
            if(p.type==2){
                printf("jai recu type = 2 ");
                //gnuplot
                //..
                return fermeture_connection_source(s,ecoute,envoie);
            }
            
            if((p.acq == DernierSeqEnv)||(p.acq>DernierAcqRecu)){
                nb_places_libres = nb_places_libres + taille_fenetre_congestion;
                taille_fenetre_congestion++;
                printf("nb de place libre  :%d\n",nb_places_libres);
                printf("J'ai changé le nb de place libre  :%d\n",nb_places_libres);
                DernierAcqRecu = p.acq;
                printf("nb places libres = %d \n",nb_places_libres);
                    
            }
                
            else{
                if(p.ecn >0 ){
                    printf("ecn est active \n");
                    int pourcent=(taille_fenetre_congestion*52 )*0.1;
                    
                    if (pourcent>52){
                        int quotient= pourcent/52;
                        taille_fenetre_congestion-=quotient;//Sinon il faut peut-etre supprimer les noeuds
                        
                    //boucle de delete de n noeuds
                    }
                    int reste = (taille_fenetre_congestion * 52) % 52 ;
                    if(reste>=42){
                        parcoursListe(node,taille_fenetre_congestion);
                        deleteNode(&node);//A revoir
                        taille_fenetre_congestion--; 
                        nb_places_libres--;
                        reste-=42;
                        //node=tete;
                    }
                    if(reste>0){ 
                        parcoursListe(node,taille_fenetre_congestion);
                        node->p->fenetre=52-reste;
                    }
                    node = tete;
                } 
                printf("Message perdu \n");
                MPerdusSuite ++ ;
                if(MPerdusSuite == 3){
                    taille_fenetre_congestion = 1 ;
                    MPerdusSuite = 0 ;
                    //  continue;
                }
                else{//Il faut tester si p.acq==dernierAcqRecu ==> Perte 
                        // Sinon il ne s'agit pas d'une perte mais d'un acq normal
                    MPerdusSuite=1;
                    node = chercheList(teteZero,DernierAcqRecu-1);
                    if(taille_fenetre_congestion>1){
                        taille_fenetre_congestion = taille_fenetre_congestion / 2 ;
                    }
                }    
            }
        }
        else{
            printf("\nboucle vide\n\n");
            compteur++;
            if (compteur ==10){
                return fermeture_connection_source(s,ecoute,envoie);
            }
           //continue;   
        }
    }
    fclose(fp); 
    return 0;
}
