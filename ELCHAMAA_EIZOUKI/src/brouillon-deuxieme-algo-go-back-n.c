/*

int go_back_N_source (int s, struct sockaddr_in ecoute,
    struct sockaddr_in envoie){
    socklen_t size=sizeof(ecoute);

    int NumSeq = 0 ; 
    int nbPlacesLibres=1;
    int taille_fen_congest=52 ;
    int DernierAcqRecu = -1;
    int ID = 0 ;
    int messagePerdusSuite = 0 ;  

    clock_t begin = clock();

    struct packet p = init_packet() ;

    fd_set fd_monitor;
    struct timeval tv;

    FD_ZERO(&fd_monitor);
    FD_SET(s, &fd_monitor);

    tv.tv_sec = 4;
    tv.tv_usec = 0;


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
    liste *node = liste_vide();

    while(1){

        int quotient = taille_fen_congest % 52;
        int reste = taille_fen_congest - (taille_fen_congest % 52); 
        
        for(int x = 0; x < quotient ; x++){ 
            if(node->suivant==NULL){//on creer un nouveau noeud
                p = init_packet();
                node = liste_insertion_queue(node,&p);
            }
            else{//on se deplace au noeud suivant
                node=liste_suivant(node);
            }
            node->NumSeq = NumSeq ;
            fgets(node->p->data,DEFAULTSIZE, fp );
            fseek(fp,DEFAULTSIZE, SEEK_CUR);
            node->p->id=ID;
            node->p->seq=NumSeq;

            if( feof(fp) ) {
                fclose(fp); 
                clock_t end = clock();
                double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,
                    time_spent);
                fprintf(gnuplot, "e");
                fflush(gnuplot);
                fclose (gnuplot);
                return fermeture_connection_source(s,ecoute,envoie);
            }
            printf("%s\n",node->p->data);
            int x = sendto(s,node->p,DEFAULTSIZE+1,0,
                (struct sockaddr*)&envoie,sizeof(envoie));
            if(x==-1){
                if(close(s)==-1){
                    raler("close");
                }   
                raler("Sendto");
            } 
            messagesEnvoyes++; 
            nbPlacesLibres--;
            ID++;
            NumSeq++;

        }
        if(reste > 0 ){
            if(liste_est_vide(node)){//on creer un nouveau noeud                p = init_packet();
                node = liste_insertion_queue(node,&p);
            }
            else{//on se deplace au noeud suivant
                if(node->suivant!=NULL){
                    node=liste_suivant(node);
                } 
            }
            node->NumSeq = NumSeq ;
            fgets(node->p->data,reste, fp );
            fseek(fp,reste,SEEK_CUR);
            printf("%s\n",node->p->data);
            node->p->id=ID;
            node->p->seq=NumSeq;
            if( feof(fp) ) {
                fclose(fp); 
                clock_t end = clock();
                double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,
                    time_spent);
                fprintf(gnuplot, "e");
                fflush(gnuplot);
                fclose (gnuplot);
                return fermeture_connection_source(s,ecoute,envoie);
            }
            

            int x = sendto(s,node->p,DEFAULTSIZE+1,0,
                (struct sockaddr*)&envoie,sizeof(envoie));
            if(x==-1){
                if(close(s)==-1){
                    raler("close");
                }   
                raler("Sendto");
            } 
            messagesEnvoyes++; 
            nbPlacesLibres--;
            ID++;
        }

        
        int retval=select(FD_SETSIZE+1,&fd_monitor,NULL,NULL,&tv);
        if(retval==-1){
            if(close(s)==-1){
                raler("close");
            }   
            raler("Sendto");
        }

        if(FD_ISSET(s,&fd_monitor)){
            retval = recvfrom(s,&p,DEFAULTSIZE,0,
            (struct sockaddr*)&ecoute,&size);
        }
        if(retval==-1){
            if(close(s)==-1){
                raler("close");
            }
            raler("recv from \n");
        }
        
        if(p.type==2){
            printf("jai recu type = 2 ");
            clock_t end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);
            fprintf(gnuplot, "e");
            fflush(gnuplot);
            fclose (gnuplot);
            fclose(fp);
            liste_free(liste_debut(node));
            return fermeture_connection_source(s,ecoute,envoie);
        }
        
        if(p.acq == DernierAcqRecu){//perte
            messagePerdusSuite ++;

            if(messagePerdusSuite == 3){
                taille_fen_congest = 52 ; 
                messagePerdusSuite=1;
            }
            else{

                taille_fen_congest = taille_fen_congest / 2 ;  
                messagePerdusSuite ++;
 
            }
            
            node=liste_ieme(node,p.acq+1);
        } 

        else{// bien recu 
            taille_fen_congest  = taille_fen_congest + 52 ; 
            messagePerdusSuite = 0 ; 
        }

        if(p.ecn>0){
            printf("Ecn Active ! \n ");
            taille_fen_congest =  taille_fen_congest*0.1 ;
        }
    }

    return -1;
}

*/
