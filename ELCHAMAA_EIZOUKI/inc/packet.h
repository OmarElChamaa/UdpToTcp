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

typedef struct liste {
    struct packet* p;
    int NumSeq ;
    struct liste *suivant;
    struct liste *precedent;
}liste;


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


//////////////////////////////////FCTS sur les listes//////////////////////////


liste *liste_vide(void) {
  return NULL;
}

void liste_free(liste *const l) {
  liste *parcours = l;
  while (parcours) {
    liste *next = parcours->suivant;
    
    if(parcours->p!=NULL){
        free(parcours->p);
    }
    free(parcours);
    parcours = next;
  }
}

liste *liste_debut(const liste *l) {
  if (!l)
    return NULL;

  const liste *parcours = l;
  while (parcours->precedent){
    parcours = parcours->precedent;
  }
  return (liste *) parcours;
}


liste *liste_fin(const liste *const l) {
  if (!l)
    return NULL;

  const liste *parcours = l;
  while (parcours->suivant){
    parcours = parcours->suivant;
  }
  return (liste *) parcours;
}

liste *liste_insertion_queue(liste *l, struct packet* p) {
  liste *const nouveau_maillon = malloc(sizeof *nouveau_maillon);

  nouveau_maillon->p = p;

  liste *const fin = liste_fin(l);
  nouveau_maillon->precedent = fin;
  nouveau_maillon->suivant = NULL;
  if (fin)
    fin->suivant = nouveau_maillon;

  return nouveau_maillon;
}


liste *liste_suppression_tete(liste *l) {
  if (!l)
    return NULL;

  liste *const debut = liste_debut(l);
  liste *const retour = debut->suivant;
  if(debut->p!=NULL){
    free(debut->p);
  }
  if (retour)
    retour->precedent = NULL;

  return retour;
}


liste *liste_suppression_queue(liste *l) {
  if (!l)
    return NULL;

  liste *const fin = liste_fin(l);
  liste *const retour = fin->precedent;
  if(fin->p!=NULL){
        free(fin->p);
    }
  free(fin);
  if (retour)
    retour->suivant = NULL;

  return retour;
}








liste *liste_suivant(const liste *l) {
  return (liste *) (l ? l->suivant : l);
}

liste *liste_precedent(const liste *l) {
  return (liste *) (l ? l->precedent : l);
}

liste *liste_ieme(const liste *l, size_t n) {
  liste *parcours = liste_debut(l);
  for (size_t i = 0; parcours && i < n; ++i){
    parcours = parcours->suivant;
  }
  return parcours;
}

struct packet * liste_element(const liste *l) {
  return l->p;
}

int liste_est_vide(const liste *l) {
  return l ? 0 : 1;
}

size_t liste_longueur(const liste *l) {
  size_t longueur = 0;
  for (const liste *parcours = l; parcours; parcours = parcours->suivant){
    longueur++;
  }
  return longueur;
}
//////////////////////////////////////////////////////////////////////////////////////

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
    fprintf(gnuplot, "set terminal png size 600,600\nset output'../bin/fig.png'\n");
    fprintf(gnuplot, "set xlabel \"temps en s\"\nset ylabel \"nbr messages\"\n");
    fprintf(gnuplot, "set title \"message perdu et envoye par rapport au temps\"\n");
}



/**
 * @brief initiale les champs du packet 
 * 
 * @return struct packet 
 */
struct packet init_packet(){
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
