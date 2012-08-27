#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h> /* per usare uint32_t invece di size_t */
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include "prototypes.h"

#define BUFFGETS 255

void do_stor_cmd(int f_sockd){
  int fd;
  struct stat fileStat;
  uint32_t rc, fsize, size_to_send;
  char *filename = NULL;
  char buf[256], dirp[256], t_buf[256];
  off_t offset;

  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  printf("Inserire il nome del file da inviare: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  filename = NULL;
  filename = strtok(dirp, "\n");
  sprintf(buf, "STOR %s", filename);
  if(send(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore invio nome file");
    onexit(f_sockd, 0, 0, 1);
  }

  fd = open(filename, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "Impossibile aprire '%s': %s\n", filename, strerror(errno));
    strcpy(buf, "ERRORE: File non esistente\0");
    if(send(f_sockd, buf, strlen(buf), 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, 0, 0, 1);
    }
    onexit(f_sockd, 0, 0, 1);
  }
  strcpy(buf, "OK\0");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, 0, 0, 1);
  }

  if(fstat(fd, &fileStat) < 0){
    perror("Errore fstat");
    onexit(f_sockd, 0, fd, 4);
  }
  fsize = fileStat.st_size;
  snprintf(t_buf, BUFSIZ-1, "%" PRIu32, fsize);
  if(send(f_sockd, t_buf, sizeof(t_buf), 0) < 0){
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
  if(recv(f_sockd, buf, 33, 0) < 0){
    perror("Errore ricezione conferma file");
    onexit(f_sockd, 0, 0, 1);
  }
  printf("%s\n", buf);
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
}