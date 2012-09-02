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
#include <fcntl.h>
#include <dirent.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../prototypes.h"

void do_server_rnm_cmd(const int f_sockd, const int m_sockd){
  uint32_t len_fname_todelete = 0;
  int renameFile = 0;
  char buf[256], rnbuf[256];
  char *other = NULL, *filename = NULL;

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &len_fname_todelete, sizeof(len_fname_todelete), MSG_WAITALL) < 0){
    perror("Errore ricezione len fname");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  if(recv(f_sockd, buf, len_fname_todelete+5, 0) < 0){
    perror("Errore nella ricezione RNFR");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");
  strcpy(rnbuf, filename);
  
  if(strcmp(other, "RNFR") == 0){
    printf("Ricevuta richiesta RNFR\n");
  } else onexit(f_sockd, m_sockd, 0, 2);

  memset(buf, 0, sizeof(buf));
  len_fname_todelete = 0;
  if(recv(f_sockd, &len_fname_todelete, sizeof(len_fname_todelete), MSG_WAITALL) < 0){
    perror("Errore ricezione len fname");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  if(recv(f_sockd, buf, len_fname_todelete+5, 0) < 0){
    perror("Errore nella ricezione RNTO");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");
  if(strcmp(other, "RNTO") == 0){
    printf("Ricevuta richiesta RNTO\n");
  } else onexit(f_sockd, m_sockd, 0, 2);
  renameFile = rename(rnbuf, filename);
  if(renameFile == 0){
    strcpy(buf, "OK");
  } else { strcpy(buf, "NO"); }
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio rnm conferma");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
}