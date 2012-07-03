/* Descrizione: Semplice server FTP iterativo sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include "prototypes.h"


int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);
	
	int sockd, newsockd, socket_len, rc, rc_list, fd, fpl;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* strutture contenenti indirizzo del server e del client */
	off_t offset = 0, offset_list = 0; /* variabile di tipo offset */
	struct stat fileStat; /* struttura contenente informazioni sul file scelto */
	struct hostent *local_ip; /* struttura contenente ip server */
	static char filename[1024], buffer[256], saved_user[256]; /* dichiaro static così viene direttamente inizializzato a 0 l'array */
	char *user_string = NULL, *username = NULL, *pass_string = NULL, *password = NULL, *other = NULL; /* puntatori per uso vario */
	size_t fsize, count, i; /* grandezza file */
  char **files;
  FILE *fp_list;
	
	if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Errore creazione socket\n");
		exit(EXIT_FAILURE);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* porta htons converte nell'ordine dei byte di rete */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */
	
	if(bind(sockd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore di bind\n");
		onexit(0, sockd, 0, 1);
	}

	if(listen(sockd, 5) < 0){
			perror("Errore nella funzione listen");
    		onexit(0, sockd, 0, 1);
	}
	socket_len = sizeof(cli_addr);
	
	if((local_ip = gethostbyname("localhost")) == NULL){
		perror("gethostbyname()");
		exit(1);
	}	
	
	signal (SIGINT, ( void *)sig_handler); /* se premuto CTRL+C il server termina */
	
	while(1){
		if((newsockd = accept(sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
			perror("Errore nella connessione\n");
			onexit(newsockd, sockd, 0, 2);
		}

    /* inet_ntoa converte un hostname in un ip decimale puntato */
    fprintf(stdout, "Ricevuta richiesta di connessione dall' indirizzo %s\n", inet_ntoa(cli_addr.sin_addr));

    /************************* MESSAGGIO DI BENVENUTO *************************/
    sprintf(buffer, "220 FTPUtils SERVER [%s]", local_ip->h_name);
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
			onexit(newsockd, sockd, 0, 2);
		}
		clear_buf(buffer, NULL, NULL, 1);
    /************************* FINE MESSAGGIO DI BENVENUTO *************************/

		/************************* INIZIO PARTE LOGIN *************************/
    /************************* RICEVIAMO NOME UTENTE E MANDIAMO CONFERMA *************************/
		 if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione del nome utente");
    	onexit(newsockd, sockd, 0, 2);
    }    	
    user_string = strtok(buffer, " ");
    username = strtok(NULL, "\n");
    fprintf(stdout, "%s %s\n", user_string, username);
    sprintf(saved_user, "%s", username);
    clear_buf(buffer, NULL, NULL, 1);
    strcpy(buffer, "USEROK\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
		  onexit(newsockd, sockd, 0, 2);
		}
		clear_buf(buffer, NULL, NULL, 1);
    /************************* FINE NOME UTENTE *************************/

    /************************* RICEVIAMO PASSWORD E MANDIAMO CONFERMA *************************/
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione del nome utente");
    	onexit(newsockd, sockd, 0, 2);
    }
    pass_string = strtok(buffer, " ");
    password = strtok(NULL, "\n");
    fprintf(stdout, "%s %s\n", pass_string, password);
    clear_buf(buffer, NULL, NULL, 1);
   	strcpy(buffer, "PASSOK\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
		  onexit(newsockd, sockd, 0, 2);
		}
		clear_buf(buffer, NULL, NULL, 1);
    /************************* FINE PASSWORD *************************/
    	
    /************************* INVIO CONFERMA LOG IN *************************/
    sprintf(buffer, "230 USER %s logged in\n", saved_user);
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
			perror("Errore durante l'invio");
			onexit(newsockd, sockd, 0, 2);
		}
		clear_buf(buffer, NULL, NULL, 1);
		/************************* FINE CONFERMA LOG IN *************************/
    /************************* FINE PARTE LOGIN *************************/

    /************************* RICEZIONE RICHIESTA LIST E INVIO LISTA *************************/
    if(recv(newsockd, buffer, 6, 0) < 0){
    	perror("Errore nella ricezione comando LIST");
    	onexit(newsockd, sockd, 0, 2);
    }
    other = strtok(buffer, "\n");
    if(strcmp(other, "LIST") == 0){
    	printf("Ricevuta richiesta LIST\n");
    } else onexit(newsockd, sockd, 0, 2);
    
    count = file_list("./", &files);
    if((fp_list = fopen("listfiles.txt", "w")) == NULL){
      perror("Impossibile aprire il file per la scrittura LIST");
      exit(EXIT_FAILURE);
    }
    for(i=0; i < count; i++){
      if(strcmp(files[i], "DIR ..") == 0 || strcmp(files[i], "DIR .") == 0) continue;
      else{
        fprintf(fp_list, "%s\n", files[i]);
      }
    }
    fclose(fp_list);
    if((fpl = open("listfiles.txt", O_RDONLY)) < 0){
      perror("open file with open");
      fclose(fp_list);
      onexit(newsockd, sockd, 0, 2);
      exit(1);
    }
    if(fstat(fpl, &fileStat) < 0){
      perror("Errore fstat");
      onexit(newsockd, sockd, fpl, 3);
    }
    fsize = fileStat.st_size;
    printf("File size: %zu\n", fsize);
    if(send(newsockd, &fsize, sizeof(fsize), 0) < 0){
      perror("Errore durante l'invio grande file list");
      onexit(newsockd, sockd, fpl, 3);
    }
    rc_list = sendfile(newsockd, fpl, &offset_list, fileStat.st_size);
    if(rc_list == -1){
      perror("Invio file list non riuscito");
      fclose(fp_list);
      onexit(newsockd, sockd, fpl, 3);
    }
    if((size_t)rc_list != fsize){
      fprintf(stderr, "Trasferimento incompleto: %d di %d bytes\n", rc_list, (int)fileStat.st_size);
      onexit(newsockd, sockd, fpl, 3);
    }
    fsize = 0;
    printf("File inviato\n");
		clear_buf(buffer, NULL, NULL, 1);
    /************************* FINE RICEZIONE LIST E INVIO LISTA *************************/

    onexit(newsockd, sockd, fpl, 3);

    	/* open the file to be sent
    	fd = open(filename, O_RDONLY);
   	 	if (fd < 0) {
    		fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    		onexit(newsockd, sockd, 0, 2);
    	}

		/* get the file name from the client
    	if((rc = recv(newsockd, filename, sizeof(filename), 0)) < 0){
    		perror("Errore nella ricezione del nome del file");
    		onexit(newsockd, sockd, 0, 2);
    	}

		/* Terminiamo il nome del file con NULL e se ultimo carattere è \n o \r lo cambiamo con \0
		filename[rc] = '\0';
    	if (filename[strlen(filename)-1] == '\n') filename[strlen(filename)-1] = '\0';
    	if (filename[strlen(filename)-1] == '\r') filename[strlen(filename)-1] = '\0';

    	/* get the size of the file to be sent
    	if(fstat(fd, &fileStat) < 0){
    		perror("Errore fstat");
    		onexit(newsockd, sockd, fd, 3);
    	}
    	fsize = fileStat.st_size;
    	if(send(newsockd, &fsize, sizeof(fsize), 0) < 0){
    		perror("Errore durante l'invio della grandezza del file\n");
    		onexit(newsockd, sockd, fd, 3);
    	}
    	/* copy file using sendfile
    	offset = 0;
   		rc = sendfile(newsockd, fd, &offset, fileStat.st_size);
    	if (rc == -1) {
      		fprintf(stderr, "Errore durante l'invio di: '%s'\n", strerror(errno));
      		onexit(newsockd, sockd, fd, 3);
    	}
    	if (rc != fsize) {
      		fprintf(stderr, "Trasferimento incompleto: %d di %d bytes\n", rc, (int)fileStat.st_size);
      		onexit(newsockd, sockd, fd, 3);
    	} */

		onexit(newsockd, 0, fd, 4);
	}
	close(sockd);
	exit(EXIT_SUCCESS);
}

void check_before_start(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stdout, "Uso: %s <numero porta>\n", argv[0]);
		exit(1);
	}
}

void sig_handler(int signo, int sockd, int newsockd, int file){
  if (signo == SIGINT){
    printf("Ricevuto SIGINT, esco...\n");
    close(newsockd);
    close(sockd);
    close(file);
    exit(EXIT_SUCCESS);
    }
}