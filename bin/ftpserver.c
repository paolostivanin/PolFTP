/* Descrizione: Semplice server FTP iterativo sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#define _GNU_SOURCE /* per definire get_current_dir_name */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h> /* per usare uint32_t invece di size_t */
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
#include <dirent.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "prototypes.h"

int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);
	
	int sockd, newsockd, socket_len, rc, rc_list, fd, fpl, n = 1, var = 0, number = 0;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* strutture contenenti indirizzo del server e del client */
	off_t offset = 0, offset_list = 0; /* variabile di tipo offset */
	struct stat fileStat; /* struttura contenente informazioni sul file scelto */
	struct hostent *local_ip; /* struttura contenente ip server */
	static char buffer[256], saved_user[256]; /* dichiaro static così viene direttamente inizializzato a 0 l'array */
	char *user_string = NULL, *username = NULL, *pass_string = NULL, *password = NULL, *other = NULL, *cd_path = NULL, *path = NULL;
  char *sysname = NULL, *filename = NULL;
	uint32_t fsize, count, i;
  char **files;
  FILE *fp_list;
	
	if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Errore creazione socket\n");
		exit(EXIT_FAILURE);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* porta htons converte nell'ordine dei byte di rete */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */
	
	if(bind(sockd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Errore di bind\n");
		onexit(0, sockd, 0, 1);
	}

	if(listen(sockd, 5) < 0){
			perror("Errore nella funzione listen");
    		onexit(0, sockd, 0, 1);
	}
	socket_len = sizeof(cli_addr);
	
	if((local_ip = gethostbyname("localhost")) == NULL){
		perror("gethostbyname()");
		exit(1);
	}	
	
	signal (SIGINT, ( void *)sig_handler); /* se premuto CTRL+C il server termina */
	
	while(1){
		if((newsockd = accept(sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
			perror("Errore nella connessione\n");
			onexit(newsockd, sockd, 0, 2);
		}

    /* inet_ntoa converte un hostname in un ip decimale puntato */
    fprintf(stdout, "Ricevuta richiesta di connessione dall' indirizzo %s\n", inet_ntoa(cli_addr.sin_addr));

    /************************* MESSAGGIO DI BENVENUTO *************************/
    sprintf(buffer, "220 FTPUtils SERVER [%s]", local_ip->h_name);
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
			onexit(newsockd, sockd, 0, 2);
		}
		memset(buffer, 0, sizeof(buffer));
    /************************* FINE MESSAGGIO DI BENVENUTO *************************/

		/************************* INIZIO PARTE LOGIN *************************/
    /************************* RICEVIAMO NOME UTENTE E MANDIAMO CONFERMA *************************/
		 if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione del nome utente");
    	onexit(newsockd, sockd, 0, 2);
    }    	
    user_string = strtok(buffer, " ");
    username = strtok(NULL, "\n");
    fprintf(stdout, "%s %s\n", user_string, username);
    sprintf(saved_user, "%s", username);
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "USEROK\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
		  onexit(newsockd, sockd, 0, 2);
		}
		memset(buffer, 0, sizeof(buffer));
    /************************* FINE NOME UTENTE *************************/

    /************************* RICEVIAMO PASSWORD E MANDIAMO CONFERMA *************************/
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
    	perror("Errore nella ricezione del nome utente");
    	onexit(newsockd, sockd, 0, 2);
    }
    pass_string = strtok(buffer, " ");
    password = strtok(NULL, "\n");
    fprintf(stdout, "%s %s\n", pass_string, password);
    memset(buffer, 0, sizeof(buffer));
   	strcpy(buffer, "PASSOK\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
		  onexit(newsockd, sockd, 0, 2);
		}
		memset(buffer, 0, sizeof(buffer));
    /************************* FINE PASSWORD *************************/
    	
    /************************* INVIO CONFERMA LOG IN *************************/
    sprintf(buffer, "230 USER %s logged in\n", saved_user);
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
			perror("Errore durante l'invio");
			onexit(newsockd, sockd, 0, 2);
		}
		memset(buffer, 0, sizeof(buffer));
		/************************* FINE CONFERMA LOG IN *************************/
    /************************* FINE PARTE LOGIN *************************/

    /************************* RESTO IN ASCOLTO DELL'AZIONE DAL CLIENT *************************/
    exec_listen_action:
    if(recv(newsockd, &var, sizeof(var), 0) < 0){
      perror("Errore nella ricezione lunghezza azione\n");
      close(newsockd);
      exit(1);
    }
    if(recv(newsockd, buffer, var, 0) < 0){
      perror("Errore nella ricezione azione\n");
      close(newsockd);
      exit(1);
    }
    if(strcmp(buffer, "SYST") == 0) number = 1;
    if(strcmp(buffer, "LIST") == 0) number = 2;
    if(strcmp(buffer, "PWD") == 0) number = 3;
    if(strcmp(buffer, "CWD") == 0) number = 4;
    if(strcmp(buffer, "RETR") == 0) number = 5;
    if(strcmp(buffer, "EXIT") == 0) number = 6;
    switch(number){
      case 1:
        var=0;
        if(send(newsockd, &var, sizeof(var), 0) < 0){
          perror("Errore durante l'invio errore ricezione azione");
          close(newsockd);
          exit(1);
        };
        goto exec_syst;
      case 2:
        var=0;
        if(send(newsockd, &var, sizeof(var), 0) < 0){
          perror("Errore durante l'invio errore ricezione azione");
          close(newsockd);
          exit(1);
        };
        goto exec_list;
      case 3: 
        var=0;
        if(send(newsockd, &var, sizeof(var), 0) < 0){
          perror("Errore durante l'invio errore ricezione azione");
          close(newsockd);
          exit(1);
        };
        goto exec_pwd;
      case 4:
        var=0;
        if(send(newsockd, &var, sizeof(var), 0) < 0){
          perror("Errore durante l'invio errore ricezione azione");
          close(newsockd);
          exit(1);
        };
        goto exec_cwd;
      case 5:
        var=0;
        if(send(newsockd, &var, sizeof(var), 0) < 0){
          perror("Errore durante l'invio errore ricezione azione");
          close(newsockd);
          exit(1);
        };
        goto exec_retr;
      case 6:
        var=0;
        if(send(newsockd, &var, sizeof(var), 0) < 0){
          perror("Errore durante l'invio errore ricezione azione");
          close(newsockd);
          exit(1);
        };
        goto send_goodbye;
      default: printf("Istruzione errata\n"); goto exec_resend;
    }
    exec_resend:
    var=1;
    if(send(newsockd, &var, sizeof(var), 0) < 0){
      perror("Errore durante l'invio errore ricezione azione");
      close(newsockd);
      exit(1);
    }
    /************************* FINE PARTE ASCOLTO *************************/

    /************************* RICHIESTA SYST *************************/
    exec_syst:
    if(recv(newsockd, buffer, 5, 0) < 0){
      perror("Errore ricezione comando SYST");
      onexit(newsockd, sockd, 0, 2);
    }
    get_syst(&sysname);
    sprintf(buffer, "%s\n", sysname);
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio risposta SYST");
      onexit(newsockd, sockd, 0, 2);
    }
    memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* FINE SYST *************************/

    /************************* RICEZIONE RICHIESTA LIST E INVIO LISTA *************************/
    exec_list:
    if(recv(newsockd, buffer, 5, 0) < 0){
    	perror("Errore nella ricezione comando LIST");
    	onexit(newsockd, sockd, 0, 2);
    }
    other = strtok(buffer, "\n");
    if(strcmp(other, "LIST") == 0){
    	printf("Ricevuta richiesta LIST\n");
    } else onexit(newsockd, sockd, 0, 2);
    
    count = file_list("./", &files);
    if((fp_list = fopen("listfiles.txt", "w")) == NULL){
      perror("Impossibile aprire il file per la scrittura LIST");
      onexit(newsockd, sockd, 0, 2);
    }
    for(i=0; i < count; i++){
      if(strcmp(files[i], "DIR ..") == 0 || strcmp(files[i], "DIR .") == 0) continue;
      else{
        if(n%2 == 0) fprintf(fp_list, "%s\n", files[i]);
        else fprintf(fp_list, "%20s\t", files[i]);
        n++;
      }
    }
    fclose(fp_list);
    if((fpl = open("listfiles.txt", O_RDONLY)) < 0){
      perror("open file with open");
      onexit(newsockd, sockd, 0, 2);
      exit(1);
    }
    if(fstat(fpl, &fileStat) < 0){
      perror("Errore fstat");
      onexit(newsockd, sockd, fpl, 3);
    }
    fsize = fileStat.st_size;
    if(send(newsockd, &fsize, sizeof(fsize), 0) < 0){
      perror("Errore durante l'invio grande file list");
      onexit(newsockd, sockd, fpl, 3);
    }
    rc_list = sendfile(newsockd, fpl, &offset_list, fileStat.st_size);
    if(rc_list == -1){
      perror("Invio file list non riuscito");
      onexit(newsockd, sockd, fpl, 3);
    }
    if((uint32_t)rc_list != fsize){
      fprintf(stderr, "Trasferimento incompleto: %d di %d bytes inviati\n", rc_list, (int)fileStat.st_size);
      onexit(newsockd, sockd, fpl, 3);
    }
    close(fpl);
    if(remove( "listfiles.txt" ) == -1 ){
      perror("errore cancellazione file");
      onexit(newsockd, sockd, 0, 2);
    }
		memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* FINE RICEZIONE LIST E INVIO LISTA *************************/

    /************************* RICHIESTA PWD *************************/
    exec_pwd:
    if(recv(newsockd, buffer, 4, 0) < 0){
      perror("Errore nella ricezione comando PWD");
      onexit(newsockd, sockd, 0, 2);
    }
    other = strtok(buffer, "\n");
    if(strcmp(other, "PWD") == 0){
      printf("Ricevuta richiesta PWD\n");
    } else onexit(newsockd, sockd, 0, 2); 
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "PWD: %s\n", (char *)(intptr_t)get_current_dir_name());
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio PWD");
      onexit(newsockd, sockd, 0, 2);
    }
    memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* FINE RICHIESTA PWD *************************/

    /************************* RICHIESTA CWD *************************/
    exec_cwd:
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione comando CWD");
      onexit(newsockd, sockd, 0, 2);
    }
    cd_path = strtok(buffer, " ");
    path = strtok(NULL, "\n");
    if(strcmp(cd_path, "CWD") == 0){
      printf("Ricevuta richiesta CWD\n");
    } else onexit(newsockd, sockd, 0, 2);
    if(chdir(path) < 0){
      perror("chdir");
      onexit(newsockd, sockd, 0, 2);
    }
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "250 CWD command successful. PWD: %s\n", (char *)(intptr_t)get_current_dir_name());
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
      onexit(newsockd, sockd, 0, 2);
    }
    memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* FINE RICHIESTA CD *************************/

    /************************* RICEZIONE NOME FILE ED INVIO FILE *************************/
    exec_retr:
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione del nome del file");
      onexit(newsockd, sockd, 0, 2);
    }
    other = strtok(buffer, " ");
    filename = strtok(NULL, "\n");
    if(strcmp(other, "RETR") == 0){
      printf("Ricevuta richiesta RETR\n");
    } else onexit(newsockd, sockd, 0, 2);
    
    fd = open(filename, O_RDONLY);
   		if (fd < 0) {
    	fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    	onexit(newsockd, sockd, 0, 2);
    }

    if(fstat(fd, &fileStat) < 0){
    	perror("Errore fstat");
    	onexit(newsockd, sockd, fd, 3);
    }
    fsize = fileStat.st_size;
    if(send(newsockd, &fsize, sizeof(fsize), 0) < 0){
    	perror("Errore durante l'invio della grandezza del file\n");
    	onexit(newsockd, sockd, fd, 3);
    }

   	rc = sendfile(newsockd, fd, &offset, fileStat.st_size);
    if(rc == -1) {
     		fprintf(stderr, "Errore durante l'invio di: '%s'\n", strerror(errno));
     		onexit(newsockd, sockd, fd, 3);
    }
    if((uint32_t)rc != fsize) {
    	fprintf(stderr, "Trasferimento incompleto: %d di %d bytes inviati\n", rc, (int)fileStat.st_size);
    	onexit(newsockd, sockd, fd, 3);
    }
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "226 File trasferito con successo\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio 226");
      onexit(newsockd, sockd, 0, 2);
    }
    memset(buffer, 0, sizeof(buffer));
    close(fd);
    goto exec_listen_action;
    /************************* FINE RICEZIONE NOME FILE ED INVIO FILE *************************/

    /************************* SALUTO FINALE *************************/
    send_goodbye:
    strcpy(buffer, "221 Goodbye\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio 221");
      onexit(newsockd, sockd, 0, 2);
    }
    onexit(newsockd, sockd, 0, 2);
    /************************* SALUTO FINALE *************************/
	}
	return EXIT_SUCCESS;
}

void check_before_start(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stdout, "Uso: %s <numero porta>\n", argv[0]);
		exit(1);
	}
}

void sig_handler(int signo, int sockd, int newsockd, int file){
  if (signo == SIGINT){
    printf("Ricevuto SIGINT, esco...\n");
    if(newsockd) close(newsockd);
    close(sockd);
    if(file) close(file);
    exit(EXIT_SUCCESS);
  }
}