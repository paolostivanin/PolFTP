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

void do_server_retr_cmd(f_sockd, m_sockd){
  int fd;
  uint32_t fsize, size_to_send, rc;
  char *filename = NULL, *other = NULL;
  char buf[256], t_buf[256];
  off_t offset;
  struct stat fileStat;

  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore nella ricezione del nome del file");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\n");

  if(strcmp(other, "RETR") == 0){
    printf("Ricevuta richiesta RETR\n");
  } else onexit(f_sockd, m_sockd, 0, 2);

  fd = open(filename, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    strcpy(buf, "ERRORE: File non esistente\0");
    if(send(f_sockd, buf, strlen(buf), 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, m_sockd, 0 ,2);
    }
    onexit(f_sockd, m_sockd, 0, 2);
  }
  strcpy(buf, "OK\0");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  if(fstat(fd, &fileStat) < 0){
  perror("Errore fstat");
    onexit(f_sockd, m_sockd, fd, 3);
  }
  fsize = fileStat.st_size;
  snprintf(t_buf, BUFSIZ-1, "%" PRIu32, fsize);
  if(send(f_sockd, t_buf, sizeof(t_buf), 0) < 0){
    perror("Errore durante l'invio della grandezza del file\n");
    onexit(f_sockd, m_sockd, fd, 3);
  }
  offset = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc = sendfile(f_sockd, fd, &offset, size_to_send);
    if (rc <= 0){
      perror("sendfile");
      onexit(f_sockd, m_sockd, fd, 3);
    }
    size_to_send -= rc;
  }
  close(fd); /* la chiusura del file va qui altrimenti rischio loop infinito e scrittura all'interno del file */
  memset(buf, 0, sizeof(buf));
  strcpy(buf, "226 File trasferito con successo\n");
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio 226");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
}