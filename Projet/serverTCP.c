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

int pseudoValide(char *pseudo, char pseudos[128][32], int nbPseudos){
	int valide = 1;
	int i;
	for(i = 0; i < nbPseudos; i++){
		if(strcmp(pseudos[i], pseudo) == 0){
			valide = 0;
			break;
		}
	}
	return valide;
}

int main(int argc, char ** argv ){

	int sd, sd2[128] ; /* socket descriptor */

  	struct sockaddr_in cli_addr; /* socket client */
  	struct sockaddr_in ser_addr; /* socket serveur  */
  	socklen_t addrlen; /* pour stocker la longueur de la structure demandee/rendue */
  	int err; /* numero d'erreur rendu par certaines fonctions a tester !! */
	char requete[32767];
  	char reponse[32767]; /* tableau pour mettre les octets recus */
  	char pseudos[128][32]; /* tableau accueillant les pseudos */
  	int nbConnect = 0;
	int i = 0;
	int pid;

	if (argc < 2)
	{
		printf("Erreur, veuillez indiquer en argument le numero de port\n");
		exit(1);
	}

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
  	ser_addr.sin_port = htons((uint16_t)atoi(argv[1]));
	
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
	err = listen(sd, 64);

	/* On vérifie que la l'attente s'est bien passe */
    if (err < 0) {
        perror("listen : erreur lors de l'attente");
        exit(-1) ;
    }

	/* On reçoit des informations */
    printf("Serveur lancé ! En attente de connexions...\n\n");

    while(1) {
		sd2[nbConnect] = accept(sd, (struct sockaddr *)&cli_addr, &addrlen);

		/* On vérifie que la la connexion s'est bien passe */
	    if (sd2[nbConnect] < 0) {
	        perror("listen : erreur lors de la connexion au client");
	        exit(-1) ;
	    }

		 /* On affiche les donnees du client */
		printf("Caracteristiques du client :\n\n");
		printf("Famille : %d\n",cli_addr.sin_family);
		printf("Port : %d\n",cli_addr.sin_port);
		printf("Adresse IP : %s\n\n",inet_ntoa(cli_addr.sin_addr));

		memset(requete, 0, sizeof(requete));

		err = read(sd2[nbConnect], requete, sizeof(requete));

		if(pseudoValide(requete, pseudos, nbConnect) == 0){
			perror("erreur, pseudo déja utilisé");
		}
		else{
			strcat(pseudos[nbConnect], requete);
			nbConnect++;
			pid = fork();
		}

		if(pid == 0){
			/* On est dans le fils */
			char *test = malloc(sizeof("/quit"));
			char pseudo[32];
			strcat(pseudo, requete);
			int j, k;
			int sdCurrent = sd2[nbConnect-1];

			while(strcmp(test, "/quit") != 0){

				memset(requete, 0, sizeof(requete));
				memset(reponse, 0, sizeof(reponse));
				memset(test, 0, sizeof(test));

				err = read(sdCurrent, requete, sizeof(requete));

				err = write(sdCurrent, "/toke", strlen("/toke")+1);

				while(requete[0] !='/' && requete[1] != 'k'){

					if(strlen(requete) > 5){
						for(j = 0; j < 5; j++){
							test[j] = requete[j];
						}
					}
					printf("Valeur du retour de read : %d\n\n", err);

					/* On vérifie que la reception s'est bien passe */
					if (err < 0) {
						perror("read : erreur lors de la reception du serveur");
						exit(-1) ;
					}

					/* On affiche la chaine */
					strcat(reponse, pseudo);
					strcat(reponse, " : ");
					strcat(reponse, requete);
					printf("%s\n\n", reponse);

					/* On envoie au client */
					for(k = 0; k < nbConnect; k++){
						err = write(sd2[k], reponse, strlen(reponse) + 1);
						/* On vérifie que l'envoi s'est bien passe */
						if(err < 0) {	
							perror("write : erreur lors de l'envoi vers le client");
							exit(-1);
						}
					}
					memset(requete, 0, sizeof(requete));
					memset(reponse, 0, sizeof(reponse));
					printf("je read\n");
					err = read(sdCurrent, requete, sizeof(requete));
					printf("j'ai read\n");
				}
			}
			free(test);
			/* on ferme le socket 2*/
			err = close(sdCurrent);

			/* On vérifie que la fermeture s'est bien passe */
			if (err < 0) {
				perror("close : erreur lors de la fermeture");
				exit(-1) ;
			}
		} else {
			/* on ferme le socket 2*/
			err = close(sd2[nbConnect-1]);

			/* On vérifie que la fermeture s'est bien passe */
			if (err < 0) {
				perror("close : erreur lors de la fermeture");
				exit(-1) ;
			}
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
  	return(0);
}