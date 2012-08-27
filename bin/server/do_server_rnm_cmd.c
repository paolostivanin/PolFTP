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

void do_server_rnm_cmd(f_sockd, m_sockd){
  int renameFile = 0;
  char buf[256], rnbuf[256];
  char *other, *filename;
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore nella ricezione RNFR");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\n");
  strcpy(rnbuf, filename);
  
  if(strcmp(other, "RNFR") == 0){
    printf("Ricevuta richiesta RNFR\n");
  } else onexit(f_sockd, m_sockd, 0, 2);

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore nella ricezione RNTO");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\n");
  if(strcmp(other, "RNTO") == 0){
    printf("Ricevuta richiesta RNTO\n");
  } else onexit(f_sockd, m_sockd, 0, 2);
  renameFile = rename(rnbuf, filename);
  if(renameFile == 0){
    strcpy(buf, "isokrnm\0");
  } else { strcpy(buf, "failrnm\0"); }
  if(send(f_sockd, buf, 8, 0) < 0){
    perror("Errore durante invio rnm conferma");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
}