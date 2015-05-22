/* include pour socket, bind, sendto */
#include <sys/types.h>
#include <sys/socket.h>

/* include pour inet_aton htons */
#include <netinet/in.h>
#include <arpa/inet.h>

/* include pour perror*/
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


/* **************************************************************** */
/* serveur TCP 														*/
/* **************************************************************** */

int main(int argc, char ** argv ){

	int sd, sd2 ; /* socket descriptor */

  	struct sockaddr_in cli_addr; /* socket client */
  	struct sockaddr_in ser_addr; /* socket serveur  */
  	//struct in_addr adresse; /* adresse IP sur 32 bits bien rangee pour le protocole IP */
  	socklen_t addrlen; /* pour stocker la longueur de la structure demandee/rendue */
  	int err; /* numero d'erreur rendu par certaines fonctions a tester !! */
	char requete[32767];
  	char reponse[32767]; /* tableau pour mettre les octets recus */
	int i = 0;

  	/* Creation du point de communication local */
  	sd = socket(AF_INET, SOCK_STREAM, 0);

	/* Initialisation  de ser_addr */
 	memset(&ser_addr, 0, sizeof(struct sockaddr_in));

	
  	if(sd < 0) {
  	  perror("ser_udp :erreur lors de la creation du point de communication");
  	  exit(-1);
  	}

	/* On remplit le necessaire pour configurer le serveur */
	ser_addr.sin_family = AF_INET;
  	ser_addr.sin_port = htons((uint16_t)5400);
	
	/* adresse IP de la machine */
  	ser_addr.sin_addr.s_addr = INADDR_ANY;

	/* bind avec ser_addr initialisé sur les lignes précédentes */
	err = bind(sd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

	/* on vérifie que le bind s'est bien passé */
	if (err < 0) {
		perror("ser_udp : erreur lors de l'obtention du nom du socket");
  	  	exit(-1);
	}

	/* Initialisation  de cli_addr */
  	memset(&cli_addr, 0, sizeof(struct sockaddr_in));

	addrlen = sizeof(cli_addr);

	/* On se place en attente d'une requete */
	err = listen(sd, 1);

	/* On vérifie que la l'attente s'est bien passe */
    if (err < 0) {
        perror("listen : erreur lors de l'attente");
        exit(-1) ;
    }

	/* On reçoit des informations */
    printf("Le serveur attend...\n\n");

	sd2 = accept(sd, (struct sockaddr *)&cli_addr, &addrlen);

	/* On vérifie que la la connexion s'est bien passe */
    if (sd2 < 0) {
        perror("listen : erreur lors de la connexion au client");
        exit(-1) ;
    }

	 /* On affiche les donnees du client */
	printf("Caracteristiques du client :\n\n");
	printf("Famille : %d\n",cli_addr.sin_family);
	printf("Port : %d\n",cli_addr.sin_port);
	printf("Adresse IP : %s\n\n",inet_ntoa(cli_addr.sin_addr));

	while(requete[0] != 'B' && requete[1] != 'y' && requete[2] != 'e') {

  		memset(requete, 0, sizeof(requete));

		err = read(sd2, requete, sizeof(requete));

		printf("Valeur du retour de read : %d\n\n", err);

		/* On vérifie que la reception s'est bien passe */
		if (err < 0) {
			perror("read : erreur lors de la reception du serveur");
			exit(-1) ;
		}

		/* On affiche la nouvelle chaine */
		printf("Valeur de la chaine : %s\n\n", requete);

	  	/* On convertit la requete de min a MAJ */
		for (i = 0; i < err + 1 ; i++) {
			reponse[i] = toupper(requete[i]);
		}

		/* On affiche la nouvelle chaine */
		printf("Valeur de la nouvelle chaine : %s\n\n", reponse);

		/* On envoie au client */
	    printf("Envoi au client...\n");
	  	err = write(sd2, reponse, strlen(reponse) + 1);
	    printf("Envoi effectue !\n\n");
			
		printf("Valeur de retour de write : %d\n\n", err);

		/* On vérifie que l'envoi s'est bien passe */
	    if(err < 0) {	
			perror("write : erreur lors de l'envoi vers le client");
			exit(-1);
	  	}

  	}

  	printf("Fermeture du client \n\n");

	/* on ferme le socket*/
    err = close(sd);

    /* On vérifie que la fermeture s'est bien passe */
	if (err < 0) {
		perror("close : erreur lors de la fermeture");
		exit(-1) ;
	}

	/* on ferme le socket 2*/
    err = close(sd2);

    	/* On vérifie que la fermeture s'est bien passe */
	if (err < 0) {
		perror("close : erreur lors de la fermeture");
		exit(-1) ;
	}

  	return(0);
}