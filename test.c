
//////////////////////////////////FCT ETABLISSEMENT DE CONNEXION COTÉ SERVEUR ////////////////
int etablissementConnexionServer (int s,struct sockaddr_in ecoute,
struct sockaddr_in envoie){
   
    int b = generateRandInt(100);
   // char buff [DEFAULTSIZE] ;
    int retour=0;

    //binding(s,ecoute);//lier le port
    char buf[DEFAULTSIZE];
    memset(buf, '\0',DEFAULTSIZE);

    fd_set fd_monitor;
    struct timeval tv;
    int retval;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);
    socklen_t size=sizeof(ecoute);

  

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

            if((retour=recvfrom(s,buf,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size))==-1){
                raler("recvfrom");
            }
            printf("J'ai recu le premier paquet\n");

            char * inter_buf = buf;
            //analyse de données reçu :
            struct packet p=generatePacketFromBuf(inter_buf);
            printf("J'ai recuperé les donnée du premier paquet \n");

            //preparation de donnée (ACK à envoyer)
            p.type.ACK=16;
            p.acq=p.seq+1;
            p.seq=b;
            p.type.SYN=1;
            p.id=ID++; 
            char *inter2_buf= generatePacket(p);
            printf("J'ai preparé l'ack \n");

            //envoie d'ACK
            int sen=sendto(s,inter2_buf, DEFAULTSIZE,0,(struct sockaddr*)&envoie,size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai envoyé l'ack \n");
            free(inter2_buf);
            //Attendre la confirmation de reçu de l'ACK de la part de source 
            char inter3_buf[DEFAULTSIZE];
            memset(inter3_buf, '\0',DEFAULTSIZE);
            sen=recvfrom(s,inter3_buf,DEFAULTSIZE+1,0,(struct sockaddr*)&ecoute,&size);
            if(sen==-1){
                raler("sender \n");
            }
            printf("J'ai recu la confirmation de l'ack \n");
            p=generatePacketFromBuf(inter3_buf);
            if(p.acq==b+1){
                printf("connexion établie :)\n");
                return 1;
            }
            else{
                return-1;
            }
        }
        else {//rien sur le socket?
            printf("rien reçu .. \nDeuxième tentative en cours\nMerci de patienter\n");
            continue;
        }
    }
    return -1;
}
//////////////////////////////////END FUNCTION ///////////////////////////////////////////

