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
#include <stdint.h> /* per usare uint32_t invece di size_t */
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include "prototypes.h"

#define BUFFGETS 256

int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);

	int sockd, fd, total_bytes_read = 0; /* descrittore del socket, file, bytes letti alla ricezione del file in totale */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	struct sockaddr_in serv_addr; /* struttura contenente indirizzo del server */
	char *user = argv[3]; /* contiene nome utente */
	char *pass = argv[4]; /* contiene password */
	char *filename = NULL, *conferma = NULL; /* contiene nome del file, contiene la conferma di ricezione */
	static char filebuffer[1048576], buffer[256], expected_string[128], dirpath[BUFFGETS]; /* buffer usato per contenere vari dati */
	struct hostent *hp; /* la struttura hostent mi servirà per l'indirizzo ip del server */
	uint32_t fsize, nread = 0, fsize_tmp; /* fsize conterrà la grandezza del file e nread i bytes letti ogni volta del file */
	FILE *fp; /* file usato per leggere listfiles.txt */
	char c; /* usato per printare il file list 1 carattere per volta */
	
	hp = gethostbyname(argv[1]); /* inseriamo nella struttura hp le informazione sull'host "argv[1]" */
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* la porta */
	serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr; /* memorizzo il tutto nella struttura serv_addr */

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
    memset(buffer, 0, sizeof(buffer));
	/************************* FINE MESSAGGIO DI BENVENUTO *************************/

    /************************* INIZIO PARTE LOGIN *************************/
	/************************* INVIO NOME UTENTE E RICEVO CONFERMA *************************/
	sprintf(buffer, "USER %s\n", user);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio di USER");
		close(sockd);
		exit(1);
	}
	memset(buffer, 0, sizeof(buffer));
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
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE NOME UTENTE *************************/
	
    /************************* INVIO PASSWORD E RICEVO CONFERMA *************************/
	sprintf(buffer, "PASS %s\n", pass);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio di PASS");
		close(sockd);
		exit(1);
	}
	memset(buffer, 0, sizeof(buffer));
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
    memset(buffer, 0, sizeof(buffer));
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
    memset(buffer, 0, sizeof(buffer));
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
    fsize_tmp = fsize;
    while(((uint32_t)total_bytes_read != fsize) && ((nread = read(sockd, filebuffer, fsize)) > 0)){
		if(write(fd, filebuffer, nread) != nread){
			perror("write");
			close(sockd);
			exit(1);
		}
		total_bytes_read += nread;
	}
	memset(buffer, 0, sizeof(buffer));
	memset(filebuffer, 0, sizeof(filebuffer));
	close(fd);
	printf("\n----- FILE LIST -----\n");
	if((fp=fopen("listfiles.txt", "r+")) == NULL){
		perror("open file for read");
		close(sockd);
		exit(EXIT_FAILURE);
	}
	while((c=getc(fp)) != EOF){
		putchar(c);
	}
	printf("----- END FILE LIST -----\n");
    if(remove( "listfiles.txt" ) == -1 ){
      perror("errore cancellazione file");
      close(sockd);
      exit(EXIT_FAILURE);
    }
	/************************* FINE RICHIESTA FILE LISTING *************************/

	/************************* RICHIESTA PWD *************************/
	strcpy(buffer, "PWD\n");
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio richiesta PWD");
		close(sockd);
		exit(1);
	}
	memset(buffer, 0, sizeof(buffer));
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore ricezione PWD");
    	close(sockd);
    	exit(1);
    }
	conferma = strtok(buffer, "\n");
   	printf("%s\n", conferma);
    memset(buffer, 0, sizeof(buffer));
	/************************* FINE RICHIESTA PWD *************************/

	/************************* INVIO RICHIESTA CWD *************************/
	printf("Inserire percorso: ");
	if(fgets(dirpath, BUFFGETS, stdin) == NULL){
		perror("fgets dir path");
		close(sockd);
	}
	sprintf(buffer, "CWD %s", dirpath);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio richiesta CWD");
		close(sockd);
		exit(1);
	}
	memset(buffer, 0, sizeof(buffer));
	if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore ricezione CWD");
    	close(sockd);
    	exit(1);
    }
	conferma = strtok(buffer, "\0");
   	printf("%s", conferma);
    memset(buffer, 0, sizeof(buffer));
    memset(dirpath, 0, sizeof(dirpath));
	/************************* FINE RICHIESTA CWD *************************/

	/************************* INVIO NOME FILE E RICEZIONE FILE *************************/
	printf("Inserire il nome del file da scaricare: ");
	if(fgets(dirpath, BUFFGETS, stdin) == NULL){
		perror("fgets nome file");
		close(sockd);
	}
	filename = strtok(dirpath, "\n");
	sprintf(buffer, "RETR %s", dirpath);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio del nome del file");
		close(sockd);
		exit(1);
	}
	if(read(sockd, &fsize, sizeof(fsize)) < 0){
		perror("Errore durante ricezione grandezza file\n");
		close(sockd);
		exit(1);
	}
	fd = open(filename, O_CREAT | O_WRONLY, 0644);
	if (fd  < 0) {
		perror("open");
		exit(1);
	}
	fsize_tmp = fsize;
    while(((uint32_t)total_bytes_read != fsize) && ((nread = read(sockd, filebuffer, fsize_tmp)) > 0)){
		if(write(fd, filebuffer, nread) != nread){
			perror("write RETR");
			close(sockd);
			exit(1);
		}
		total_bytes_read += nread;
		fsize_tmp -= nread;
	}
	memset(buffer, 0, sizeof(buffer));
	if(recv(sockd, buffer, 34, 0) < 0){
    	perror("Errore ricezione 226");
    	close(sockd);
    	exit(1);
    }
    printf("%s", buffer);
    memset(buffer, 0, sizeof(buffer));
	if(recv(sockd, buffer, 13, 0) < 0){
    	perror("Errore ricezione 221");
    	close(sockd);
    	exit(1);
    }
    printf("%s", buffer);
    memset(buffer, 0, sizeof(buffer));
    close(fd);
	/************************* FINE INVIO NOME FILE E RICEZIONE FILE *************************/

	close(sockd);
	return EXIT_SUCCESS;
}


void check_before_start(int argc, char *argv[]){
	if(argc != 5){
		printf("Uso: %s <hostname> <numero porta> <nomeutente> <password>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}