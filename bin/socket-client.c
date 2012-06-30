/* Descrizione: Client FTP sviluppato come progetto per il corso di Reti di Calcolatori
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
#include <stdint.h>
#include <inttypes.h>

int main(int argc, char *argv[]){
	
	/* Controllo che vi sia argv[0], argv[1] e argv[2] */
	if(argc != 5){
		printf("Uso: ./client <hostname> <numero porta> <nomeutente> <password>\n");
		exit(1);
	}

	int DescrittoreClient, fd; /* descrittore del socket */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	//strcpy(buffer, filename); /* copio il nome del file nel buffer */
	struct sockaddr_in serv_addr; /* indirizzo del server */
	static char filebuffer[1024]; /* contiene i dati di invio e ricezione */
	char *user = argv[3]; /* contiene nome utente */
	char *pass = argv[4]; /* contiene password */
	char *filename = NULL, *conferma = NULL;
	static char buffer[256];
	struct hostent *hp; /* con la struttura hostent definisco l'hostname del server */
	size_t fsize, nread = 0;
	int total_bytes_read = 0;
	
	hp = gethostbyname(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* la porta */
	serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr; /* memorizzo il tutto nella struttura serv_addr */

	//strcpy(buffer, filename); /* copio il nome del file nel buffer */

	if((DescrittoreClient = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Errore nella creazione della socket");
		exit(1);
	}

	if(connect(DescrittoreClient, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore nella connessione");
		close(DescrittoreClient);
		exit(1);
	}
	/************************* MESSAGGIO DI BENVENUTO *************************/
	if(recv(DescrittoreClient, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione del messaggio di benvenuto\n");
    	close(DescrittoreClient);
    	exit(1);
    }
    printf("%s\n", buffer);
    memset(buffer, '0', sizeof(buffer));
	/************************* FINE MESSAGGIO DI BENVENUTO *************************/

	/************************* INVIO NOME UTENTE E RICEVO CONFERMA *************************/
	sprintf(buffer, "USER %s\n", user);
	if(send(DescrittoreClient, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio");
		close(DescrittoreClient);
		exit(1);
	}
	memset(buffer, '0', sizeof(buffer));
	if(recv(DescrittoreClient, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione della conferma");
    	close(DescrittoreClient);
    	exit(1);
    }
    conferma = strtok(buffer, "\n");
    if(strcmp(conferma, "USEROK") != 0){
    	printf("Nome utente non ricevuto\n");
    	close(DescrittoreClient);
    	exit(1);
    }
    memset(buffer, '0', sizeof(buffer));
    memset(conferma, '0', sizeof(conferma));
    /************************* FINE PARTE NOME UTENTE *************************/
	
    /************************* INVIO PASSWORD E RICEVO CONFERMA *************************/
	sprintf(buffer, "PASS %s\n", pass);
	if(send(DescrittoreClient, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio");
		close(DescrittoreClient);
		exit(1);
	}
	memset(buffer, '0', sizeof(buffer));
	if(recv(DescrittoreClient, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione della conferma");
    	close(DescrittoreClient);
    	exit(1);
    }
    conferma = strtok(buffer, "\n");
    if(strcmp(conferma, "PASSOK") != 0){
    	printf("Password non ricevuta\n");
    	close(DescrittoreClient);
    	exit(1);
    }
    memset(buffer, '0', sizeof(buffer));
    memset(conferma, '0', sizeof(conferma));
	/************************* FINE PARTE PASSWORD *************************/

	printf("230: User %s logged in\n", argv[3]);

	exit(0);

	if(read(DescrittoreClient, buffer, sizeof(buffer)) < 0){
		perror("Errore durante ricezione grandezza file\n");
		close(DescrittoreClient);
		exit(1);
	} else{
		
	}

	if(read(DescrittoreClient, &fsize, sizeof(fsize)) < 0){
		perror("Errore durante ricezione grandezza file\n");
		close(DescrittoreClient);
		exit(1);
	}

	fd = open(filename, O_CREAT | O_WRONLY,0644);
	if (fd  < 0) {
		perror("open");
		exit(1);
	}
	
	while(total_bytes_read < fsize){
		while ((nread = read(DescrittoreClient, filebuffer, sizeof(filebuffer))) > 0){
			if(write(fd, buffer, nread) < 0){
				perror("write");
				close(DescrittoreClient);
				exit(1);
			}
			total_bytes_read += nread;
		}
	}
	printf("File ricevuto\n");

	close(DescrittoreClient);
	return EXIT_SUCCESS;
}