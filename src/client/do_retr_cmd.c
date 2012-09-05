#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h> /* per usare uint32_t invece di size_t */
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include "../prototypes.h"

#define BUFFGETS 255

int do_retr_cmd(const int f_sockd){
  int fd;
  ssize_t nread = 0;
  uint32_t fsize, fsize_tmp, total_bytes_read, size_to_receive, fn_len;
  char *filename = NULL, *conferma = NULL;
  void *filebuffer = NULL;
  char buf[256], dirp[256];
  
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nome del file da scaricare: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    return -1;
  }
  filename = NULL;
  filename = strtok(dirp, "\n");
  fn_len = 0;
  fn_len = strlen(filename)+1;
  if(send(f_sockd, &fn_len, sizeof(fn_len), 0) < 0){
    perror("Errore durante l'invio della lunghezza del nome del file");
    return -1;
  }
  sprintf(buf, "RETR %s", filename);
  if(send(f_sockd, buf, fn_len+5, 0) < 0){
    perror("Errore durante l'invio del nome del file");
    return -1;
  }
  if(recv(f_sockd, buf, 3, 0) < 0){
    perror("Errore ricezione conferma file");
    return -1;
  }
  conferma = NULL;
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "NO") == 0){
    printf("ERRORE: il file richiesto non esiste\n");
    return -1;
  }
  recv(f_sockd, &fsize, sizeof(fsize), MSG_WAITALL);
  fd = open(filename, O_CREAT | O_WRONLY, 0644);
  if (fd  < 0) {
    perror("open");
    return -1;
  }
  fsize_tmp = fsize;
  filebuffer = malloc(fsize);
  if(filebuffer == NULL){
    perror("malloc");
    close(fd);
    return -1;
  }
  total_bytes_read = 0;
  nread = 0;
  for(size_to_receive = fsize; size_to_receive > 0;){
    nread = read(f_sockd, filebuffer, size_to_receive);
    if(nread < 0){
      perror("read error on retr");
      close(fd);
      return -1;
    }
    if(write(fd, filebuffer, nread) != nread){
      perror("write error on retr");
      close(fd);
      return -1;
    }
    size_to_receive -= nread;
  }
  close(fd); /* la chiusura del file va qui altrimenti client entra in loop infinito e si scrive all'interno del file */
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 33, 0) < 0){
    perror("Errore ricezione 226");
    return -1;
  }
  printf("%.32s\n", buf);
  memset(buf, 0, sizeof(buf));
  memset(dirp, 0, sizeof(dirp));
  free(filebuffer);
  return 0;
}