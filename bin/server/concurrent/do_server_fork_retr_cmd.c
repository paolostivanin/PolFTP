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
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../../prototypes.h"

void do_server_fork_retr_cmd(const int f_sockd){
  int fd, rc;
  uint32_t fsize, size_to_send, fn_size;
  char *filename = NULL, *other = NULL;
  char buf[512];
  off_t offset;
  struct stat fileStat;

  memset(buf, 0, sizeof(buf));
  fn_size = 0;
  if(recv(f_sockd, &fn_size, sizeof(fn_size), MSG_WAITALL) < 0){
    perror("Errore durante la ricezione della lunghezza del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, fn_size+5, 0) < 0){
    perror("Errore nella ricezione del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");

  if(strcmp(other, "RETR") == 0){
    printf("Ricevuta richiesta RETR\n");
  } else onexit(f_sockd, 0, 0, 1);

  fd = open(filename, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "Impossibile aprire il file '%s'\n", filename);
    strcpy(buf, "NO\0");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, 0, 0 ,1);
    }
    onexit(f_sockd, 0, 0, 1);
  }
  strcpy(buf, "OK\0");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, 0, 0 ,1);
  }
  fsize = 0;
  fileStat.st_size = 0;
  if(fstat(fd, &fileStat) < 0){
  perror("Errore fstat");
    onexit(f_sockd, 0, fd, 4);
  }
  fsize = fileStat.st_size;
  if(send(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Errore durante l'invio della grandezza del file\n");
    onexit(f_sockd, 0, fd, 4);
  }
  offset = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc = sendfile(f_sockd, fd, &offset, size_to_send);
    if (rc <= 0){
      perror("sendfile");
      onexit(f_sockd, 0, fd, 4);
    }
    size_to_send -= rc;
  }
  close(fd); /* la chiusura del file va qui altrimenti rischio loop infinito e scrittura all'interno del file */
  memset(buf, 0, sizeof(buf));
  strcpy(buf, "226 File trasferito con successo");
  if(send(f_sockd, buf, 33, 0) < 0){
    perror("Errore durante l'invio 226");
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
}