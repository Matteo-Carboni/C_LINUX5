#include "windowclient.h"
#include "ui_windowclient.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

extern WindowClient *w;

#include "protocole.h" // contient la cle et la structure d'un message

void handler_SIGUSR1(int sig) {
    MESSAGE msg;
    // Lecture du message dont le type = pid du client (getpid())
    if (msgrcv(idQ, &msg, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1) {
        perror("Erreur msgrcv dans handler SIGUSR1");
        return;
    }
    if (w) {
        w->setRecu(msg.texte);  // Affiche le message dans l’UI
    }
}

extern char nomClient[40];
int idQ; // identifiant de la file de message

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowClient::WindowClient(QWidget *parent):QMainWindow(parent),ui(new Ui::WindowClient)
{
  ui->setupUi(this);
  setWindowTitle(nomClient);

  // Recuperation de l'identifiant de la file de messages
  fprintf(stderr,"(CLIENT %s %d) Recuperation de l'id de la file de messages\n",nomClient,getpid());

  //  (etape 2)

  idQ = msgget(CLE, 0);  // Ouvrir la file existante
  if (idQ == -1) {
      perror("Erreur récupération idQ");
      exit(EXIT_FAILURE);
  } else {
      fprintf(stderr, "(CLIENT %s) idQ récupéré = %d\n", nomClient, idQ);
  }

  // Envoi d'une requete d'identification

  //  (etape 5)

  MESSAGE msgId;
  msgId.type = 2;            // Type 2 comme convention pour identification
  msgId.expediteur = getpid();
  strcpy(msgId.texte, "client_id");
  if (msgsnd(idQ, &msgId, sizeof(MESSAGE) - sizeof(long), 0) == -1) {
      perror("Erreur envoi id client");
      exit(EXIT_FAILURE);
  }

  // Armement du signal SIGUSR1

  //  (etape 4)

  struct sigaction sa;
  sa.sa_handler = [](int) {
      MESSAGE msg;
      if (msgrcv(idQ, &msg, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1) {
          perror("Erreur msgrcv dans handler SIGUSR1");
          return;
      }
      w->setRecu(msg.texte);
  };
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);
}

WindowClient::~WindowClient()
{
  delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::setRecu(const char* Text)
{
  //fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditRecu->clear();
    return;
  }
  ui->lineEditRecu->setText(Text);
}

void WindowClient::setAEnvoyer(const char* Text)
{
  //fprintf(stderr,"---%s---\n",Text);
  if (strlen(Text) == 0 )
  {
    ui->lineEditEnvoyer->clear();
    return;
  }
  ui->lineEditEnvoyer->setText(Text);
}

const char* WindowClient::getAEnvoyer()
{
  if (ui->lineEditEnvoyer->text().size())
  { 
    strcpy(aEnvoyer,ui->lineEditEnvoyer->text().toStdString().c_str());
    return aEnvoyer;
  }
  return NULL;
}

const char* WindowClient::getRecu()
{
  if (ui->lineEditRecu->text().size())
  { 
    strcpy(recu,ui->lineEditRecu->text().toStdString().c_str());
    return recu;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowClient::on_pushButtonEnvoyer_clicked()
{
    fprintf(stderr, "Clic sur le bouton Envoyer\n");
    // Construction du message à envoyer
    MESSAGE msg;
    msg.type = 1;
    msg.expediteur = getpid();
    strcpy(msg.texte, getAEnvoyer());

    // Envoi du message
    if (msgsnd(idQ, &msg, sizeof(MESSAGE) - sizeof(long), 0) == -1) {
        perror("Erreur msgsnd");
        return;
    }

    // Lecture immédiate (on récupère le message du même type pour le test)
    MESSAGE recuMsg;
    if (msgrcv(idQ, &recuMsg, sizeof(MESSAGE) - sizeof(long), 1, 0) == -1) {
        perror("Erreur msgrcv");
        return;
    }

    // Affichage dans l’UI
    setRecu(recuMsg.texte);
}


void WindowClient::on_pushButtonQuitter_clicked()
{
  fprintf(stderr,"Clic sur le bouton Quitter\n");
  exit(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Handlers de signaux ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  (etape 4)


