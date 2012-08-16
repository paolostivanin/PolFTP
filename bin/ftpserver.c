/* Descrizione: Semplice server FTP iterativo sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.0-alpha3
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
	
	int sockd, newsockd, socket_len, rc, rc_list, fd, fpl, /*var = 0,*/ retval;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* strutture contenenti indirizzo del server e del client */
	off_t offset, offset_list; /* variabile di tipo offset */
	struct stat fileStat; /* struttura contenente informazioni sul file scelto */
	struct hostent *local_ip; /* struttura contenente ip server */
  struct in_addr **pptr;
	static char buffer[512], saved_user[512], tmp_buf[BUFSIZ]; /* dichiaro static così viene direttamente inizializzato a 0 l'array */
	char *user_string = NULL, *username = NULL, *pass_string = NULL, *password = NULL, *other = NULL, *cd_path = NULL, *path = NULL;
  char *sysname = NULL, *filename = NULL, **files;
	uint32_t fsize, count, i, size_to_send;
  char *serverdir = (char *)(intptr_t)get_current_dir_name();
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

	signal (SIGINT, ( void *)sig_handler); /* se premuto CTRL+C il server termina */
	
	while(1){
    memset(&cli_addr, 0, sizeof(cli_addr));
    memset(buffer, 0, sizeof(buffer));
    memset(&local_ip, 0, sizeof(local_ip));
    if((local_ip = gethostbyname("localhost")) == NULL){
      perror("gethostbyname()");
      exit(1);
    }
    /* La lista puntata da h_addr_list è un array di puntatori. Pertanto è necessario dereferenziare ogni indice due volte,
     * una per avere il puntatore alla lista e l'altra per prendere l'in_addr struct a cui esso punta. 
     * Fatto ciò bisogna convertire l'unsigned int a 32 bit in un indirizzo ip decimale */
    pptr = (struct in_addr **)local_ip->h_addr_list;
    
		if((newsockd = accept(sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
			perror("Errore nella connessione\n");
			onexit(newsockd, sockd, 0, 2);
		}
    /* inet_ntoa converte un hostname in un ip decimale puntato */
    fprintf(stdout, "Ricevuta richiesta di connessione dall' indirizzo %s\n", inet_ntoa(cli_addr.sin_addr));

    /************************* MESSAGGIO DI BENVENUTO *************************/
    //sprintf(buffer, "220 FTPUtils SERVER [%s]", inet_ntoa(*(struct in_addr*)(local_ip->h_addr_list[i]))); /* converto hostname in ip decimale */
    sprintf(buffer, "220 FTPUtils Server [%s]", inet_ntoa(**(pptr)));
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio");
			onexit(newsockd, sockd, 0, 2);
		}
		memset(buffer, 0, sizeof(buffer));
    memset(&local_ip, 0, sizeof(local_ip)); /* pulire la struttura altrimenti alla seconda connessione si ha un segfault */
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
    memset(buffer, 0, sizeof(buffer));
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione azione\n");
      close(newsockd);
      exit(1);
    }
    if(strcmp(buffer, "SYST") == 0) goto prepara;
    if(strcmp(buffer, "LIST") == 0) goto prepara;
    if(strcmp(buffer, "PWD") == 0) goto prepara;
    if(strcmp(buffer, "CWD") == 0) goto prepara;
    if(strcmp(buffer, "RETR") == 0) goto prepara;
    if(strcmp(buffer, "DELETE") == 0) goto prepara;
    if(strcmp(buffer, "MKDIR") == 0) goto prepara;
    if(strcmp(buffer, "RMDIR") == 0) goto prepara;
    if(strcmp(buffer, "EXIT") == 0) goto prepara;

    prepara:
    if(strcmp(buffer, "SYST") == 0) goto exec_syst;
    if(strcmp(buffer, "LIST") == 0) goto exec_list;
    if(strcmp(buffer, "PWD") == 0) goto exec_pwd;
    if(strcmp(buffer, "CWD") == 0) goto exec_cwd;
    if(strcmp(buffer, "RETR") == 0) goto exec_retr;
    if(strcmp(buffer, "DELETE") == 0) goto exec_delete;
    if(strcmp(buffer, "MKDIR") == 0) goto exec_mkdir;
    if(strcmp(buffer, "RMDIR") == 0) goto exec_rmdir;
    if(strcmp(buffer, "EXIT") == 0) goto send_goodbye;
    /************************* FINE PARTE ASCOLTO *************************/

    /************************* RICHIESTA SYST *************************/
    exec_syst:
    memset(buffer, 0, sizeof(buffer));
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
    memset(buffer, 0, sizeof(buffer));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    if(recv(newsockd, buffer, 6, 0) < 0){ /* i 6 caratteri sono dati da L I S T \n \0 */
    	perror("Errore nella ricezione comando LIST");
    	onexit(newsockd, sockd, 0, 2);
    }
    other = NULL;
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
        fprintf(fp_list, "%s\n", files[i]);
      }
    }
    fclose(fp_list);
    free_file_list(&files, count);
    if((fpl = open("listfiles.txt", O_RDONLY)) < 0){
      perror("open file with open");
      onexit(newsockd, sockd, 0, 2);
    }
    if(fstat(fpl, &fileStat) < 0){
      perror("Errore fstat");
      onexit(newsockd, sockd, fpl, 3);
    }
    fsize = fileStat.st_size;
    snprintf(tmp_buf, BUFSIZ-1, "%" PRIu32, fsize);
    if(send(newsockd, tmp_buf, sizeof(tmp_buf), 0) < 0){
      perror("Errore durante l'invio della grandezza del file\n");
      onexit(newsockd, sockd, fpl, 3);
    }
    offset_list = 0;
    for (size_to_send = fsize; size_to_send > 0; ){
      rc_list = sendfile(newsockd, fpl, &offset_list, size_to_send);
      if (rc_list <= 0){
        perror("sendfile");
        onexit(newsockd, sockd, fpl, 3);
      }
      size_to_send -= rc_list;
    }
    close(fpl);
    if(remove( "listfiles.txt" ) == -1 ){
      perror("errore cancellazione file");
      onexit(newsockd, sockd, 0, 2);
    }
		memset(buffer, 0, sizeof(buffer));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    goto exec_listen_action;
    /************************* FINE RICEZIONE LIST E INVIO LISTA *************************/

    /************************* RICHIESTA PWD *************************/
    exec_pwd:
    memset(buffer, 0, sizeof(buffer));
    if(recv(newsockd, buffer, 4, 0) < 0){
      perror("Errore nella ricezione comando PWD");
      onexit(newsockd, sockd, 0, 2);
    }
    other = NULL;
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
    memset(buffer, 0, sizeof(buffer));
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione comando CWD");
      onexit(newsockd, sockd, 0, 2);
    }
    cd_path = NULL;
    path = NULL;
    cd_path = strtok(buffer, " ");
    path = strtok(NULL, "\n");
    if(strcmp(cd_path, "CWD") == 0){
      printf("Ricevuta richiesta CWD\n");
    } else onexit(newsockd, sockd, 0, 2);
    if(chdir(path) < 0){
      perror("chdir");
      strcpy(buffer, "ERRORE: Percorso non esistente\0");
      if(send(newsockd, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante l'invio");
        onexit(newsockd, sockd, 0, 2);
      }
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
    /************************* FINE RICHIESTA CWD *************************/

    /************************* RICEZIONE NOME FILE ED INVIO FILE *************************/
    exec_retr:
    memset(buffer, 0, sizeof(buffer));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione del nome del file");
      onexit(newsockd, sockd, 0, 2);
    }
    other = NULL;
    filename = NULL;
    other = strtok(buffer, " ");
    filename = strtok(NULL, "\n");

    if(strcmp(other, "RETR") == 0){
      printf("Ricevuta richiesta RETR\n");
    } else onexit(newsockd, sockd, 0, 2);

    fd = open(filename, O_RDONLY);
   	if(fd < 0){
    	fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
      strcpy(buffer, "ERRORE: File non esistente\0");
      if(send(newsockd, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante invio");
        onexit(newsockd, sockd, 0 ,2);
      }
    	onexit(newsockd, sockd, 0, 2);
    }
    strcpy(buffer, "OK\0");
    if(send(newsockd, buffer, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(newsockd, sockd, 0 ,2);
    }

    if(fstat(fd, &fileStat) < 0){
    	perror("Errore fstat");
    	onexit(newsockd, sockd, fd, 3);
    }
    fsize = fileStat.st_size;
    snprintf(tmp_buf, BUFSIZ-1, "%" PRIu32, fsize);
    if(send(newsockd, tmp_buf, sizeof(tmp_buf), 0) < 0){
    	perror("Errore durante l'invio della grandezza del file\n");
    	onexit(newsockd, sockd, fd, 3);
    }
    offset = 0;
    for (size_to_send = fsize; size_to_send > 0; ){
      rc = sendfile(newsockd, fd, &offset, size_to_send);
      if (rc <= 0){
        perror("sendfile");
        onexit(newsockd, sockd, fd, 3);
      }
      size_to_send -= rc;
    }
    close(fd); /* la chiusura del file va qui altrimenti rischio loop infinito e scrittura all'interno del file */

    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "226 File trasferito con successo\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio 226");
      onexit(newsockd, sockd, 0, 2);
    }
    memset(buffer, 0, sizeof(buffer));
    memset(tmp_buf, 0, sizeof(tmp_buf));
    goto exec_listen_action;
    /************************* FINE RICEZIONE NOME FILE ED INVIO FILE *************************/

    /************************* RICHIESTA DELETE FILE *************************/
    exec_delete:
    memset(buffer, 0, sizeof(buffer));
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione del nome del file");
      onexit(newsockd, sockd, 0, 2);
    }
    other = NULL;
    filename = NULL;
    other = strtok(buffer, " ");
    filename = strtok(NULL, "\n");
    if(strcmp(other, "DELETE") == 0){
      printf("Ricevuta richiesta DELETE\n");
    } else onexit(newsockd, sockd, 0, 2);
    
    fd = open(filename, O_WRONLY);
    if(fd < 0){
      fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
      strcpy(buffer, "ERRORE: File non esistente\0");
      if(send(newsockd, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante invio");
        onexit(newsockd, sockd, 0, 2);
      }
      onexit(newsockd, sockd, 0, 2);
    }
    retval = remove(filename);
    if(retval != 0){
      perror("File non cancellato");
      strcpy(buffer, "NONOK\0");
      if(send(newsockd, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante invio");
        onexit(newsockd, sockd, fd, 3);
      }
      onexit(newsockd, sockd, fd, 3);
    }
    strcpy(buffer, "OK\0");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante invio");
      onexit(newsockd, sockd, 0 ,2);
    }
    memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* RICHIESTA DELETE FILE *************************/

    /************************* RICHIESTA MKDIR *************************/
    exec_mkdir:
    memset(buffer, 0, sizeof(buffer));
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione del nome della cartella");
      onexit(newsockd, sockd, 0, 2);
    }
    other = NULL;
    filename = NULL;
    other = strtok(buffer, " ");
    filename = strtok(NULL, "\n");
    if(strcmp(other, "MKDIR") == 0){
      printf("Ricevuta richiesta MKDIR\n");
    } else onexit(newsockd, sockd, 0, 2);
    
    if(mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0){
      fprintf(stderr, "Impossibile creare la cartella '%s'\n", filename);
      strcpy(buffer, "ERRORE: Cartella non creata\0");
      if(send(newsockd, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante invio");
        onexit(newsockd, sockd, 0, 2);
      }
      onexit(newsockd, sockd, 0, 2);
    }
    strcpy(buffer, "OK\0");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante invio");
      onexit(newsockd, sockd, 0 ,2);
    }
    memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* FINE RICHIESTA MKDIR *************************/

    /************************* RICHIESTA RMDIR *************************/
    exec_rmdir:
    memset(buffer, 0, sizeof(buffer));
    if(recv(newsockd, buffer, sizeof(buffer), 0) < 0){
      perror("Errore nella ricezione del nome della cartella");
      onexit(newsockd, sockd, 0, 2);
    }
    other = NULL;
    filename = NULL;
    other = strtok(buffer, " ");
    filename = strtok(NULL, "\n");
    if(strcmp(other, "MKDIR") == 0){
      printf("Ricevuta richiesta RMDIR\n");
    } else onexit(newsockd, sockd, 0, 2);
    
    if(rmdir(filename) != 0){
      fprintf(stderr, "Impossibile eliminare la cartella '%s'\n", filename);
      strcpy(buffer, "ERRORE: Cartella non eliminata\0");
      if(send(newsockd, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante invio");
        onexit(newsockd, sockd, 0, 2);
      }
      onexit(newsockd, sockd, 0, 2);
    }
    strcpy(buffer, "OK\0");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante invio");
      onexit(newsockd, sockd, 0 ,2);
    }
    memset(buffer, 0, sizeof(buffer));
    goto exec_listen_action;
    /************************* FINE RICHIESTA RMDIR *************************/

    /************************* SALUTO FINALE *************************/
    send_goodbye:
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "221 Goodbye\n");
    if(send(newsockd, buffer, strlen(buffer), 0) < 0){
      perror("Errore durante l'invio 221");
      onexit(newsockd, sockd, 0, 2);
    }
    if(chdir(serverdir) < 0){
      perror("chdir on default dir");
    }
    memset(buffer, 0, sizeof(buffer));
    printf("Ricevuta richiesta EXIT\n");
    close(newsockd);
    /************************* FINE SALUTO FINALE *************************/
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