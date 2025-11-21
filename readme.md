# Exercice 5 de "Développement système en C sous Linux"

Objectifs :
* Se familiariser avec les files de messages  (msgget, msgctl, msgsnd et msgrcv).
* Se familiariser avec l’envoi et l’armement des signaux (kill et sigaction).
* Et toujours : création d’un makefile.



// Réception d'un message de type 0 (le premier message disponible)
//if (msgrcv(idQ, &requete, sizeof(MESSAGE) - sizeof(long), 0, 0) == -1) {
//   perror("Erreur msgrcv");
//    exit(EXIT_FAILURE);
//}
Explications :

idQ est l'identifiant de la file.

&requete est un pointeur vers la structure MESSAGE où sera stockée la requête reçue.

sizeof(MESSAGE) - sizeof(long) est la taille du message à recevoir (on retire la taille du champ type car msgrcv ne la compte pas dans la taille).

Le 0 en 4ème argument indique qu'on accepte n'importe quel type de message (sinon on pourrait recevoir uniquement un type donné).

Le dernier 0 correspond aux flags, ici aucun flag spécial.

Une fois reçue, vous pouvez ensuite afficher la requête et identifier son expéditeur via requete.expediteur et le contenu via requete.texte.
