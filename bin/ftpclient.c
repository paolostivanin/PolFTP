/* Descrizione: Semplice client FTP sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.0-alpha3
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

struct info{
  char *user, *pass, *filename, *conferma, *filebuffer, *scelta;
};

/* ricordarsi malloc e free per i membri della struttura a cui accedo direttamente con sInfo.user, sInfo.pass, ecc */

int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);

	int sockd, fd, total_bytes_read = 0, var = 0, i = 0; /* descrittore del socket, file, bytes letti alla ricezione del file in totale */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	static struct sockaddr_in serv_addr; /* struttura contenente indirizzo del server */
  static struct termios oldt, newt; /* struttura contenente i paramentri della shell */
  static struct hostent *hp; /* la struttura hostent mi servirà per l'indirizzo ip del server */
  static struct info sInfo;
	static char buffer[256], expected_string[128], dirpath[256], tmp_buf[BUFSIZ]; /*buffer usato per contenere vari dati */
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
  memset(buffer, 0, sizeof(buffer));
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
	if(scanf("%m[^\n]%*c", &sInfo.user) == EOF){
		perror("scanf user");
		onexit(sockd, 0, 0, 1);
	}
	printf("Pass: ");
  /* imposto il bit appropriato nella struttura newt */
  newt.c_lflag &= ~(ECHO); 
  /* imposto il nuovo bit nell'attuale STDIN_FILENO */
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	if(scanf("%m[^\n]%*c", &sInfo.pass) == EOF){
		perror("scanf user");
		onexit(sockd, 0, 0, 1);
	}
  /* resetto con oldt l'attuale STDIN_FILENO*/ 
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
  printf("\n");
	sprintf(buffer, "USER %s\n", sInfo.user);
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
  sInfo.conferma = strtok(buffer, "\n");
  if(strcmp(sInfo.conferma, "USEROK") != 0){
   	printf("Nome utente non ricevuto\n");
   	close(sockd);
  	exit(1);
  }
  memset(buffer, 0, sizeof(buffer));
  /************************* FINE NOME UTENTE *************************/
	
  /************************* INVIO PASSWORD E RICEVO CONFERMA *************************/
	sprintf(buffer, "PASS %s\n", sInfo.pass);
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
  sInfo.conferma = strtok(buffer, "\n");
  if(strcmp(sInfo.conferma, "PASSOK") != 0){
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
  sInfo.conferma = strtok(buffer, "\n");
  sprintf(expected_string, "230 USER %s logged in", sInfo.user);
  if(strcmp(sInfo.conferma, expected_string) != 0){
   	printf("Login non effettuato\n");
   	onexit(sockd, 0, 0, 1);
  } else{
   	printf("%s\n", sInfo.conferma);
  }
  memset(buffer, 0, sizeof(buffer));
  free(sInfo.user);
  free(sInfo.pass);
	/************************* FINE RICEZIONE CONFERMA LOG IN *************************/
	/************************* FINE PARTE LOGIN *************************/

	/************************* SCELTA AZIONE, INVIO AZIONE, RICEZIONE CONFERMA, ESECUZIONE AZIONE *************************/
  exec_switch:
  printf("\nFTPUtils:~$ ");
  if(scanf("%m[^\n]%*c", &sInfo.scelta) < 1){
   	perror("Errore scanf scelta");
   	onexit(sockd, 0, 0, 1);
  }
  printf("\n");
  for(i=0; i<(int)strlen(sInfo.scelta); i++){
    if(isalpha((unsigned char)sInfo.scelta[i])){
      if(islower((unsigned char)sInfo.scelta[i])){
          sInfo.scelta[i] = toupper((unsigned char)sInfo.scelta[i]);
      }
    }
    else{
      printf("%c non è un carattere\n", sInfo.scelta[i]);
      free(sInfo.scelta);
      goto exec_switch;
    }
  }
  if((strcmp("SYST", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("LIST", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("PWD", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("CWD", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("RETR", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("DELETE", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("MKDIR", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("RMDIR", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("EXIT", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("HELP", sInfo.scelta) == 0)) goto prepara;
  printf("Comando errato. Scrivere HELP per l'aiuto.\n"); goto exec_switch;

  prepara:
  if(strcmp(sInfo.scelta, "HELP") == 0) goto exec_help;
  strcpy(buffer, sInfo.scelta);
  var = strlen(buffer);
  if(send(sockd, buffer, var, 0) < 0){
    perror("Errore durante l'invio azione");
    onexit(sockd, 0, 0, 1);
  }
  
  if(strcmp(sInfo.scelta, "SYST") == 0){ free(sInfo.scelta); goto exec_syst; }
  if(strcmp(sInfo.scelta, "LIST") == 0){ free(sInfo.scelta); goto exec_list; }
  if(strcmp(sInfo.scelta, "PWD") == 0){ free(sInfo.scelta); goto exec_pwd; }
  if(strcmp(sInfo.scelta, "CWD") == 0){ free(sInfo.scelta); goto exec_cwd; }
  if(strcmp(sInfo.scelta, "RETR") == 0){ free(sInfo.scelta); goto exec_retr; }
  if(strcmp(sInfo.scelta, "DELETE") == 0){ free(sInfo.scelta); goto exec_delete; }
  if(strcmp(sInfo.scelta, "MKDIR") == 0){ free(sInfo.scelta); goto exec_mkdir; }
  if(strcmp(sInfo.scelta, "RMDIR") == 0){ free(sInfo.scelta); goto exec_rmdir; }
  if(strcmp(sInfo.scelta, "EXIT") == 0){ free(sInfo.scelta); goto exec_exit; }

  exec_help:
  printf("I comandi disponibili sono:\nSYST - LIST - PWD - CWD - RETR - DELETE - MKDIR - RMDIR - EXIT - HELP\n");
  free(sInfo.scelta);
  goto exec_switch;
  /************************* FINE PARTE AZIONE UTENTE *************************/

	/************************* RICHIESTA SYST *************************/
	exec_syst:
  memset(buffer, 0, sizeof(buffer));
  strcpy(buffer, "SYST\n");
  if(send(sockd, buffer, strlen(buffer), 0) < 0){
  	perror("Errore durante l'invio richiesta SYST");
  	onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
  	perror("Errore durante la ricezione risposta SYST");
  	onexit(sockd, 0, 0, 1);
  }
  sInfo.conferma = strtok(buffer, "\n");
  printf("SYST type: %s\n", sInfo.conferma);
  sInfo.conferma = NULL;
  memset(buffer, 0, sizeof(buffer));
  goto exec_switch;
	/************************* FINE SYST *************************/

	/************************* INVIO RICHIESTA FILE LISTING *************************/
	exec_list:
  memset(buffer, 0, sizeof(buffer));
  memset(tmp_buf, 0, sizeof(tmp_buf));
	strcpy(buffer, "LIST\n");
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio richiesta LIST");
		onexit(sockd, 0, 0, 1);
	}
  if(recv(sockd, tmp_buf, sizeof(tmp_buf), 0) < 0){
    perror("Errore nella ricezione della grandezza del file");
    onexit(sockd, 0 ,0 ,1);
  }
  fsize = atoi(tmp_buf);
  if((fd = open("listfiles.txt", O_CREAT | O_WRONLY,0644)) < 0){
  	perror("open file list");
   	onexit(sockd, 0, 0, 1);
  }
  fsize_tmp = fsize;
  sInfo.filebuffer = malloc(fsize);
  if(sInfo.filebuffer == NULL){
   	perror("malloc");
   	onexit(sockd, 0, fd, 4);
  }
  while(((uint32_t)total_bytes_read != fsize) && ((nread = read(sockd, sInfo.filebuffer, fsize_tmp)) > 0)){
    if(write(fd, sInfo.filebuffer, nread) != nread){
      perror("write list file");
		  onexit(sockd, 0, 0, 1);
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
	}
	close(fd);
	printf("----- FILE LIST -----\n");
	if((fp = fopen("listfiles.txt", "r+")) == NULL){
		perror("open file for read");
		onexit(sockd, 0, 0, 1);
	}
	while((c=getc(fp)) != EOF){
		putchar(c);
	}
  fclose(fp);
	printf("----- END FILE LIST -----\n");
  if(remove( "listfiles.txt" ) == -1 ){
    perror("errore cancellazione file");
    onexit(sockd, 0, 0, 1);
  }
  memset(buffer, 0, sizeof(buffer));
  memset(tmp_buf, 0, sizeof(tmp_buf));
  free(sInfo.filebuffer);
  fflush(stdout);
  goto exec_switch;
	/************************* FINE RICHIESTA FILE LISTING *************************/

	/************************* RICHIESTA PWD *************************/
	exec_pwd:
  memset(buffer, 0, sizeof(buffer));
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
	sInfo.conferma = strtok(buffer, "\n");
  printf("%s\n", sInfo.conferma);
  sInfo.conferma = NULL;
  memset(buffer, 0, sizeof(buffer));
  goto exec_switch;
	/************************* FINE RICHIESTA PWD *************************/

	/************************* INVIO RICHIESTA CWD *************************/
	exec_cwd:
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
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
  sInfo.conferma = NULL;
	sInfo.conferma = strtok(buffer, "\0");
  printf("%s", sInfo.conferma);
  if(strcmp(sInfo.conferma, "ERRORE: Percorso non esistente\n") == 0){
    onexit(sockd, 0, 0, 1);
  }
  memset(buffer, 0, sizeof(buffer));
  memset(dirpath, 0, sizeof(dirpath));
  goto exec_switch;
	/************************* FINE RICHIESTA CWD *************************/

	/************************* INVIO NOME FILE E RICEZIONE FILE *************************/
	exec_retr:
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  memset(tmp_buf, 0, sizeof(tmp_buf));
	printf("Inserire il nome del file da scaricare: ");
	if(fgets(dirpath, BUFFGETS, stdin) == NULL){
		perror("fgets nome file");
		onexit(sockd, 0 ,0 ,1);
	}
  sInfo.filename = NULL;
	sInfo.filename = strtok(dirpath, "\n");
	sprintf(buffer, "RETR %s", dirpath);
	if(send(sockd, buffer, strlen(buffer), 0) < 0){
		perror("Errore durante l'invio del nome del file");
		onexit(sockd, 0, 0, 1);
	}
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    perror("Errore ricezione conferma file");
    onexit(sockd, 0 ,0 ,1);
  }
  fsize = 0;
  sInfo.conferma = NULL;
  sInfo.conferma = strtok(buffer, "\0");
  if(strcmp(sInfo.conferma, "ERRORE: File non esistente") == 0){
    printf("ERRORE: il file richiesto non esiste\n");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, tmp_buf, sizeof(tmp_buf), 0) < 0){
    perror("Errore nella ricezione della grandezza del file");
    onexit(sockd, 0 ,0 ,1);
  }
  fsize = atoi(tmp_buf);
  fflush(stdout);
	fd = open(sInfo.filename, O_CREAT | O_WRONLY, 0644);
	if (fd  < 0) {
		perror("open");
		onexit(sockd, 0 ,0 ,1);
	}
	fsize_tmp = fsize;
	sInfo.filebuffer = malloc(fsize);
  if(sInfo.filebuffer == NULL){
   	perror("malloc");
   	onexit(sockd, 0, fd, 4);
  }
  total_bytes_read = 0;
  nread = 0;
  while(((uint32_t)total_bytes_read != fsize) && ((nread = read(sockd, sInfo.filebuffer, fsize_tmp)) > 0)){
    if(write(fd, sInfo.filebuffer, nread) != nread){
			perror("write RETR");
			onexit(sockd, 0, 0, 1);
		}
		total_bytes_read += nread;
		fsize_tmp -= nread;
	}
  close(fd); /* la chiusura del file va qui altrimenti client entra in loop infinito e si scrive all'interno del file */

	memset(buffer, 0, sizeof(buffer));
	if(recv(sockd, buffer, 34, 0) < 0){
    perror("Errore ricezione 226");
    onexit(sockd, 0, 0, 1);
  }
  printf("%s", buffer);
  memset(buffer, 0, sizeof(buffer));
  memset(tmp_buf, 0, sizeof(tmp_buf));
  memset(dirpath, 0, sizeof(dirpath));
  free(sInfo.filebuffer);
  goto exec_switch;
	/************************* FINE INVIO NOME FILE E RICEZIONE FILE *************************/

  /************************* INVIO RICHIESTA DELETE FILE *************************/
  exec_delete:
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  printf("Inserire il nome del file da scaricare: ");
  if(fgets(dirpath, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(sockd, 0 ,0 ,1);
  }
  sInfo.filename = NULL;
  sInfo.conferma = NULL;
  sInfo.filename = strtok(dirpath, "\n");
  sprintf(buffer, "DELETE %s", dirpath);
  if(send(sockd, buffer, strlen(buffer), 0) < 0){
    perror("Errore durante l'invio del nome del file");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    perror("Errore ricezione conferma file");
    onexit(sockd, 0 ,0 ,1);
  }
  sInfo.conferma = strtok(buffer, "\0");
  if((strcmp(sInfo.conferma, "ERRORE: File non esistente") == 0) || (strcmp(sInfo.conferma, "NONOK") == 0)){
    printf("ERRORE: il file richiesto non esiste o non si può cancellare\n");
    onexit(sockd, 0, 0, 1);
  } else printf("Il file '%s' è stato cancellato correttamente\n", sInfo.filename);
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  goto exec_switch;
  /************************* FINE INVIO RICHIESTA DELETE FILE *************************/

  /************************* INIZIO PARTE RICHIESTA MKDIR *************************/
  exec_mkdir:
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  printf("Inserire il nome della cartella da creare: ");
  if(fgets(dirpath, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(sockd, 0 ,0 ,1);
  }
  sInfo.filename = NULL;
  sInfo.conferma = NULL;
  sInfo.filename = strtok(dirpath, "\n");
  sprintf(buffer, "MKDIR %s", dirpath);
  if(send(sockd, buffer, strlen(buffer), 0) < 0){
    perror("Errore durante l'invio del nome della cartella");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    perror("Errore ricezione conferma cartella");
    onexit(sockd, 0 ,0 ,1);
  }
  sInfo.conferma = strtok(buffer, "\0");
  if(strcmp(sInfo.conferma, "ERRORE: Cartella non creata") == 0){
    printf("ERRORE: la cartella non può essere creata\n");
    onexit(sockd, 0, 0, 1);
  } else printf("La cartella è stata creata correttamente\n");
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  goto exec_switch;
  /************************* FINE PARTE RICHIESTA MKDIR *************************/

  /************************* INIZIO PARTE RICHIESTA RMDIR *************************/
  exec_rmdir:
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  printf("Inserire il nome della cartella da creare: ");
  if(fgets(dirpath, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(sockd, 0 ,0 ,1);
  }
  sInfo.filename = NULL;
  sInfo.conferma = NULL;
  sInfo.filename = strtok(dirpath, "\n");
  sprintf(buffer, "MKDIR %s", dirpath);
  if(send(sockd, buffer, strlen(buffer), 0) < 0){
    perror("Errore durante l'invio del nome della cartella");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, sizeof(buffer), 0) < 0){
    perror("Errore ricezione conferma cartella");
    onexit(sockd, 0 ,0 ,1);
  }
  sInfo.conferma = strtok(buffer, "\0");
  if(strcmp(sInfo.conferma, "ERRORE: Cartella non eliminata") == 0){
    printf("ERRORE: impossibile eliminare la cartella\n");
    onexit(sockd, 0, 0, 1);
  } else printf("La cartella è stata eliminata correttamente\n");
  memset(dirpath, 0, sizeof(dirpath));
  memset(buffer, 0, sizeof(buffer));
  goto exec_switch;
  /************************* FINE PARTE RICHIESTA RMDIR *************************/

  /************************* SALUTO FINALE *************************/
	exec_exit:
  memset(buffer, 0, sizeof(buffer));
  if(recv(sockd, buffer, 12, 0) < 0){
    perror("Errore ricezione 221");
    onexit(sockd, 0, 0, 1);
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