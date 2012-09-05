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
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../prototypes.h"

void do_server_syst_cmd(const int f_sockd, const int m_sockd){
  char buf[256];
  char *sysname = NULL, *other = NULL;
  uint32_t buf_len = 0;

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 5, 0) < 0){
    perror("Errore ricezione comando SYST");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  other = strtok(buf, "\0");
  if(strcmp(other, "SYST") == 0){
    printf("Ricevuta richiesta SYST\n");
  } else onexit(f_sockd, m_sockd, 0, 2); 
  get_syst(&sysname);
  sprintf(buf, "%s", sysname);
  buf_len = strlen(buf)+1;
  if(send(f_sockd, &buf_len, sizeof(buf_len), 0) < 0){
    perror("Errore durante invio lunghezza buffer");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  if(send(f_sockd, buf, buf_len, 0) < 0){
    perror("Errore durante l'invio risposta SYST");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
}
