#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <termios.h>
#include "../prototypes.h"

#define BUFFGETS 255

struct info{
  char *user, *pass, *filename, *conferma, *filebuffer, *scelta;
};

int main(int argc, char *argv[]){
  
  check_before_start(argc, argv);

  int sockd = -1, i = 0, ret_val = -1;
  uint32_t len_string;
  int NumPorta = atoi(argv[2]);
  char *is_err = NULL;
  static struct sockaddr_in serv_addr;
  static struct termios oldt, newt;
  static struct hostent *hp;
  static struct info sInfo;
  static char buffer[BUFFGETS], expected_string[128];
  
  if(NumPorta < 1 || NumPorta > 65535){
	  fprintf(stderr, "Port number must be between 1 and 65535\n");
	  return -1;
  }
  
  hp = gethostbyname(argv[1]);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(NumPorta);
  serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr;

  if((sockd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Error during socket creation");
    return -1;
  }

  if(connect(sockd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
    perror("Connection error");
    close(sockd);
    return -1;
  }
  /************************* MESSAGGIO DI BENVENUTO *************************/
  memset(buffer, 0, sizeof(buffer));
  if(recv(sockd, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
    perror("Error on receiving the buffer length");
    close(sockd);
    return -1;
  }
  if(recv(sockd, buffer, len_string, 0) < 0){
    perror("Error on receiving the 'Welcome' message\n");
    close(sockd);
    return -1;
   }
  printf("%s\n", buffer);
  memset(buffer, 0, sizeof(buffer));
  /************************* FINE MESSAGGIO DI BENVENUTO *************************/

  /************************* INIZIO PARTE LOGIN *************************/
  /************************* INVIO NOME UTENTE E RICEVO CONFERMA *************************/
  /* salvo i settaggi attuali di STDIN_FILENO ed assegno a newt i valore di oldt*/
  tcgetattr( STDIN_FILENO, &oldt);
  newt = oldt;

  printf("USER: ");
  /* Which will read everything up to the newline into the string you pass in, then will consume
   * a single character (the newline) without assigning it to anything (that '*' is 'assignment suppression'). */
  if(scanf("%m[^\n]%*c", &sInfo.user) == EOF){
    perror("scanf user");
    close(sockd);
    return -1;
  }
  printf("PASS: ");
  /* imposto il bit appropriato nella struttura newt */
  newt.c_lflag &= ~(ECHO); 
  /* imposto il nuovo bit nell'attuale STDIN_FILENO */
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  if(scanf("%m[^\n]%*c", &sInfo.pass) == EOF){
    perror("scanf user");
    close(sockd);
    return -1;
  }
  /* resetto con oldt l'attuale STDIN_FILENO*/ 
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
  printf("\n");
  sprintf(buffer, "USER %s\n", sInfo.user);
  len_string = strlen(buffer)+1;
  if(send(sockd, &len_string, sizeof(len_string), 0) < 0){
    perror("Error on sending the username length");
    close(sockd);
    return -1;
  }
  if(send(sockd, buffer, len_string, 0) < 0){
    perror("Error on sending the username");
    close(sockd);
    return -1;
  }
  memset(buffer, 0, sizeof(buffer));
  /************************* FINE NOME UTENTE *************************/
  
  /************************* INVIO PASSWORD E RICEVO CONFERMA *************************/
  sprintf(buffer, "PASS %s\n", sInfo.pass);
  len_string = strlen(buffer)+1;
  if(send(sockd, &len_string, sizeof(len_string), 0) < 0){
    perror("Error on sending the password length");
    close(sockd);
    return -1;
  }
  if(send(sockd, buffer, len_string, 0) < 0){
    perror("Error on sending password");
    close(sockd);
    return -1;
  }
  memset(buffer, 0, sizeof(buffer));
  /************************* FINE PASSWORD *************************/

  /************************* RICEZIONE CONFERMA LOG IN *************************/
  if(recv(sockd, &len_string, sizeof(len_string), MSG_WAITALL) < 0){
    perror("Error on sending LOG IN length");
    close(sockd);
    return -1;
  }
  if(recv(sockd, buffer, len_string, 0) < 0){
    perror("Error on receiving LOG IN confirmation");
    close(sockd);
    return -1;
  }
  sInfo.conferma = strtok(buffer, "\0");
  sprintf(expected_string, "230 USER %s logged in", sInfo.user);
  if(strcmp(sInfo.conferma, expected_string) != 0){
    printf("%s\n", sInfo.conferma);
    close(sockd);
    return -1;
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
    perror("scanf error");
    close(sockd);
    return -1;
  }
  for(i=0; i<(int)strlen(sInfo.scelta); i++){
    if(isalpha((unsigned char)sInfo.scelta[i])){
      if(islower((unsigned char)sInfo.scelta[i])){
          sInfo.scelta[i] = toupper((unsigned char)sInfo.scelta[i]);
      }
    }
    else{
      printf("%c isn't a char\n", sInfo.scelta[i]);
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
  printf("Wrong command. Type HELP to see the list of available commands.\n"); goto exec_switch;

  prepara:
  if(strcmp(sInfo.scelta, "HELP") == 0) goto exec_help;
  strcpy(buffer, sInfo.scelta);
  len_string = strlen(buffer)+1;
  if(send(sockd, &len_string, sizeof(len_string), 0) < 0){
    perror("Error on sending the buffer length");
    close(sockd);
    return -1;
  }
  if(send(sockd, buffer, len_string, 0) < 0){
    perror("Error on sending the action");
    close(sockd);
    return -1;
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
    perror("Error on receiving 221 goodbye");
    close(sockd);
    return -1;
  }
  printf("%s", buffer);
  close(sockd);
  /************************* FINE SALUTO FINALE *************************/

  return EXIT_SUCCESS;
}


void check_before_start(int argc, char *argv[]){
  if(argc != 3){
    printf("Usage: %s <hostname> <port number>\n", argv[0]);
	return -1;
  }
}
