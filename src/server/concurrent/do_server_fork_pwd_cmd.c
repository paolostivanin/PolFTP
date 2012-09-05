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
#include "../../prototypes.h"

int do_server_fork_pwd_cmd(const int f_sockd){
  char *other;
  char buf[256];
  uint32_t pwd_buf_len = 0;

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 4, 0) < 0){
    perror("Errore nella ricezione comando PWD");
    return -1;
  }
  other = NULL;
  other = strtok(buf, "\n");
  if(strcmp(other, "PWD") == 0){
    printf("Ricevuta richiesta PWD\n");
  } else return -1; 
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "PWD: %s", (char *)(intptr_t)get_current_dir_name());
  pwd_buf_len = strlen(buf)+1;
  if(send(f_sockd, &pwd_buf_len, sizeof(pwd_buf_len), 0) < 0){
    perror("Errore invio lunghezza buffer");
    return -1;
  }
  if(send(f_sockd, buf, pwd_buf_len, 0) < 0){
    perror("Errore durante l'invio PWD");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}