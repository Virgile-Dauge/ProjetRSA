#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{
  int sockfd;                   // descripteur de socket
  struct sockaddr_in serveur;   // structure d'adresse qui contiendra les param reseaux du serveur
  fd_set readfds;               // ensemble des descripteurs en lecture qui seront surveilles par select
  char buf[1024];               // espace necessaire pour stocker le message recu
  char buf2[1024];              // espace necessaire pour envoyer un message au serveur
  
  memset(buf,'\0',1024);        // initialisation du buffer qui sera utilisé
  memset(buf2,'\0',1024);       // initialisation de l'autre buffer qui sera utilisé

  // verification du nombre d'arguments sur la ligne de commande
  if(argc != 3)
  {
    printf("Usage: %s @serveur port_serveur\n", argv[0]);
    exit(-1);
  }

  // creation de la socket
  sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

  // initialisation de la structure d'adresse du serveur :
  
  // famille d'adresse
  serveur.sin_family = AF_INET;

  // recuperation de l'adresse IPv4 du serveur
  inet_aton(argv[1], &(serveur.sin_addr));

  // recuperation du port du serveur
  serveur.sin_port = htons(atoi(argv[2]));

  printf("Tentative de connexion\n");

  // tentative de connexion
  if(connect(sockfd,(struct sockaddr*)&serveur,sizeof(serveur)) == -1)
  {
    perror("Erreur de connexion -> ");
    exit(2);
  }

  printf("Connexion etablie\n");

  while(1){
           memset(buf,'\0',1024);                                    // on reinitialise les buffers qui seront utilises
           memset(buf2,'\0',1024);
           
           FD_ZERO(&readfds);                                        // il faut remettre tt les elements ds readfds a chaque recommencement de la boucle, vu que select modifie les ensembles
           FD_SET(0,&readfds);                                       // on rajoute l'entree standard
           FD_SET(sockfd,&readfds);                                  // on rajoute la socket de communication avec le serveur
           
           if(select(sockfd+1,&readfds,NULL,NULL,NULL) == -1){
                                          perror("Erreur lors de l'appel a select -> ");
                                          exit(1);
                                          }
                                          
           if(FD_ISSET(0,&readfds)){                                 // si l'entree standard est dans readfds, alors l'utilisateur en en train de rédiger un message a envoyer
                                    if(read(0,buf2,1024) == -1){                             // on lit donc ce qui arrive sur l'entrée standard
                                              perror("Erreur lors de l'appel a read -> ");
                                              exit(1);
                                              }
                                    if(send(sockfd,buf2,1024,0) == -1){                      // puis on l'envoie au serveur
                                         perror("Erreur lors de l'appel a send -> ");
                                         exit(1);
                                         }                
                                   }                               
           if(FD_ISSET(sockfd,&readfds)){                            // si la socket de communication est dans readfds, alors le serveur nous a envoye un message
                                         if(recv(sockfd,&buf,1024,0) == -1){
                                                                   perror("Erreur lors de la reception -> ");
                                                                   exit(4);
                                                                   }
                                         printf("La chaine recue est: %s\n",buf);            // on l'affiche
                                        }
           }