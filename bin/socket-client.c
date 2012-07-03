/* Descrizione: Semplice client FTP sviluppato per progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU GPL v3 <http://www.gnu.org/licenses/gpl-3.0.html>
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
#include <fcntl.h>
#include "prototypes.h"

void clear_var(int, int, size_t);

int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);

	int sockd, fd; /* descrittore del socket */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	struct sockaddr_in serv_addr; /* struttura contenente indirizzo del server */
	char *user = argv[3]; /* contiene nome utente */
	char *pass = argv[4]; /* contiene password */
	char *filename = NULL, *conferma = NULL; /* contiene nome del file, contiene la conferma di ricezione */
	static char filebuffer[1024], buffer[256], expected_string[256]; /* buffer usato per contenere vari dati */
	struct hostent *hp; /* la struttura hostent mi servirà per l'indirizzo ip del server */
	size_t fsize, nread = 0; /* fsize conterrà la grandezza del file e nread i bytes letti ogni volta del file */
	int total_bytes_read = 0; /* bytes totali letti del file */
	
	hp = gethostbyname(argv[1]); /* inseriamo nella struttura hp le informazione sull'host "argv[1]" */
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* la porta */
	serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr; /* memorizzo il tutto nella struttura serv_addr */

	//strcpy(buffer, filename); /* copio il nome del file nel buffer */

	if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Errore nella creazione della socket");
		exit(1);
	}

	if(connect(sockd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore nella connessione");
		close(sockd);
		exit(1);
	}
	/************************* MESSAGGIO DI BENVENUTO *************************/
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione del messaggio di benvenuto\n");
    	close(sockd);
    	exit(1);
    }
    printf("%s\n", buffer);
    clear_buf(buffer, NULL, NULL, 1);
	/************************* FINE MESSAGGIO DI BENVENUTO *************************/

    /************************* INIZIO PARTE LOGIN *************************/
	/************************* INVIO NOME UTENTE E RICEVO CONFERMA *************************/
	sprintf(buffer, "USER %s\n", user);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio di USER");
		close(sockd);
		exit(1);
	}
	clear_buf(buffer, NULL, NULL, 1);
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione della conferma USER");
    	close(sockd);
    	exit(1);
    }
    conferma = strtok(buffer, "\n");
    if(strcmp(conferma, "USEROK") != 0){
    	printf("Nome utente non ricevuto\n");
    	close(sockd);
    	exit(1);
    }
    clear_buf(buffer, NULL, conferma, 4);
    /************************* FINE NOME UTENTE *************************/
	
    /************************* INVIO PASSWORD E RICEVO CONFERMA *************************/
	sprintf(buffer, "PASS %s\n", pass);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio di PASS");
		close(sockd);
		exit(1);
	}
	clear_buf(buffer, NULL, NULL, 1);
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione della conferma PASS");
    	close(sockd);
    	exit(1);
    }
    conferma = strtok(buffer, "\n");
    if(strcmp(conferma, "PASSOK") != 0){
    	printf("Password non ricevuta\n");
    	close(sockd);
    	exit(1);
    }
    clear_buf(buffer, NULL, conferma, 4);
	/************************* FINE PASSWORD *************************/

	/************************* RICEZIONE CONFERMA LOG IN *************************/
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione della conferma LOG IN");
    	close(sockd);
    	exit(1);
    }
    conferma = strtok(buffer, "\n");
    sprintf(expected_string, "230 USER %s logged in", argv[3]);
    if(strcmp(conferma, expected_string) != 0){
    	printf("Login non effettuato\n");
    	close(sockd);
    	exit(1);
    } else{
    	printf("%s\n", conferma);
    }
    clear_buf(buffer, NULL, conferma, 4);
	/************************* FINE RICEZIONE CONFERMA LOG IN *************************/
	/************************* FINE PARTE LOGIN *************************/

	/************************* INVIO RICHIESTA FILE LISTING *************************/
	strcpy(buffer, "LIST\n");
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio richiesta LIST");
		close(sockd);
		exit(1);
	}
	if(recv(sockd, &fsize, sizeof(fsize), 0) < 0){
    	perror("Errore nella ricezione della grandezza del file");
    	close(sockd);
    	exit(1);
    }
    if((fd = open("listfiles.txt", O_CREAT | O_WRONLY,0644)) < 0){
    	perror("open file list");
    	close(sockd);
    	exit(1);
    }
    while((total_bytes_read != fsize) && ((nread = read(sockd, filebuffer, sizeof(filebuffer))) > 0)){
		if(write(fd, filebuffer, nread) < 0){
			perror("write");
			close(sockd);
			exit(1);
		}
		total_bytes_read += nread;
	}
	clear_buf(buffer, filebuffer, NULL, 2);
	close(fd);
	printf("----- FILE LIST -----\n");
	system("cat listfiles.txt");
	printf("----- END FILE LIST -----\n");
	/************************* FINE RICHIESTA FILE LISTING *************************/

	/************************* RICHIESTA CWD *************************/
	strcpy(buffer, "CWD\n");
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio richiesta CWD");
		close(sockd);
		exit(1);
	}
	clear_buf(buffer, NULL, NULL, 1);
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione CWD");
    	close(sockd);
    	exit(1);
    }
	conferma = strtok(buffer, "\n");
   	printf("%s\n", conferma);
    clear_buf(buffer, NULL, conferma, 4);
	/************************* FINE RICHIESTA CWD *************************/

	/************************* INVIO RICHIESTA CD *************************/

	/************************* FINE RICHIESTA CD *************************/


	exit(0);
}
	/*
	if(read(sockd, &fsize, sizeof(fsize)) < 0){
		perror("Errore durante ricezione grandezza file\n");
		close(sockd);
		exit(1);
	}

	fd = open(filename, O_CREAT | O_WRONLY,0644);
	if (fd  < 0) {
		perror("open");
		exit(1);
	}
	
	while(total_bytes_read < fsize){
		while ((nread = read(sockd, filebuffer, sizeof(filebuffer))) > 0){
			if(write(fd, buffer, nread) < 0){
				perror("write");
				close(sockd);
				exit(1);
			}
			total_bytes_read += nread;
		}
	}
	printf("File ricevuto\n");

	close(sockd);
	return EXIT_SUCCESS;
}*/

void check_before_start(int argc, char *argv[]){
	/* Controllo che vi siano argv[0], argv[1], argv[2], argv[3] e argv[4] */
	if(argc != 5){
		printf("Uso: %s <hostname> <numero porta> <nomeutente> <password>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}