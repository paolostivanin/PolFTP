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

void do_child(int sock);

int main(int argc, char *argv[]){
	if(argc != 2){
		perror("./server <numero porta>\n");
		exit(1);
	}
	pid_t pid;
	int DescrittoreServer, DescrittoreClient, LunghezzaClient;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* indirizzo del server e del client */
	DescrittoreServer = socket(AF_INET, SOCK_STREAM, 0);
	if(DescrittoreServer < 0){
		perror("Errore: creazione socket\n");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* porta */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */

	if(bind(DescrittoreServer, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore di bind\n");
		close(DescrittoreServer);
		exit(1);
	}
	listen(DescrittoreServer, 5);
	LunghezzaClient = sizeof(cli_addr);
	while(1){
		DescrittoreClient = accept(DescrittoreServer, (struct sockaddr *) &cli_addr, &LunghezzaClient);
		if(DescrittoreClient < 0){
			perror("Errore: non è possibile stabilire la connessione\n");
			close(DescrittoreServer);
			close(DescrittoreClient);
			exit(1);
		}
		pid = fork();
		if(pid != 0){
			perror("Fork error");
			close(DescrittoreServer);
			close(DescrittoreClient);
			exit(1);
		}
		if(pid == 0){
			close(DescrittoreServer);
			do_child(DescrittoreClient);
			_exit(0);
		}
		else{
			close(DescrittoreClient);
		}
	}
	return EXIT_SUCCESS;
}

void do_child(int sock){
	int rc, fd;
	off_t offset = 0;
	struct stat stat_buf;
	char filename[1024];

	/* get the file name from the client */
    rc = recv(sock, filename, sizeof(filename), 0);
    if (rc == -1) {
      fprintf(stderr, "recv failed: %s\n", strerror(errno));
      exit(1);
    }

    /* null terminate and strip any \r and \n from filename */
	filename[rc] = '\0';
    if (filename[strlen(filename)-1] == '\n')
    	filename[strlen(filename)-1] = '\0';
    if (filename[strlen(filename)-1] == '\r')
    	filename[strlen(filename)-1] = '\0';
    
    fprintf(stderr, "received request to send file %s\n", filename);

    /* open the file to be sent */
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
    	fprintf(stderr, "unable to open '%s': %s\n", filename, strerror(errno));
    	_exit(1);
    }

    /* get the size of the file to be sent */
    fstat(fd, &stat_buf);

    /* copy file using sendfile */
    offset = 0;
    rc = sendfile(sock, fd, &offset, stat_buf.st_size);
    if (rc == -1) {
      fprintf(stderr, "error from sendfile: %s\n", strerror(errno));
      _exit(1);
    }
    if (rc != stat_buf.st_size) {
      fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n", rc, (int)stat_buf.st_size);
      _exit(1);
    }
    close(fd);
    close(sock);
}

/*
Un processo UNIX è un programma in esecuzione che ha i seguenti attributi:
* le istruzioni del programma stesso o codice eseguibile ( text nel gergo di UNIX )
* la memoria destinata a contenere le variabili globali e tutto ciò che il programma alloca dinamicamente
* una tabella che tiene traccia dei file aperti
* una tabella per gestire i segnali del processo
* lo stack per le variabili locali e le informazioni necessarie alla chiamata ed al ritorno delle funzioni
* lo stato della CPU ( valore dei registri ed altro )
Queste parti sono scomposte per un semplice motivo, più processi che eseguono lo stesso programma non hanno bisogno di duplicare
la memoria necessaria, il text ad esempio si può condividere.
Ogni processo è identificato in modo univoco da un PID ( Process ID ), per visualizzarne la lista sul nostro sistema
basta eseguire il comando ps oppure dare un’occhiata nella directory /proc. Un processo può ottenere in ogni momento il suo PID
chiamando la funzione getpid().
Per creare un nuovo processo si usa la chiamata di sistema fork(), che genera una copia del processo che lo invoca
con alcune importanti considerazioni:
* il text non ha bisogno di essere duplicato, perché i due processi lo usano solo in lettura
* il PID del processo figlio è diverso da quello del padre
Linux fa uso della memoria copy-on-write e solo le porzioni di memoria modificate dal figlio sono duplicate,
tutto il resto punta a strutture del processo padre. La funzione fork() restituisce zero al figlio, mentre il PID del figlio al padre, 
per questo motivo padre e figlio intraprendono due strade diverse e possono compiere anche istruzioni diverse
*/
