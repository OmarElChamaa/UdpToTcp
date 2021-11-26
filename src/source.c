#include "../inc/packet.h"
#include <unistd.h>




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
    @brief Fonction implementant la procedure stop n wait du cote source  
    @param socket
    @param sockaddr_in ecoute
    @param sockaddr_in envoie
    @return int 
*/

int stopNwait(int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){

    clock_t begin = clock();

    FILE *gnuplot = fopen("./bin/StopWaitFig.p", "w");
    if (gnuplot ==NULL){
        raler("gnuplot open \n");
    }
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

    FILE *fp=fopen("./bin/test.txt","r");

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
    

    liste *node = liste_vide();
    struct packet p = init_packet();
    
    node = liste_insertion_queue(node,&p);
    node->NumSeq = 0 ;
    

    fd_set fd_monitor;
    struct timeval tv;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 4;
    tv.tv_usec = 0;

    int x=0,compteur=0;
    socklen_t size=sizeof(ecoute);

    FILE *fp=fopen("./bin/test.txt","r");

    FILE *gnuplot = fopen("./bin/StopWaitFig.p", "w");
    if (gnuplot ==NULL){
        raler("gnuplot open \n");
    }
    setupPlotStop(gnuplot);

    if(fp==NULL){
        if(close(s)==-1){
            raler("close");
        }
        raler("fopen source go back \n");
    }
    while(1){
        if( feof(fp) ) {
            fclose(fp); 
            node->p->type = 2;
            liste_free(liste_debut(node));
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
            node->NumSeq++;
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
            }
           else{
                node = liste_insertion_queue(node,&p);
            }
            DernierSeqEnv = node->p->seq ;
            messagesEnvoyes ++ ; 
        }
        //tete=node ;
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
                liste_free(liste_debut(node));
                return fermeture_connection_source(s,ecoute,envoie);
            }
            if((p.acq == DernierSeqEnv)||(p.acq>DernierAcqRecu)){
                taille_fenetre_congestion++;
                nb_places_libres = nb_places_libres +(p.acq-DernierAcqRecu);
                //nb_places_libres = nb_places_libres +(taille_fenetre_congestion- (p.acq-DernierAcqRecu));
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
                        node = liste_ieme(node,taille_fenetre_congestion);
                        node = liste_suppression_queue(node);
                        taille_fenetre_congestion--; 
                        nb_places_libres--;
                        reste-=42;
                        //node=tete;
                    }
                    if(reste>0){ 
                        node = liste_ieme(node,taille_fenetre_congestion);
                        node->p->fenetre=52-reste;
                    }
                    //node = tete;
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
                    node = liste_ieme(node,DernierAcqRecu-1);
                    if(taille_fenetre_congestion>1){
                        taille_fenetre_congestion = taille_fenetre_congestion / 2 ;
                    }
                }    
            }
        }
        else{
            printf("\nboucle vide\n\n");
            compteur++;
            if (compteur ==5){
                liste_free(liste_debut(node));
                fclose(fp); 
                return fermeture_connection_source(s,ecoute,envoie);
            }
           //continue;   
        }
    }
    return 0;
}



int main(int argc, char * argv[]){

    if(argc!=5){
        raler("Mauvais nombre d'arguments \n");
    }
    char *mode=argv[1];
    char *ipDistante=argv[2];
    char *portLocal=argv[3];
    char *portEnvoie=argv[4];

    unsigned short portLocalInt=atoi(portLocal);//3333
    unsigned short portEcouteInt=atoi(portEnvoie); //4444

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;

    s = socket(AF_INET, SOCK_DGRAM, 0);

    ecoute.sin_family = AF_INET; 
    ecoute.sin_port = htons(portLocalInt); 
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    bind(s, (struct sockaddr *)&ecoute,sizeof(ecoute));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(portEcouteInt); 
    inet_pton(AF_INET,ipDistante,&(envoie.sin_addr));
    
    
    if(strcmp(mode,"0")==0){
        int x =etablissementConnexionSource(s,ecoute,envoie,'0');
        if(x==0){
            stopNwait(s,ecoute,envoie);
        }
    }else{
        int x =etablissementConnexionSource(s,ecoute,envoie,'1');
        if(x==0){
            go_back_N_source(s,ecoute,envoie);
        }
    }
    return 0;
}

