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

void do_retr_cmd(int f_sockd){
  int fd, total_bytes_read;
  uint32_t fsize, nread = 0, fsize_tmp;
  char *filename = NULL, *conferma = NULL, *filebuffer = NULL;
  char buf[256], dirp[256], t_buf[256];
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  printf("Inserire il nome del file da scaricare: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  filename = NULL;
  filename = strtok(dirp, "\n");
  sprintf(buf, "RETR %s", dirp);
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore ricezione conferma file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  fsize = 0;
  conferma = NULL;
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "ERRORE: File non esistente") == 0){
    printf("ERRORE: il file richiesto non esiste\n");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, t_buf, sizeof(t_buf), 0) < 0){
    perror("Errore nella ricezione della grandezza del file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  fsize = atoi(t_buf);
  fflush(stdout);
  fd = open(filename, O_CREAT | O_WRONLY, 0644);
  if (fd  < 0) {
    perror("open");
    onexit(f_sockd, 0 ,0 ,1);
  }
  fsize_tmp = fsize;
  filebuffer = malloc(fsize);
  if(filebuffer == NULL){
    perror("malloc");
    onexit(f_sockd, 0, fd, 4);
  }
  total_bytes_read = 0;
  nread = 0;
  while(((uint32_t)total_bytes_read != fsize) && ((nread = read(f_sockd, filebuffer, fsize_tmp)) > 0)){
    if(write(fd, filebuffer, nread) != nread){
      perror("write RETR");
      onexit(f_sockd, 0, 0, 1);
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
  }
  close(fd); /* la chiusura del file va qui altrimenti client entra in loop infinito e si scrive all'interno del file */
  fflush(stdin);
  fflush(stdout);
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 34, 0) < 0){
    perror("Errore ricezione 226");
    onexit(f_sockd, 0, 0, 1);
  }
  printf("%s", buf);
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  memset(dirp, 0, sizeof(dirp));
  free(filebuffer);
}