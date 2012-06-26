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
#include <signal.h>

void sig_handler(int signo, int DescrittoreServer, int DescrittoreClient, int fd){
	if (signo == SIGINT){
		printf("Received SIGINT, exiting...\n");
    	close(DescrittoreClient);
		close(fd);
		close(DescrittoreServer);
		exit(1);
    }
}

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Uso: ./server <numero porta>\n");
		exit(1);
	}
	int DescrittoreServer, DescrittoreClient, LunghezzaClient;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* indirizzo del server e del client */
	int rc, fd;
	off_t offset = 0;
	struct stat stat_buf;
	char filename[1024] = {};
	size_t fsize;
	
	if((DescrittoreServer = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Errore creazione socket\n");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* porta htons converte nell'ordine dei byte di rete */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */
	
	if(bind(DescrittoreServer, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore di bind\n");
		close(DescrittoreServer);
		exit(1);
	}

	if(listen(DescrittoreServer, 5) < 0){
			perror("Errore nella funzione listen");
    		close(DescrittoreServer);
      		exit(1);
	}
	LunghezzaClient = sizeof(cli_addr);
	signal (SIGINT, ( void *)sig_handler); 
	while(1){
		if((DescrittoreClient = accept(DescrittoreServer, (struct sockaddr *) &cli_addr, (socklen_t *) &LunghezzaClient)) < 0){
			perror("Errore nella connessione\n");
			close(DescrittoreServer);
			close(DescrittoreClient);
			exit(1);
		}

		/* get the file name from the client */
    	if((rc = recv(DescrittoreClient, filename, sizeof(filename), 0)) < 0){
    		perror("Errore nella ricezione del nome del file");
    		close(DescrittoreClient);
    		close(DescrittoreServer);
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
   	 	if (fd < 0) {
    		fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    		exit(1);
    	}

    	/* get the size of the file to be sent */
    	if(fstat(fd, &stat_buf) < 0){
    		perror("Errore fstat");
    		close(DescrittoreClient);
    		close(fd);
    		close(DescrittoreServer);
      		exit(1);
    	}
    	fsize = stat_buf.st_size;
    	if(send(DescrittoreClient, &fsize, sizeof(fsize), 0) < 0){
    		perror("Errore durante l'invio della grandezza del file\n");
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