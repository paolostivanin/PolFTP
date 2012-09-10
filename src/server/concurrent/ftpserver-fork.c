/* Descrizione: Semplice server FTP iterativo sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.0-rc
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
		perror("Error on socket creation\n");
		exit(EXIT_FAILURE);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* porta htons converte nell'ordine dei byte di rete */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */
	
	if(bind(sockd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("Bind error\n");
		onexit(sockd, 0, 0, 1);
	}

	if(listen(sockd, 20) < 0){
		perror("Error on listen");
        onexit(sockd, 0, 0, 1);
	}
	socket_len = sizeof(cli_addr);

	signal (SIGINT, ( void *)sig_handler); /* se premuto CTRL+C il server termina */
	
	while(1){
        newsockd = -1; /* inizializzare a -1 per testare se "l'accept" sarà valido o no */
        memset(&cli_addr, 0, sizeof(cli_addr));

        if((newsockd = accept(sockd, (struct sockaddr *) &cli_addr, (socklen_t *) &socket_len)) < 0){
            perror("Connection error (accept)\n");
            onexit(newsockd, sockd, 0, 2);
        }
        /* inet_ntoa converte un hostname in un ip decimale puntato */
        fprintf(stdout, "Received connection from address: %s\n", inet_ntoa(cli_addr.sin_addr));

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
            while(waitpid(child_pid, NULL, WNOHANG) > 0){ /* serve solo per "pulire" dopo la "morte" del figlio per evitare gli "zombie". */
                continue;
            }
        }
    }
    return EXIT_SUCCESS;
}

void do_child(const int child_sock){
    int login, ret_val = -1;
    uint32_t len_string;
    static char buffer[512]; /* dichiaro static così viene direttamente inizializzato a 0 l'array */
    char *user_string = NULL, *username = NULL, *pass_string = NULL, *password = NULL;
    char *serverdir = (char *)(intptr_t)get_current_dir_name();
    char *tmpip = NULL, *pubip = NULL, *is_err = NULL;  
    /************************* MESSAGGIO DI BENVENUTO *************************/
    tmpip = get_public_ip();
    pubip = strtok(tmpip, "\n");
    sprintf(buffer, "220 FTPUtils Server [%s]", pubip);
    len_string = strlen(buffer)+1;
    if(send(child_sock, &len_string, sizeof(len_string), 0) < 0){
        perror("Error on sending buffer length");
        close(child_sock);
        exit(1);
    }
    if(send(child_sock, buffer, len_string, 0) < 0){
        perror("Error on sending the buffer");
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
        perror("Error on receiving ");
        close(child_sock);
        exit(1);
    }
    if(recv(child_sock, buffer, len_string, 0) < 0){
        perror("Error on receiving USER");
    	close(child_sock);
        exit(1);
    }    	
    user_string = strtok(buffer, " ");
    username = strtok(NULL, "\n");
    username = strdup(username); /* con strdup copio lo username in un'altra zona di memoria così posso memsettare il buffer
                                  * (altrimenti memsettando il buffer perdevo anche 'username' perchè strtok ritorna un puntatore a quella stringa.
                                  * Bisogna ricordarsi 'free(USED-BY-STRDUP)') */
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE NOME UTENTE *************************/

    /************************* RICEVIAMO PASSWORD E MANDIAMO CONFERMA *************************/
    if(recv(child_sock, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
        perror("Error on receiving buffer length");
        close(child_sock);
        exit(1);
    }
    if(recv(child_sock, buffer, len_string, 0) < 0){
        perror("Error on receiving PASS");
    	close(child_sock);
        exit(1);
    }
    pass_string = strtok(buffer, " ");
    password = strtok(NULL, "\n");
    password = strdup(password);
    memset(buffer, 0, sizeof(buffer));
    /************************* FINE PASSWORD *************************/
    	
    /************************* INVIO CONFERMA LOG IN *************************/
    login = check_login_details(username, password);
    if(login != 0){
        strcpy(buffer, "Wrong USER or PASS");
        len_string = strlen(buffer)+1;
        if(send(child_sock, &len_string, sizeof(len_string), 0) < 0){
            perror("Error on sending buffer length");
            close(child_sock);
            exit(1);
        }
        if(send(child_sock, buffer, len_string, 0) < 0){
            perror("Error on sending buffer");
            close(child_sock);
            exit(1);
        }
        close(child_sock);
        exit(1);
    }
    printf("USER: %s - PASS: ******\n", username);
    sprintf(buffer, "230 USER %s logged in", username);
    len_string = strlen(buffer)+1;
    if(send(child_sock, &len_string, sizeof(len_string), 0) < 0){
        perror("Error on sending buffer length");
        close(child_sock);
        exit(1);
    }
    if(send(child_sock, buffer, strlen(buffer), 0) < 0){
        perror("Error on sending buffer");
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
        perror("Error on receiving buffer length");
        close(child_sock);
        exit(1);
    }
    if(recv(child_sock, buffer, len_string, 0) < 0){
        perror("Error on receiving buffer\n");
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
    ret_val = do_server_fork_syst_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE SYST *************************/

    /************************* INIZIO AZIONE LIST *************************/
    exec_list:
    ret_val = do_server_fork_list_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE LIST *************************/

    /************************* INIZIO AZIONE PWD *************************/
    exec_pwd:
    ret_val = do_server_fork_pwd_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE PWD *************************/

    /************************* INIZIO AZIONE CWD *************************/
    exec_cwd:
    ret_val = do_server_fork_cwd_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE CWD *************************/

    /************************* INIZIO AZIONE RETR *************************/
    exec_retr:
    ret_val = do_server_fork_retr_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE RETR *************************/

    /************************* INIZIO AZIONE STOR *************************/
    exec_stor:
    ret_val = do_server_fork_stor_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE STOR *************************/

    /************************* INIZIO AZIONE DELE *************************/
    exec_delete:
    ret_val = do_server_fork_dele_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE DELE *************************/

    /************************* INIZIO AZIONE MKD *************************/
    exec_mkdir:
    ret_val = do_server_fork_mkd_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE MKD *************************/

    /************************* INIZIO AZIONE RMD *************************/
    exec_rmdir:
    ret_val = do_server_fork_rmd_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE RMD *************************/

    /************************* INIZIO AZIONE RNM *************************/
    exec_rename:
    ret_val = do_server_fork_rnm_cmd(child_sock);
    server_errors_handler(child_sock, ret_val);
    goto exec_listen_action;
    /************************* FINE AZIONE RNM *************************/

    /************************* INIZIO SALUTO FINALE *************************/
    send_goodbye:
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "221 Goodbye\n");
    if(send(child_sock, buffer, strlen(buffer), 0) < 0){
        perror("Error on receiving 221 message");
        close(child_sock);
        exit(1);
    }
    if(chdir(serverdir) < 0){
        perror("chdir on default dir");
    }
    memset(buffer, 0, sizeof(buffer));
    printf("Received EXIT request\n");
    close(child_sock);
    /************************* FINE SALUTO FINALE *************************/
}

void check_before_start(int argc, char *argv[]){
	if(argc != 2){
		fprintf(stdout, "Usage: %s <port number>\n", argv[0]);
		exit(1);
	}
}

void sig_handler(const int signo, const int sockd, const int newsockd){
  if (signo == SIGINT){
    printf("Received SIGINT, exiting...\n");
    if(newsockd) close(newsockd);
    if(sockd) close(sockd);
    exit(EXIT_SUCCESS);
  }
}
