/* Descrizione: Semplice client FTP sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU GPL v3 <http://www.gnu.org/licenses/gpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
#include <termios.h> /* per nascondere la password */
#include "prototypes.h"

#define BUFFGETS 255

int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);

	int sockd, fd, total_bytes_read = 0, var = 0, i = 0; /* descrittore del socket, file, bytes letti alla ricezione del file in totale */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	static struct sockaddr_in serv_addr; /* struttura contenente indirizzo del server */
	char *user = NULL, *pass = NULL, *filename = NULL, *conferma = NULL, *filebuffer = NULL, *scelta = NULL;
	static char buffer[256], expected_string[128], dirpath[256]; /*buffer usato per contenere vari dati */
	static struct hostent *hp; /* la struttura hostent mi servirà per l'indirizzo ip del server */
	uint32_t fsize, nread = 0, fsize_tmp; /* fsize conterrà la grandezza del file e nread i bytes letti ogni volta del file */
	static struct termios oldt, newt;
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
	/* salvo i settaggi attuali di STDIN_FILENO ed assegno a newt i valore di oldt*/
	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;

	printf("User: ");
	/* Which will read everything up to the newline into the string you pass in, then will consume
	 * a single character (the newline) without assigning it to anything (that '*' is 'assignment suppression'). */
	if(scanf("%m[^\n]%*c", &user) == EOF){
		perror("scanf user");
		onexit(sockd, 0, 0, 1);
	}
	printf("Pass: ");
    /* imposto il bit appropriato nella struttura newt */
    newt.c_lflag &= ~(ECHO); 
    /* imposto il nuovo bit nell'attuale STDIN_FILENO */
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	if(scanf("%m[^\n]%*c", &pass) == EOF){
		perror("scanf user");
		onexit(sockd, 0, 0, 1);
	}
  /* resetto con oldt l'attuale STDIN_FILENO*/ 
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
  printf("\n");
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
   	perror("Errore nella ricezione dellaa conferma LOG IN");
   	onexit(sockd, 0, 0, 1);
  }
  conferma = strtok(buffer, "\n");
  sprintf(expected_string, "230 USER %s logged in", user);
  if(strcmp(conferma, expected_string) != 0){
   	printf("Login non effettuato\n");
   	onexit(sockd, 0, 0, 1);
  } else{
   	printf("%s\n", conferma);
  }
  memset(buffer, 0, sizeof(buffer));
  free(user);
  free(pass);
	/************************* FINE RICEZIONE CONFERMA LOG IN *************************/
	/************************* FINE PARTE LOGIN *************************/

	/************************* SCELTA AZIONE, INVIO AZIONE, RICEZIONE CONFERMA, ESECUZIONE AZIONE *************************/
  exec_switch:
  printf("\nFTPUtils:~$ ");
  if(scanf("%m[^\n]%*c", &scelta) < 1){
   	perror("Errore scanf scelta");
   	onexit(sockd, 0, 0, 1);
  }
  printf("\n");
  for(i=0; i<(int)strlen(scelta); i++){
    if(isalpha((unsigned char)scelta[i])){
      if(islower((unsigned char)scelta[i])){
          scelta[i] = toupper((unsigned char)scelta[i]);
      }
    }
    else{
      printf("%c non è un carattere\n", scelta[i]);
      goto exec_switch;
    }
  }
  if((strcmp("SYST", scelta) == 0)) goto prepara;
  if((strcmp("LIST", scelta) == 0)) goto prepara;
  if((strcmp("PWD", scelta) == 0)) goto prepara;
  if((strcmp("CWD", scelta) == 0)) goto prepara;
  if((strcmp("RETR", scelta) == 0)) goto prepara;
  if((strcmp("EXIT", scelta) == 0)) goto prepara;
  if((strcmp("HELP", scelta) == 0)) goto prepara;
  printf("Comando errato. Scrivere HELP per l'aiuto.\n"); goto exec_switch;

  prepara:
  if(strcmp(scelta, "HELP") == 0) goto exec_help;
  strcpy(buffer, scelta);
  var = strlen(buffer);
  if(send(sockd, &var, sizeof(var), 0) < 0){
    perror("Errore durante l'invio lunghezza azione");
    onexit(sockd, 0, 0, 1);
  }
  if(send(sockd, buffer, var, 0) < 0){
    perror("Errore durante l'invio azione");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, &var, sizeof(var), 0) < 0){
    perror("Errore durante la ricezione conferma azione");
    onexit(sockd, 0, 0, 1);
  }
  
  if(strcmp(scelta, "SYST") == 0) goto exec_syst;
  if(strcmp(scelta, "LIST") == 0) goto exec_list;
  if(strcmp(scelta, "PWD") == 0) goto exec_pwd;
  if(strcmp(scelta, "CWD") == 0) goto exec_cwd;
  if(strcmp(scelta, "RETR") == 0) goto exec_retr;
  if(strcmp(scelta, "EXIT") == 0) goto exec_exit;

  exec_help:
  printf("I comandi disponibili sono:\nSYST - LIST - PWD - CWD - RETR - EXIT - HELP\n");
  goto exec_switch;
  /************************* FINE PARTE AZIONE UTENTE *************************/

	/************************* RICHIESTA SYST *************************/
	exec_syst:
  strcpy(buffer, "SYST\n");
  if(send(sockd, buffer, strlen(buffer), 0) < 0){
  	perror("Errore durante l'invio richiesta SYST");
  	onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
  	perror("Errore durante la ricezione risposta SYST");
  	onexit(sockd, 0, 0, 1);
  }
  conferma = strtok(buffer, "\n");
  printf("SYST type: %s\n", conferma);
  memset(buffer, 0, sizeof(buffer));
  goto exec_switch;
	/************************* FINE SYST *************************/

	/************************* INVIO RICHIESTA FILE LISTING *************************/
	exec_list:
	strcpy(buffer, "LIST\n");
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio richiesta LIST");
		onexit(sockd, 0, 0, 1);;
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
  filebuffer = malloc(fsize);
  if(filebuffer == NULL){
   	perror("malloc");
   	onexit(sockd, 0, fd, 4);
  }
  while(((uint32_t)total_bytes_read != fsize) && ((nread = read(sockd, filebuffer, fsize)) > 0)){
    if(write(fd, filebuffer, nread) != nread){
      perror("write");
		  close(sockd);
		  exit(1);
    }
	total_bytes_read += nread;
	}
	memset(buffer, 0, sizeof(buffer));
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
  free(filebuffer);
  goto exec_switch;
	/************************* FINE RICHIESTA FILE LISTING *************************/

	/************************* RICHIESTA PWD *************************/
	exec_pwd:
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
  goto exec_switch;
	/************************* FINE RICHIESTA PWD *************************/

	/************************* INVIO RICHIESTA CWD *************************/
	exec_cwd:
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
  goto exec_switch;
	/************************* FINE RICHIESTA CWD *************************/

	/************************* INVIO NOME FILE E RICEZIONE FILE *************************/
	exec_retr:
	printf("Inserire il nome del file da scaricare: ");
	if(fgets(dirpath, BUFFGETS, stdin) == NULL){
		perror("fgets nome file");
		onexit(sockd, 0 ,0 ,1);
	}
	filename = strtok(dirpath, "\n");
	sprintf(buffer, "RETR %s", dirpath);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio del nome del file");
		onexit(sockd, 0, 0, 1);
	}
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    perror("Errore ricezione conferma file");
    onexit(sockd, 0 ,0 ,1);
  }
  conferma = strtok(buffer, "\0");
  if(strcmp(conferma, "ERRORE: File non esistente") == 0){
    printf("ERRORE: il file richiesto non esiste\n");
    onexit(sockd, 0, 0, 1);
  }
	if(read(sockd, &fsize, sizeof(fsize)) < 0){
		perror("Errore durante ricezione grandezza file\n");
		onexit(sockd, 0 ,0 ,1);
	}
	fd = open(filename, O_CREAT | O_WRONLY, 0644);
	if (fd  < 0) {
		perror("open");
		onexit(sockd, 0 ,0 ,1);
	}
	fsize_tmp = fsize;
	filebuffer = malloc(fsize);
  if(filebuffer == NULL){
   	perror("malloc");
   	onexit(sockd, 0, fd, 4);
  }
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
	if(recv(sockd, buffer, 33, 0) < 0){
    perror("Errore ricezione 226");
    close(sockd);
    exit(1);
  }
  printf("%s", buffer);
  memset(buffer, 0, sizeof(buffer));
  free(filebuffer);
  close(fd);
  goto exec_switch;
	/************************* FINE INVIO NOME FILE E RICEZIONE FILE *************************/

  /************************* SALUTO FINALE *************************/
	exec_exit:
  if(recv(sockd, buffer, 12, 0) < 0){
    perror("Errore ricezione 221");
    close(sockd);
    exit(1);
  }
  printf("%s", buffer);
  close(sockd);
  /************************* SALUTO FINALE *************************/

	return EXIT_SUCCESS;
}


void check_before_start(int argc, char *argv[]){
	if(argc != 3){
		printf("Uso: %s <hostname> <numero porta>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}