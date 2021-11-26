#include "packet.h"
#include <unistd.h>



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
 
int go_back_N_server (int s,struct sockaddr_in ecoute,
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







int main (int argc, char * argv[]){

    if(argc!=4){
        raler("Mauvais nombre d'arguments \n");
    }
    char *ipDistante=argv[1];
    char *portLocal=argv[2];
    char *portEnvoie=argv[3];

    unsigned short portLocalInt=atoi(portLocal);//6666
    unsigned short portEcouteInt=atoi(portEnvoie); //5555

    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    
    s= creationSocket(s);

    ecoute.sin_family = AF_INET;
    ecoute.sin_port = htons(portLocalInt);
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    
    bind(s,(struct sockaddr*)&ecoute,sizeof(struct sockaddr_in));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(portEcouteInt); 
    inet_pton(AF_INET,ipDistante,&(envoie.sin_addr));//"127.0.0.1"

    int x =etablissementConnexionServer(s,ecoute,envoie);
    
    if(x==1){//probleme de connexion
        printf("Pb de connexion retour 1\n")   ;
    }

    if(x==0){//mode stop and wait
        stopNwaitServer(s,ecoute,envoie);
    }
     if(x==2){//mode Go back N
        go_back_N_server(s,ecoute,envoie);
    }

    printf("%d \n", x);
return 0;

}
