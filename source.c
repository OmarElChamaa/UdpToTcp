#include "packet.h"
#include <unistd.h>



int id = 0 ;

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
    p.id=id ; 
    p.type+=4;
    p.fenetre=42;

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
    printf("donnees lu sont %s \n",p.data);

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
        messagesEnvoyes ++ ; 
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
                    altern =(altern+1)%2;
                    printf("Jai recu in je modif altern %d \n ",altern);
                    p.seq=altern ;
                    id++;
                    p.id=id ;
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
                    //dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);

                    fseek(fp, TAILLEFEN, SEEK_CUR);
                    printf("donnees lu sont %s \n",p.data);
                    continue;
                }else{ // message perdu ou acq non recu 
                    messagesPerdus ++ ; 

                    clock_t end = clock();
                    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                    dessinerFigure(gnuplot,messagesEnvoyes,messagesPerdus,time_spent);
                    continue ; 
                }

                //else if
                //si je recoi syn + acq 
                //je renvoie acq
                
            }
            
        }
    }

}





int main (){
    int s=0;
    struct sockaddr_in ecoute;
    struct sockaddr_in envoie;
    char mode ='0'; ///ca va etre intialiser a argv[1]

    s = socket(AF_INET, SOCK_DGRAM, 0);

    ecoute.sin_family = AF_INET; 
    ecoute.sin_port = htons(3333); 
    ecoute.sin_addr.s_addr = htonl(INADDR_ANY) ;
    bind(s, (struct sockaddr *)&ecoute,sizeof(ecoute));


    memset(&envoie,0,sizeof(envoie));
    envoie.sin_family = AF_INET; 
    envoie.sin_port = htons(4444); 
    inet_pton(AF_INET,"127.0.0.1",&(envoie.sin_addr));
    

    int x =etablissementConnexionSource(s,ecoute,envoie,mode);
    if(x==0){
        stopNwait(s,ecoute,envoie);
    }
    printf("%d \n", x);

    return 0;
}

