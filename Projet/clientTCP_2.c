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
  char requete2[32767];
	char reponse[32767]; /* tableau pour mettre les octets recus */
	char pseudo[32]; /* tableau pour accueillir le pseudo */
  char pseudoValide[2];
	int gotToken = 0;
	fd_set readfds;  

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

  err = read(sd, pseudoValide, sizeof(pseudoValide));

  if(pseudoValide[0] == 'E' && pseudoValide[1] == 'R'){
    printf("Désolé, ce pseudo est déja utilisé\n");
    err = close(sd);
    exit(-1);
  }

	char *test = malloc(sizeof("/quit"));
	int j;
	int connect = 1;

	while(connect){

  	FD_ZERO(&readfds);       

    FD_SET(0,&readfds);                                       
    FD_SET(sd,&readfds);

    memset(requete, 0, sizeof(requete));
    memset(requete2, 0, sizeof(requete2));
    memset(reponse, 0, sizeof(reponse));

    if(select(sd+1,&readfds,NULL,NULL,NULL) == -1){
    	perror("Erreur lors de l'appel a select");
    	exit(-1);
    }
                                      
   if(FD_ISSET(0, &readfds)){                                
        if(read(0, requete, 32767) == -1){                             
        	perror("Erreur lors de l'appel a read");
        	exit(-1);
      	}
        if(requete[0] != '/' && requete[1] != 't'){
          strcat(requete2, pseudo);
          strcat(requete2, " : ");
        }
        strcat(requete2, requete);
        if(send(sd, requete2, 32767, 0) == -1){                    
            perror("Erreur lors de l'appel a send");
            exit(-1);
        }                
    }                               
    if(FD_ISSET(sd, &readfds)){                           
        if(recv(sd, &reponse, 32767, 0) == -1){
            perror("Erreur lors de la reception");
            exit(-1);
        }
        printf("%s\n",reponse);          
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
