#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>


/* **************************************************************** */
/* serveur TCP 														*/
/* **************************************************************** */

int str_echo (int sockfd, int tab_clients[128])
{
  	int nrcv, nsnd, i;
  	char msg[32767];


  	/*    * Attendre  le message envoye par le client 
   	*/
  	memset( (char*) msg, 0, sizeof(msg) );
  	if ( (nrcv= read ( sockfd, msg, sizeof(msg)-1) ) < 0 )  {
	    perror ("servmulti : : readn error on socket");
	    exit (1);
  	}

  	msg[nrcv]='\0';
  	printf ("servmulti :message recu=%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

  	for(i = 0; i < 128; i++){

	  	if(tab_clients[i] > 0){

			if ( (nsnd = write (tab_clients[i], msg, nrcv) ) <0 ) {
		    	printf ("servmulti : writen error on socket");
		    	exit (1);
		    }
		}
    }
    printf ("nsnd = %d \n", nsnd);
    return (nsnd);
	
}

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


  	struct sockaddr_in cli_addr; /* socket client */
  	struct sockaddr_in ser_addr; /* socket serveur  */
  	socklen_t addrlen; /* pour stocker la longueur de la structure demandee/rendue */
  	int err; /* numero d'erreur rendu par certaines fonctions a tester !! */
  	int erreurPseudo;
	char requete[32767];
  	char reponse[32767]; /* tableau pour mettre les octets recus */
  	char pseudos[128][32]; /* tableau accueillant les pseudos */
  	int nbConnect = 0;
	int sockfd, n, newsockfd, childpid, servlen,fin, i, nbfd, sockcli, j;
  	socklen_t clilen;
  	int tab_clients[128];
  	fd_set rset, pset;

	if (argc < 2)
	{
		printf("Erreur, veuillez indiquer en argument le numero de port\n");
		exit(1);
	}

  	/* Creation du point de communication local */
  	sockfd = socket(AF_INET, SOCK_STREAM, 0);

  	int maxfdp = sockfd+1;

	for(i=0;i<128;i++){
		tab_clients[i] = -1;
	}

	FD_ZERO(&pset);
	FD_ZERO(&rset);

	FD_SET(sockfd, &rset);

	/* Initialisation  de ser_addr */
 	memset(&ser_addr, 0, sizeof(struct sockaddr_in));

	
  	if(sockfd < 0) {
  	  perror("ser_udp :erreur lors de la creation du point de communication");
  	  exit(-1);
  	}

	/* On remplit le necessaire pour configurer le serveur */
	ser_addr.sin_family = AF_INET;
  	ser_addr.sin_port = htons((uint16_t)atoi(argv[1]));
	
	/* adresse IP de la machine */
  	ser_addr.sin_addr.s_addr = INADDR_ANY;

	/* bind avec ser_addr initialisé sur les lignes précédentes */
	err = bind(sockfd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

	/* on vérifie que le bind s'est bien passé */
	if (err < 0) {
		perror("ser_udp : erreur lors de l'obtention du nom du socket");
  	  	exit(-1);
	}

	/* Initialisation  de cli_addr */
  	memset(&cli_addr, 0, sizeof(struct sockaddr_in));

	addrlen = sizeof(cli_addr);

	/* On se place en attente d'une requete */
	err = listen(sockfd, 64);

	/* On vérifie que la l'attente s'est bien passe */
    if (err < 0) {
        perror("listen : erreur lors de l'attente");
        exit(-1) ;
    }

	/* On reçoit des informations */
    printf("Serveur lancé ! En attente de connexions...\n\n");

    for (;;) {

    	erreurPseudo = 0;

		pset = rset;
		nbfd = select(maxfdp, &pset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &pset)){
			clilen = sizeof(cli_addr);
			newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

			 /* On affiche les donnees du client */
			printf("Caracteristiques du client :\n\n");
			printf("Famille : %d\n",cli_addr.sin_family);
			printf("Port : %d\n",cli_addr.sin_port);
			printf("Adresse IP : %s\n\n",inet_ntoa(cli_addr.sin_addr));

			memset(requete, 0, sizeof(requete));

			err = read(newsockfd, requete, sizeof(requete));

			if(pseudoValide(requete, pseudos, nbConnect) == 0){
				perror("erreur, pseudo déja utilisé\n");
				err = send(newsockfd, "ER", sizeof("ER" + 1), 0);
				erreurPseudo = 1;
			}
			else{
				err = send(newsockfd, "OK", sizeof("OK" + 1), 0);
				strcat(pseudos[nbConnect], requete);
				nbConnect++;
			}

			if(erreurPseudo != 1){

				i = 0;
				while((i < 128) && (tab_clients[i] >= 0)){
					i++;
				}
				if(i==128){
					exit(1);
				}

				tab_clients[i] = newsockfd;

				FD_SET(newsockfd, & rset);
				if(newsockfd >= maxfdp){
					maxfdp = newsockfd + 1;
				}
				nbfd--;

			}
		}
		
	   	i=0;
		while((nbfd>0) && (i < 128)){
			if(((sockcli = tab_clients[i]) > 0) && (FD_ISSET(sockcli, &pset))){
				if(str_echo(sockcli, tab_clients)==0){
					close(sockcli);
					tab_clients[i] = -1;
					FD_CLR(sockcli, &rset);
				}
				nbfd--;
			}
			i++;
		}
 	}
}