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
	if(argc != 4){
		printf("Uso: ./client <hostname> <numero porta> <nomefile>\n");
		exit(1);
	}

	int DescrittoreClient, fd; /* descrittore del socket */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	struct sockaddr_in serv_addr; /* indirizzo del server */
	char Buffer[1024] = {}; /* contiene i dati di invio e ricezione */
	struct hostent *hp; /* con la struttura hostent definisco l'hostname del server */
	char *filename = argv[3];
	size_t fsize, nread = 0;
	int total_bytes_read = 0;
	
	hp = gethostbyname(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* la porta */
	serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr; /* memorizzo il tutto nella struttura serv_addr */
	
	if((DescrittoreClient = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Errore nella creazione della socket");
		exit(1);
	}

	if(connect(DescrittoreClient, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore nella connessione");
		close(DescrittoreClient);
		exit(1);
	}

	strcpy(Buffer, filename);

	if(send(DescrittoreClient, Buffer, strlen(Buffer), 0) < 0){
		perror("Errore durante l'invio")
		close(DescrizioneClient);
		exit(1);
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
		while ((nread = read(DescrittoreClient, Buffer, sizeof(Buffer))) > 0){
			if(write(fd, Buffer, nread) < 0){
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
