/* Descrizione: Semplice server FTP iterativo sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.0-alpha6
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
#include <signal.h>
#include <dirent.h>
#include <wait.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../../prototypes.h"

int main(int argc, char *argv[]){
	
	check_before_start(argc, argv);
	
	int sockd = -1, newsockd, socket_len;
	int NumPorta = atoi(argv[1]);
	struct sockaddr_in serv_addr, cli_addr; /* strutture contenenti indirizzo del server e del client */
    pid_t child_pid;
	
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
		onexit(sockd, 0, 0, 1);
	}

	if(listen(sockd, 20) < 0){
		perror("Errore nella funzione listen");
        onexit(sockd, 0, 0, 1);
	}
	socket_len = sizeof(cli_addr);

	signal (SIGINT, ( void *)sig_handler); /* se premuto CTRL+C il server termina */
	
	while(1){
        newsockd = -1; /* inizializzare a -1 per testare se "l'accept" sarà valido o no */
        memset(&cli_addr, 0, sizeof(cli_addr));

        if((newsockd = accept(sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
            perror("Errore nella connessione\n");
            onexit(newsockd, sockd, 0, 2);
        }
        /* inet_ntoa converte un hostname in un ip decimale puntato */
        fprintf(stdout, "Ricevuta richiesta di connessione dall' indirizzo %s\n", inet_ntoa(cli_addr.sin_addr));

        child_pid = fork();
        if(child_pid < 0){ /* se pid < 0 il processo figlio non è stato generato */
            perror("Fork error");
            onexit(newsockd, sockd, 0, 2);
        }
        if(child_pid == 0){ /* se pid == 0 il processo figlio è stato generato */
            do_child(newsockd); /* ed eseguo il server figlio */
            exit(0);
        }
        else{ /* siamo nel processo padre...che fare?? */
            while(waitpid(child_pid, NULL, WNOHANG) > 0){ /* it's just to clean up after dead child processes, which ones doesn't matter. */
                continue;
            }
        }
    }
    return EXIT_SUCCESS;
}

void do_child(const int child_sock){
    int login;
    uint32_t len_string;
    static char buffer[512], saved_user[512]; /* dichiaro static così viene direttamente inizializzato a 0 l'array */
    char *user_string = NULL, *username = NULL, *pass_string = NULL, *password = NULL;
    char *serverdir = (char *)(intptr_t)get_current_dir_name();
    char *tmpip = NULL, *pubip = NULL;  
    /************************* MESSAGGIO DI BENVENUTO *************************/
    tmpip = get_public_ip();
    pubip = strtok(tmpip, "\n");
    sprintf(buffer, "220 FTPUtils Server [%s]", pubip);
    len_string = strlen(buffer)+1;
    if(send(child_sock, &len_string, sizeof(len_string), 0) < 0){
        perror("Errore invio len buffer");
        close(child_sock);
        exit(1);
    }
    if(send(child_sock, buffer, len_string, 0) < 0){
        perror("Errore durante l'invio");
        close(child_sock);
        exit(1);
    }
    pubip = NULL;
    free(tmpip);
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE MESSAGGIO DI BENVENUTO *************************/

    /************************* INIZIO PARTE LOGIN *************************/
    /************************* RICEVIAMO NOME UTENTE E MANDIAMO CONFERMA *************************/
    if(recv(child_sock, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
        perror("Errore ricezione len user buffer");
        close(child_sock);
        exit(1);
    }
    if(recv(child_sock, buffer, len_string, 0) < 0){
        perror("Errore ricezione del nome utente");
    	close(child_sock);
        exit(1);
    }    	
    user_string = strtok(buffer, " ");
    username = strtok(NULL, "\n");
    username = strdup(username); /* con strdup copio lo username in un'altra zona di memoria così posso memsettare il buffer (ricordarsi FREE) */
    fprintf(stdout, "%s %s\n", user_string, username);
    sprintf(saved_user, "%s", username);
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE NOME UTENTE *************************/

    /************************* RICEVIAMO PASSWORD E MANDIAMO CONFERMA *************************/
    if(recv(child_sock, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
        perror("Errore ricezione len pass buffer");
        close(child_sock);
        exit(1);
    }
    if(recv(child_sock, buffer, len_string, 0) < 0){
        perror("Errore ricezione password");
    	close(child_sock);
        exit(1);
    }
    pass_string = strtok(buffer, " ");
    password = strtok(NULL, "\n");
    password = strdup(password);
    fprintf(stdout, "%s %s\n", pass_string, password);
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE PASSWORD *************************/
    	
    /************************* INVIO CONFERMA LOG IN *************************/
    printf("%s %s\n", username, password);
    login = check_login_details(username, password);
    if(login != 0){
        strcpy(buffer, "Username o password non esistenti");
        len_string = strlen(buffer)+1;
        if(send(child_sock, &len_string, sizeof(len_string), 0) < 0){
            perror("Errore invio len buffer login failed");
            close(child_sock);
            exit(1);
        }
        if(send(child_sock, buffer, len_string, 0) < 0){
            perror("Errore invio buffer login failed");
            close(child_sock);
            exit(1);
        }
        close(child_sock);
        exit(1);
    }
    printf("USER: %s - PASS: ******\n", username);
    sprintf(buffer, "230 USER %s logged in", saved_user);
    len_string = strlen(buffer)+1;
    if(send(child_sock, &len_string, sizeof(len_string), 0) < 0){
        perror("Errore invio len buffer conferma login");
        close(child_sock);
        exit(1);
    }
    if(send(child_sock, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante l'invio");
        close(child_sock);
        exit(1);
    }
    free(username);
    free(password);
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE CONFERMA LOG IN *************************/
    /************************* FINE PARTE LOGIN *************************/

    /************************* RESTO IN ASCOLTO DELL'AZIONE DAL CLIENT *************************/
    exec_listen_action:
    memset(buffer, 0, sizeof(buffer));
    if(recv(child_sock, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
        perror("Errore ricezione len buffer scelta");
        close(child_sock);
        exit(1);
    }
    if(recv(child_sock, buffer, len_string, 0) < 0){
        perror("Errore nella ricezione azione\n");
        close(child_sock);
        exit(1);
    }
    if(strcmp(buffer, "SYST") == 0) goto prepara;
    if(strcmp(buffer, "LIST") == 0) goto prepara;
    if(strcmp(buffer, "PWD") == 0) goto prepara;
    if(strcmp(buffer, "CWD") == 0) goto prepara;
    if(strcmp(buffer, "RETR") == 0) goto prepara;
    if(strcmp(buffer, "STOR") == 0) goto prepara;
    if(strcmp(buffer, "DELE") == 0) goto prepara;
    if(strcmp(buffer, "MKD") == 0) goto prepara;
    if(strcmp(buffer, "RMD") == 0) goto prepara;
    if(strcmp(buffer, "RNM") == 0) goto prepara;
    if(strcmp(buffer, "EXIT") == 0) goto prepara;
    
    prepara:
    if(strcmp(buffer, "SYST") == 0) goto exec_syst;
    if(strcmp(buffer, "LIST") == 0) goto exec_list;
    if(strcmp(buffer, "PWD") == 0) goto exec_pwd;
    if(strcmp(buffer, "CWD") == 0) goto exec_cwd;
    if(strcmp(buffer, "RETR") == 0) goto exec_retr;
    if(strcmp(buffer, "STOR") == 0) goto exec_stor;
    if(strcmp(buffer, "DELE") == 0) goto exec_delete;
    if(strcmp(buffer, "MKD") == 0) goto exec_mkdir;
    if(strcmp(buffer, "RMD") == 0) goto exec_rmdir;
    if(strcmp(buffer, "RNM") == 0) goto exec_rename;
    if(strcmp(buffer, "EXIT") == 0) goto send_goodbye;
    /************************* FINE PARTE ASCOLTO *************************/

    /************************* INIZIO AZIONE SYST *************************/
    exec_syst:
    do_server_fork_syst_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE SYST *************************/

    /************************* INIZIO AZIONE LIST *************************/
    exec_list:
    do_server_fork_list_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE LIST *************************/

    /************************* INIZIO AZIONE PWD *************************/
    exec_pwd:
    do_server_fork_pwd_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE PWD *************************/

    /************************* INIZIO AZIONE CWD *************************/
    exec_cwd:
    do_server_fork_cwd_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE CWD *************************/

    /************************* INIZIO AZIONE RETR *************************/
    exec_retr:
    do_server_fork_retr_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE RETR *************************/

    /************************* INIZIO AZIONE STOR *************************/
    exec_stor:
    do_server_fork_stor_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE STOR *************************/

    /************************* INIZIO AZIONE DELE *************************/
    exec_delete:
    do_server_fork_dele_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE DELE *************************/

    /************************* INIZIO AZIONE MKD *************************/
    exec_mkdir:
    do_server_fork_mkd_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE MKD *************************/

    /************************* INIZIO AZIONE RMD *************************/
    exec_rmdir:
    do_server_fork_rmd_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE RMD *************************/

    /************************* INIZIO AZIONE RNM *************************/
    exec_rename:
    do_server_fork_rnm_cmd(child_sock);
    goto exec_listen_action;
    /************************* FINE AZIONE RNM *************************/

    /************************* INIZIO SALUTO FINALE *************************/
    send_goodbye:
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "221 Goodbye\n");
    if(send(child_sock, buffer, strlen(buffer), 0) < 0){
        perror("Errore durante l'invio 221");
        close(child_sock);
        exit(1);
    }
    if(chdir(serverdir) < 0){
        perror("chdir on default dir");
    }
    memset(buffer, 0, sizeof(buffer));
    printf("Ricevuta richiesta EXIT\n");
    close(child_sock);
    /************************* FINE SALUTO FINALE *************************/
}

void check_before_start(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stdout, "Uso: %s <numero porta>\n", argv[0]);
		exit(1);
	}
}

void sig_handler(const int signo, const int sockd, const int newsockd){
  if (signo == SIGINT){
    printf("%d\n", getpid());
    printf("Ricevuto SIGINT, esco...\n");
    if(newsockd) close(newsockd);
    if(sockd) close(sockd);
    exit(EXIT_SUCCESS);
  }
}