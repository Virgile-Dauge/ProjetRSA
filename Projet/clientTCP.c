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

	//struct sockaddr_in cli_addr; /* socket client */
  	struct sockaddr_in ser_addr; /* socket serveur  */
	struct sockaddr_in test_addr; /* socket serveur  */
  	struct in_addr adresse; /* adresse IP sur 32 bits bien rangee pour le protocole IP */
  	socklen_t addrlen; /* pour stocker la longueur de la structure demandee/rendue */
  	int err; /* numero d'erreur rendu par certaines fonctions a tester !! */
  	//int nb; /* nb octets pris en compte par UDP ou passes par UDP  */
  	char requete[32767];
  	char reponse[32767]; /* tableau pour mettre les octets recus */
  	char pseudo[32]; /* tableau pour accueillir le pseudo */
  	int gotToken = 0;

	if (argc < 4)
	{
		printf("Erreur, veuillez indiquer en argument l'adresse IP, le numero de port et le pseudo\n");
		exit(1);
	}


  	/* Creation du point de communication local */
  	sd = socket(AF_INET, SOCK_STREAM,0);

  	if(sd < 0)
    {
  	  perror("cli_udp :erreur lors de la creation du point de communication");
  	  exit(-1);
  	}



  	/* Initialisation  de ser_addr */
 	memset(&ser_addr, 0, sizeof(struct sockaddr_in));

  	/* On remplit le necessaire pour parler au serveur */
  	ser_addr.sin_family = AF_INET;
  	ser_addr.sin_port = htons((uint16_t)atoi(argv[2]));
  	err=inet_aton(argv[1] ,&adresse);

  	strcat(pseudo, argv[3]);
  	printf("Pseudo : %s\n\n", pseudo);

    /* on vérifie que l'adresse est valide */
  	if(err <= 0)
    {
        perror("inet_aton : adresse IP non valide");
        exit(-1);
  	}

  	ser_addr.sin_addr=adresse;

	addrlen = sizeof(ser_addr);
	
	memset(&test_addr, 0, sizeof(struct sockaddr_in));

	err = getsockname(sd, (struct sockaddr *)&test_addr, &addrlen);
	if(err < 0)
    {	
        perror("\ngetsockname : erreur lors du getsockname\n\n");
  	}
	else
	{
		printf("GETSOCKNAME :\n");
		printf("Famille : %d\n",test_addr.sin_family);
		printf("Port : %d\n",test_addr.sin_port);
		printf("Adresse IP : %s\n\n",inet_ntoa(test_addr.sin_addr));
	}

	memset(&test_addr, 0, sizeof(struct sockaddr_in));

	err = getpeername(sd, (struct sockaddr *)&test_addr, &addrlen);
	if(err < 0)
    {	
        perror("getpeername : erreur lors du getpeername");
		printf("\n");
  	}
	else
	{
		printf("getpeername : %d\n\n", err);
	}


	/* On se connecte au serveur */
	printf("Connexion en cours...\n");
	err = connect(sd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

	/* On vérifie que la connexion s'est bien passe */
    if(err < 0)
    {	
        perror("sendto : erreur lors de la connexion vers le serveur");
        exit(-1);
  	}

	printf("Connexion reussi !\n\n");

	memset(&test_addr, 0, sizeof(struct sockaddr_in));

	err = getsockname(sd, (struct sockaddr *)&test_addr, &addrlen);
	if(err < 0)
    {	
        perror("\ngetsockname : erreur lors du getsockname");
  	}
	else
	{
		printf("GETSOCKNAME :\n");
		printf("Famille : %d\n",test_addr.sin_family);
		printf("Port : %d\n",test_addr.sin_port);
		printf("Adresse IP : %s\n\n",inet_ntoa(test_addr.sin_addr));
	}

	memset(&test_addr, 0, sizeof(struct sockaddr_in));

	err = getpeername(sd, (struct sockaddr *)&test_addr, &addrlen);
	if(err < 0)
    {	
        perror("\ngetpeername : erreur lors du getpeername\n\n");
  	}
	else
	{
		printf("GETPEERNAME :\n");
		printf("Famille : %d\n",test_addr.sin_family);
		printf("Port : %d\n",test_addr.sin_port);
		printf("Adresse IP : %s\n\n",inet_ntoa(test_addr.sin_addr));
	}

	err = write(sd, pseudo, strlen(pseudo) + 1);

	char *test = malloc(sizeof("/quit"));
	int j;
	int connect = 1;

	while(connect){

		printf("Veuillez entrez le texte à envoyer : ");
		fgets(requete, 32767 - 1, stdin);
		if(requete[0] =='/' && requete[1] == 'k'){
			gotToken = 0;
			printf("Token rendu. Entrez du texte pour le redemander");
			memset(requete, 0, sizeof(requete));
			fgets(requete, 32767 - 1, stdin);
		}
		printf("\n");

	    /* On envoie au serveur */
	  	err = write(sd, requete, strlen(requete) + 1);


		while(gotToken == 0){
			err = read(sd, reponse, sizeof(reponse));
			if(reponse[0] =='/' && reponse[1] == 't' && reponse[2] == 'o' && reponse[3] == 'k' && reponse[4] == 'e'){
				gotToken = 1;
				printf("Vous avez la parole. Entrez /k pour la rendre\n");
			}
			else{
				printf("%s\n", reponse);
				memset(reponse, 0, sizeof(reponse));
			}
		}

		memset(test, 0, sizeof(test));
		if(strlen(requete) > 5){
			for(j = 0; j < 5; j++){
				test[j] = requete[j];
			}
		}

		if(strcmp(test, "/quit") == 0){		
			printf("Bye\n");
			connect = 0;
		}
	}
	/* on ferme le socket*/
	err = close(sd);

	/* On vérifie que la fermeture s'est bien passe */
	if (err < 0)
	{
		perror("recvfrom : erreur lors de la fermeture");
		exit(-1) ;
	}

  	return(0);
}
