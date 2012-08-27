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

void do_server_dele_cmd(f_sockd, m_sockd){
  int fd, retval;
  char buf[256];
  char *other = NULL, *filename = NULL;
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore nella ricezione del nome del file");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\n");
  if(strcmp(other, "DELE") == 0){
    printf("Ricevuta richiesta DELETE\n");
  } else onexit(f_sockd, m_sockd, 0, 2);
  
  fd = open(filename, O_WRONLY);
  if(fd < 0){
    fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    strcpy(buf, "ERRORE: File non esistente\0");
    if(send(f_sockd, buf, strlen(buf), 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, m_sockd, 0, 2);
    }
    onexit(f_sockd, m_sockd, 0, 2);
  }
  retval = remove(filename);
  if(retval != 0){
    perror("File non cancellato");
    strcpy(buf, "NONOK\0");
    if(send(f_sockd, buf, strlen(buf), 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, m_sockd, fd, 3);
    }
    onexit(f_sockd, m_sockd, fd, 3);
  }
  strcpy(buf, "OK\0");
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  memset(buf, 0, sizeof(buf));
}