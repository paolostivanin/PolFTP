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
		perror("./server <numero porta>\n");
		exit(1);
	}
	int DescrittoreServer, DescrittoreClient, LunghezzaClient;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* indirizzo del server e del client */
	char Buffer[1024] = {};
	int rc, fd;
	off_t offset = 0;
	struct stat stat_buf;
	char filename[1024] = {};
	
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
		DescrittoreClient = accept(DescrittoreServer, (struct sockaddr *) &cli_addr, &LunghezzaClient);
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
    	if((strcmp(filename, "exit")) == 0){
    		printf("Esco dal server...\n");
    		strcpy(Buffer, "Server chiuso");
			send(DescrittoreClient, Buffer, strlen(Buffer), 0);
    		close(DescrittoreClient);
    		close(DescrittoreServer);
    		exit(0);
    	}

    	/* null terminate and strip any \r and \n from filename */
		filename[rc] = '\0';
    	if (filename[strlen(filename)-1] == '\n')
    		filename[strlen(filename)-1] = '\0';
    	if (filename[strlen(filename)-1] == '\r')
    		filename[strlen(filename)-1] = '\0';
    
    	fprintf(stderr, "Ricevuta richiesta di inviare il file: '%s'\n", filename);

    	/* open the file to be sent */
    	fd = open(filename, O_RDONLY);
   	 	if (fd == -1) {
    		fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    		exit(1);
    	}

    	/* get the size of the file to be sent */
    	fstat(fd, &stat_buf);

    	/* copy file using sendfile */
    	offset = 0;
    	rc = sendfile(DescrittoreClient, fd, &offset, stat_buf.st_size);
    	if (rc == -1) {
      		fprintf(stderr, "Errore durante l'invio di: '%s'\n", strerror(errno));
      		exit(1);
    	}
    	if (rc != stat_buf.st_size) {
      		fprintf(stderr, "Trasferimento incompleto: %d di %d bytes\n", rc, (int)stat_buf.st_size);
      		exit(1);
    	}
		char *ip_address = inet_ntoa(cli_addr.sin_addr); /* inet_ntoa converte un hostname in un ip */
		printf("IP del client: %s\n", ip_address);		

		close(DescrittoreClient);
		close(fd);
	}
	close(DescrittoreServer);
	return EXIT_SUCCESS;
}

/* bind assegna un indirizzo al socket.
-----------------------------------------------
INADDR_ANY è in indirizzo ip speciale che permette alla macchina di lavorare senza conoscere il proprio indirizzio IP
-----------------------------------------------
*/
