/* include pour socket, bind, sendto */
#include <sys/types.h>
#include <sys/socket.h>

/* include pour inet_aton htons */
// a completer en faisant man inet_aton
#include <netinet/in.h>
#include <arpa/inet.h>

/* include pour perror*/
// a completer en faisant man perror
#include <stdio.h>
#include <errno.h>

/* include pour memset*/
#include <string.h>

/* include pour toupper */
#include <ctype.h>

/* include pour exit */
#include <stdlib.h>

/* include pour unistd */
#include <unistd.h>


/******************************************************************/
/* client UDP qui  envoie une chaine de caracteres  */
/* au serveur echo UDP                              */
/* recupere la reponse du serveur et l'affiche */
/******************************************************************/

int main(int argc, char ** argv ){

	int sd ; /* socket descriptor */

	struct sockaddr_in cli_addr; /* socket client */
	struct sockaddr_in ser_addr; /* socket serveur  */
	//struct in_addr adresse; /* adresse IP sur 32 bits bien rangee pour le protocole IP */
	socklen_t addrlen; /* pour stocker la longueur de la structure demandee/rendue */
	int err; /* numero d'erreur rendu par certaines fonctions a tester !! */
	//int nb; /* nb octets pris en compte par UDP ou passes par UDP  */
  char requete[256];
	char reponse[256]; /* tableau pour mettre les octets recus */
  int i = 0;


	/* Creation du point de communication local */
	sd = socket(AF_INET, SOCK_DGRAM,0);

	/* Initialisation  de ser_addr */
 	memset(&ser_addr, 0, sizeof(struct sockaddr_in));


	if(sd < 0)
  {
	  perror("ser_udp :erreur lors de la creation du point de communication");
	  exit(-1);
	}

	/* On remplit le necessaire pour configurer le serveur */
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons((uint16_t)atoi(argv[1]));

 /* adresse IP de la machine */
	ser_addr.sin_addr.s_addr = INADDR_ANY;

	/* bind avec ser_addr initialisé sur les lignes précédentes */
	err = bind(sd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

	/* on vérifie que le bind s'est bien passé */
	if (err < 0)
	{
	  perror("ser_udp : erreur lors de l'obtention du nom du socket");
	  exit(-1);
  }

	/* Initialisation  de cli_addr */
	memset(&cli_addr, 0, sizeof(struct sockaddr_in));

 addrlen = sizeof(cli_addr);

  while(1){

  	/* On reçoit des informations */
    printf("Le serveur attend...\n\n");

  	/* On se place en attente d'une requete */
  	err = recvfrom(sd , requete , sizeof(requete) , 0, (struct sockaddr *)&cli_addr, &addrlen);
  	
  	printf("Valeur du retour de recvfrom : %d\n\n", err);

  	/* On vérifie que la reception s'est bien passe */
    if (err < 0)
    {
        perror("recvfrom : erreur lors de la reception de la requete");
        exit(-1) ;
    }

  	/* On affiche la nouvelle chaine */
  	printf("Valeur de la chaine : %s\n\n", requete);

  	/* On envoie au client */
    printf("Envoi au client...\n");
  	err = sendto(sd, requete, err, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
    printf("Envoi effectue !\n\n");
  		
  	printf("Valeur de retour de sendto : %d\n\n", err);

  	/* On vérifie que l'envoi s'est bien passe */
    if(err < 0)
    {	
      perror("sendto : erreur lors de l'envoi vers le client");
      exit(-1);
  	}
  }

	/* on ferme le socket*/
  err = close(sd);

  /* On vérifie que la fermeture s'est bien passe */
  if (err < 0)
  {
    perror("close : erreur lors de la fermeture");
    exit(-1) ;
  }


  return(0);
}
