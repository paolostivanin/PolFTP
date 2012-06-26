/* Descrizione: GUI del Client FTP sviluppato come progetto per il corso di Reti di Calcolatori
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
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Uso: ./server <numero porta>\n");
		exit(1);
	}
	int DescrittoreServer, DescrittoreClient, LunghezzaClient;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* indirizzo del server e del client */
	//char Buffer[1024] = {};
	int rc, fd, ret_val;
	off_t offset = 0;
	struct stat stat_buf;
	char filename[1024] = {};
	size_t fsize;
	
	DescrittoreServer = socket(AF_INET, SOCK_STREAM, 0);
	if(DescrittoreServer < 0){
		perror("Errore creazione socket\n");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* porta htons converte nell'ordine dei byte di rete */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */
	
	/* int bind(int descrittore_socket, struct sockaddr* indirizzo, int lunghezza_record_indirizzo) */
	if(bind(DescrittoreServer, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore di bind\n");
		close(DescrittoreServer);
		exit(1);
	}
	/* int listen (int descrittore_socket, int dimensione_coda) */
	listen(DescrittoreServer, 5);
	LunghezzaClient = sizeof(cli_addr);
	while(1){
		/* int accept(int descrittore_socket, struct sockaddr* indirizzo, int* lunghezza_record_indirizzo) */
		DescrittoreClient = accept(DescrittoreServer, (struct sockaddr *) &cli_addr, (socklen_t *) &LunghezzaClient);
		if(DescrittoreClient < 0){
			perror("Errore: non è possibile stabilire la connessione\n");
			close(DescrittoreServer);
			close(DescrittoreClient);
			exit(1);
		}

		/* get the file name from the client */
    	rc = recv(DescrittoreClient, filename, sizeof(filename), 0);
    	if (rc == -1) {
      		fprintf(stderr, "recv failed: %s\n", strerror(errno));
      		exit(1);
    	}

		/* Terminiamo il nome del file con NULL e se ultimo carattere è \n o \r lo cambiamo con \0*/
		filename[rc] = '\0';
    	if (filename[strlen(filename)-1] == '\n')
    		filename[strlen(filename)-1] = '\0';
    	if (filename[strlen(filename)-1] == '\r')
    		filename[strlen(filename)-1] = '\0';

    	/* inet_ntoa converte un hostname in un ip decimale puntato */
    	fprintf(stderr, "Ricevuta richiesta di inviare il file '%s' dall' indirizzo %s\n", filename, inet_ntoa(cli_addr.sin_addr));

    	/* open the file to be sent */
    	fd = open(filename, O_RDONLY);
   	 	if (fd == -1) {
    		fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    		exit(1);
    	}

    	/* get the size of the file to be sent */
    	fstat(fd, &stat_buf);
    	fsize = stat_buf.st_size;
    	ret_val = send(DescrittoreClient, &fsize, sizeof(fsize), 0);
    	if(ret_val == -1){
    		printf("Errore durante l'invio della grandezza del file\n");
    		close(DescrittoreClient);
			close(fd);
			close(DescrittoreServer);
    		exit(1);
    	}
    	/* copy file using sendfile */
    	offset = 0;
   		rc = sendfile(DescrittoreClient, fd, &offset, stat_buf.st_size);
    	if (rc == -1) {
      		fprintf(stderr, "Errore durante l'invio di: '%s'\n", strerror(errno));
      		exit(1);
    	}
    	if (rc != fsize) {
      		fprintf(stderr, "Trasferimento incompleto: %d di %d bytes\n", rc, (int)stat_buf.st_size);
      		exit(1);
    	}

		close(DescrittoreClient);
		close(fd);
	}
	close(DescrittoreServer);
	return EXIT_SUCCESS;
}
