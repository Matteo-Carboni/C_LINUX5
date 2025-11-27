#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "protocole.h" // contient la cle et la structure d'un message

int idQ;
int pid1,pid2;
void handler_SIGINT(int sig);

int main()
{
  MESSAGE requete;
  pid_t destinataire;

  // Armement du signal SIGINT
  // (etape 6)
  struct sigaction sb;
  sb.sa_handler = handler_SIGINT;
  sigemptyset(&sb.sa_mask);
  sb.sa_flags = 0;
  sigaction(SIGINT, &sb, NULL);

  // Creation de la file de message
  fprintf(stderr,"(SERVEUR) Creation de la file de messages\n");
  // TO DO (etape 2)


  //                   _id de la file
  //                  /
  //                 /     _envoie signal a msgget et cree file si existe pas
  //                /     /     
  //               /     /                    _lecture et ecriture pour tous 
  //              /     /                    /
  //             /     /                    /
  idQ = msgget(CLE, IPC_CREAT| IPC_EXCL | 0666);
  if (idQ == -1) {
      perror("Erreur lors de la creation de la file de messages");
      exit(1);
  }
  

  // Attente de connection de 2 clients
  fprintf(stderr,"(SERVEUR) Attente de connection d'un premier client...\n");
  // (etape 5)

  if( msgrcv(idQ, &requete, sizeof(MESSAGE) - sizeof(long), 1, 0) == -1){

    perror("erreur msgrcv");
    exit(1);
  }
  pid1 = requete.expediteur;
  fprintf(stderr,"le client avec le pid (%d) est bien connecté", pid1);


  fprintf(stderr,"(SERVEUR) Attente de connection d'un second client...\n");
  // (etape 5)
    if( msgrcv(idQ, &requete, sizeof(MESSAGE) - sizeof(long), 1, 0) == -1){

    perror("erreur msgrcv");
  }
  pid2 = requete.expediteur;
  fprintf(stderr,"le client avec le pid (%d) est bien connecté", pid2);


    while (1) {
    // Attente d'une requête de type 1
      fprintf(stderr,"(SERVEUR) Attente d'une requete\n");
    if (msgrcv(idQ, &requete, sizeof(MESSAGE) - sizeof(long), 1, 0) == -1) {
        perror("Erreur msgrcv");
        exit(1);
    }
    fprintf(stderr, "(SERVEUR) Requete recue de %d : --%s--\n", requete.expediteur, requete.texte);

    // Construction de la réponse
    MESSAGE reponse;

    if(requete.expediteur == pid1){
      destinataire = pid2;
    }
    else if(requete.expediteur == pid2){
      destinataire = pid1;
    }

    reponse.type = destinataire;
    reponse.expediteur = 0;
    sprintf(reponse.texte, "(Serveur) %s", requete.texte);

    // Envoi de la réponse
    if (msgsnd(idQ, &reponse, sizeof(MESSAGE) - sizeof(long), 0) == -1) {
        perror("Erreur msgsnd");
        exit(1);
    }
    fprintf(stderr, "(SERVEUR) Envoi de la reponse à %d\n", reponse.type);

       // Réception d'un message de type 0 (le premier message disponible)
    if (msgsnd(idQ, &requete, sizeof(MESSAGE) - sizeof(long), 0) == -1) {
        perror("Erreur msgsnd");
        exit(1);
    }

    if( kill(destinataire, SIGUSR1) == -1){

      perror("erreur de kill");
      exit(1);
    }
  }
   

    exit(0);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Handlers de signaux ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (etape 6)
void handler_SIGINT(int sig){

  fprintf(stderr, "(SERVEUR) Reception du signal sigint et destruction de la file\n");
  if( msgctl(idQ, IPC_RMID, NULL) == -1){

    perror("erreur de msgctl");
    exit(1);
  }
  exit(0);
}
