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

void do_server_rmd_cmd(const int f_sockd, const int m_sockd){
  uint32_t len_server_dirname_todelete;
  char *other = NULL, *filename = NULL;
  char buf[256];
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &len_server_dirname_todelete, sizeof(len_server_dirname_todelete), MSG_WAITALL) < 0){
    perror("Errore ricezione lunghezza dirname da eliminare");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  if(recv(f_sockd, buf, len_server_dirname_todelete+4, 0) < 0){
    perror("Errore nella ricezione del nome della cartella");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");
  if(strcmp(other, "RMD") == 0){
    printf("Ricevuta richiesta RMDIR\n");
  } else onexit(f_sockd, m_sockd, 0, 2);
  
  if(rmdir(filename) != 0){
    fprintf(stderr, "Impossibile eliminare la cartella '%s'\n", filename);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, m_sockd, 0, 2);
    }
    onexit(f_sockd, m_sockd, 0, 2);
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  memset(buf, 0, sizeof(buf));
}