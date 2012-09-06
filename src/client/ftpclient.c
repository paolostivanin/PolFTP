/* Descrizione: Semplice client FTP sviluppato per il progetto di Reti di Calcolatori
 * Sviluppatore: Paolo Stivanin
 * Versione: 1.0-beta1
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
#include "../prototypes.h"

#define BUFFGETS 255
#define VERSION "1.0-beta1"

struct info{
  char *user, *pass, *filename, *conferma, *filebuffer, *scelta;
};

int main(int argc, char *argv[]){
  
  check_before_start(argc, argv);

  int sockd = -1, i = 0, ret_val = -1; /* descrittore del socket, file, contatore e valore di ritorno delle funzioni */
  uint32_t len_string;
  int NumPorta = atoi(argv[2]); /* numero di porta */
  char *is_err = NULL;
  static struct sockaddr_in serv_addr; /* struttura contenente indirizzo del server */
  static struct termios oldt, newt; /* struttura contenente i paramentri della shell */
  static struct hostent *hp; /* la struttura hostent mi servirà per l'indirizzo ip del server */
  static struct info sInfo; /* struttura che contiene alcuni dati utili al programma */
  static char buffer[256], expected_string[128]; /*buffer usato per contenere vari dati */
  
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
  if(recv(sockd, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
    perror("Errore ricezione len buffer");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, len_string, 0) < 0){
    perror("Errore nella ricezione del messaggio di benvenuto\n");
    onexit(sockd, 0, 0, 1);
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
  len_string = strlen(buffer)+1;
  if(send(sockd, &len_string, sizeof(len_string), 0) < 0){
    perror("Errore invio len buffer user");
    onexit(sockd, 0, 0, 1);
  }
  if(send(sockd, buffer, len_string, 0) < 0){
    perror("Errore durante l'invio di USER");
    onexit(sockd, 0, 0, 1);
  }
  memset(buffer, 0, sizeof(buffer));
  /************************* FINE NOME UTENTE *************************/
  
  /************************* INVIO PASSWORD E RICEVO CONFERMA *************************/
  sprintf(buffer, "PASS %s\n", sInfo.pass);
  len_string = strlen(buffer)+1;
  if(send(sockd, &len_string, sizeof(len_string), 0) < 0){
    perror("Errore invio len pass buffer");
    onexit(sockd, 0, 0, 1);
  }
  if(send(sockd, buffer, len_string, 0) < 0){
    perror("Errore invio pass buffer");
    onexit(sockd, 0, 0, 1);
  }
  memset(buffer, 0, sizeof(buffer));
  /************************* FINE PASSWORD *************************/

  /************************* RICEZIONE CONFERMA LOG IN *************************/
  if(recv(sockd, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
    perror("Errore ricezione len buffer conferma login");
    onexit(sockd, 0, 0, 1);
  }
  if(recv(sockd, buffer, len_string, 0) < 0){
    perror("Errore nella ricezione dellaa conferma LOG IN");
    onexit(sockd, 0, 0, 1);
  }
  sInfo.conferma = strtok(buffer, "\0");
  sprintf(expected_string, "230 USER %s logged in", sInfo.user);
  if(strcmp(sInfo.conferma, expected_string) != 0){
    printf("%s\n", sInfo.conferma);
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
  if((strcmp("STOR", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("DELE", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("MKD", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("RMD", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("RNM", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("EXIT", sInfo.scelta) == 0)) goto prepara;
  if((strcmp("HELP", sInfo.scelta) == 0)) goto prepara;
  printf("Comando errato. Scrivere HELP per l'aiuto.\n"); goto exec_switch;

  prepara:
  if(strcmp(sInfo.scelta, "HELP") == 0) goto exec_help;
  strcpy(buffer, sInfo.scelta);
  len_string = strlen(buffer)+1;
  if(send(sockd, &len_string, sizeof(len_string), 0) < 0){
    perror("Errore invio len buffer scelta");
    onexit(sockd, 0, 0, 1);
  }
  if(send(sockd, buffer, len_string, 0) < 0){
    perror("Errore durante l'invio azione");
    onexit(sockd, 0, 0, 1);
  }
  
  if(strcmp(sInfo.scelta, "SYST") == 0){ free(sInfo.scelta); goto exec_syst; }
  if(strcmp(sInfo.scelta, "LIST") == 0){ free(sInfo.scelta); goto exec_list; }
  if(strcmp(sInfo.scelta, "PWD") == 0){ free(sInfo.scelta); goto exec_pwd; }
  if(strcmp(sInfo.scelta, "CWD") == 0){ free(sInfo.scelta); goto exec_cwd; }
  if(strcmp(sInfo.scelta, "RETR") == 0){ free(sInfo.scelta); goto exec_retr; }
  if(strcmp(sInfo.scelta, "STOR") == 0){ free(sInfo.scelta); goto exec_stor; }
  if(strcmp(sInfo.scelta, "DELE") == 0){ free(sInfo.scelta); goto exec_delete; }
  if(strcmp(sInfo.scelta, "MKD") == 0){ free(sInfo.scelta); goto exec_mkdir; }
  if(strcmp(sInfo.scelta, "RMD") == 0){ free(sInfo.scelta); goto exec_rmdir; }
  if(strcmp(sInfo.scelta, "RNM") == 0){ free(sInfo.scelta); goto exec_rename; }
  if(strcmp(sInfo.scelta, "EXIT") == 0){ free(sInfo.scelta); goto exec_exit; }

  exec_help:
  printf("FTPUtils (Client) %s developed by Paolo Stivanin\n", VERSION);
  printf("\nAvailable commands are:\nSYST - LIST - PWD - CWD - RETR - STOR - DELE - MKD - RMD - RNM - STOR - EXIT - HELP\n");
  free(sInfo.scelta);
  goto exec_switch;
  /************************* FINE PARTE AZIONE UTENTE *************************/

  /************************* INIZIO AZIONE SYST *************************/
  exec_syst:
  ret_val = do_syst_cmd(sockd);
  /* invio al server la stringa ERR o OKK */
  /*if(ret_val < 0){
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "ERR");
    if(send(sockd, buffer, 4, 0) < 0){
      perror("Error on sending the syst retval")
      exit(1);
    }
    goto exec_switch;
  }
  else{
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "OKK");
    if(send(sockd, buffer, 4, sizeof(buffer)) < 0){
      perror("Error on sending the syst retval");
      exit(1);
    }
  }*/
  /* --- */
  /* aspetto conferma dal server di OKK o ERR */
  /*memset(buffer, 0, sizeof(buffer));
  if(recv(sockd, buffer, 4, sizeof(buffer), MSG_WAITALL) < 0){
    perror("Error on recv syst retval");
    exit(1);
  }
  is_err = strtok(buffer, "\0");
  if(strcmp(is_err, "ERR") == 0){
    printf("An error occured on the server");
    goto exec_switch;
  }*/
  /* --- */
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE SYST *************************/

  /************************* INIZIO RICHIESTA LIST *************************/
  exec_list:
  ret_val = do_list_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE LIST *************************/

  /************************* INIZIO AZIONE PWD *************************/
  exec_pwd:
  ret_val = do_pwd_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE PWD *************************/

  /************************* INIZIO AZIONE CWD *************************/
  exec_cwd:
  ret_val = do_cwd_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE CWD *************************/

  /************************* INIZIO AZIONE RETR *************************/
  exec_retr:
  ret_val = do_retr_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE RETR *************************/

  /************************* INIZIO AZIONE STOR *************************/
  exec_stor:
  ret_val = do_stor_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE STOR *************************/

  /************************* INIZIO AZIONE DELE *************************/
  exec_delete:
  ret_val = do_dele_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE DELE *************************/

  /************************* INIZIO AZIONE MKD *************************/
  exec_mkdir:
  ret_val = do_mkd_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE MKD *************************/

  /************************* INIZIO RICHIESTA RMD *************************/
  exec_rmdir:
  ret_val = do_rmd_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE RMD *************************/

  /************************* INIZIO AZIONE RNM *************************/
  exec_rename:
  ret_val = do_rnm_cmd(sockd);
  client_errors_handler(sockd, ret_val);
  goto exec_switch;
  /************************* FINE AZIONE RNM *************************/

  /************************* INIZIO SALUTO FINALE *************************/
  exec_exit:
  memset(buffer, 0, sizeof(buffer));
  if(recv(sockd, buffer, 12, 0) < 0){
    perror("Errore ricezione 221");
    onexit(sockd, 0, 0, 1);
  }
  printf("%s", buffer);
  close(sockd);
  /************************* FINE SALUTO FINALE *************************/

  return EXIT_SUCCESS;
}


void check_before_start(int argc, char *argv[]){
  if(argc != 3){
    printf("Uso: %s <hostname> <numero porta>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
}