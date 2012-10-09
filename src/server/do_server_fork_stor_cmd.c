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

int do_server_fork_stor_cmd(const int f_sockd){
  int fd;
  ssize_t nread;
  uint32_t fsize, fsize_tmp, total_bytes_read = 0, fn_size = 0;
  char *stor_filename = NULL, *other = NULL;
  void *filebuffer = NULL;
  char buf[256];

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &fn_size, sizeof(fn_size), MSG_WAITALL) < 0){
    perror("Error on receiving the file name length");
    return -1;
  }
  if(recv(f_sockd, buf, fn_size+5, 0) < 0){
    perror("Error on receiving the file name");
    return -1;
  }
  other = NULL;
  stor_filename = NULL;
  other = strtok(buf, " ");
  stor_filename = strtok(NULL, "\0");
  stor_filename = strdup(stor_filename);

  if(strcmp(other, "STOR") == 0){
    printf("Received STOR request\n");
  } else return -1;

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 3, 0) < 0){
    perror("Error on receiving the buffer");
    return -1;
  }    

  other = NULL;
  other = strtok(buf, "\0");
  if(strcmp(other, "NO") == 0){
    printf("ERROR: requested file doesn't exist\n");
    return -1;
  }

  fsize = 0;
  if(recv(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Error on receiving the file size");
    return -1;
  }
  fd = open(stor_filename, O_CREAT | O_WRONLY, 0644);
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
  while((total_bytes_read != fsize) && ((nread = read(f_sockd, filebuffer, fsize_tmp)) > 0)){
    if(write(fd, filebuffer, nread) != nread){
      perror("write RETR");
      close(fd);
      return -1;
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
  }
  close(fd); /* la chiusura del file va qui altrimenti client entra in loop infinito e si scrive all'interno del file */

  memset(buf, 0, sizeof(buf));
  strcpy(buf, "226 File successfully transferred");
  if(send(f_sockd, buf, 34, 0) < 0){
    perror("Error on sending the upload confirmation");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  free(filebuffer);
  free(stor_filename);
  return 0;
}