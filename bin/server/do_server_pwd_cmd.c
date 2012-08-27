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

void do_server_pwd_cmd(f_sockd, m_sockd){
  char *other;
  char buf[256];
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 4, 0) < 0){
    perror("Errore nella ricezione comando PWD");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  other = strtok(buf, "\n");
  if(strcmp(other, "PWD") == 0){
    printf("Ricevuta richiesta PWD\n");
  } else onexit(f_sockd, m_sockd, 0, 2); 
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "PWD: %s\n", (char *)(intptr_t)get_current_dir_name());
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio PWD");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
}