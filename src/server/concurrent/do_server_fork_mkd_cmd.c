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
#include <sys/stat.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../../prototypes.h"

void do_server_fork_mkd_cmd(const int f_sockd){  
  uint32_t server_dir_name_len = 0;
  char *server_new_dir_name = NULL, *other = NULL;
  char buf[256];

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &server_dir_name_len, sizeof(server_dir_name_len), MSG_WAITALL) < 0){
    perror("Errore ricezione lunghezza dirname");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, server_dir_name_len+4, 0) < 0){
    perror("Errore nella ricezione del nome della cartella");
    onexit(f_sockd, 0, 0, 1);
  }
  other = strtok(buf, " ");
  server_new_dir_name = strtok(NULL, "\n");
  if(strcmp(other, "MKD") == 0){
    printf("Ricevuta richiesta MKDIR\n");
  } else onexit(f_sockd, 0, 0, 1);
  
  if(mkdir(server_new_dir_name, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0){
    fprintf(stderr, "Impossibile creare la cartella '%s'\n", server_new_dir_name);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, 0, 0, 1);
    }
    onexit(f_sockd, 0, 0, 1);
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
}