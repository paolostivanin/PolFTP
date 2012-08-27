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

void do_list_cmd(int f_sockd){
  int fd, total_bytes_read = 0;
  uint32_t fsize, nread = 0, fsize_tmp;
  char *filebuffer = NULL;
  char c, buf[256], t_buf[256];
  FILE *fp;
  
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  strcpy(buf, "LIST\n");
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio richiesta LIST");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, t_buf, sizeof(t_buf), 0) < 0){
    perror("Errore nella ricezione della grandezza del file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  fsize = atoi(t_buf);
  if((fd = open("listfiles.txt", O_CREAT | O_WRONLY,0644)) < 0){
    perror("open file list");
    onexit(f_sockd, 0, 0, 1);
  }
  fsize_tmp = fsize;
  filebuffer = malloc(fsize);
  if(filebuffer == NULL){
    perror("malloc");
    onexit(f_sockd, 0, fd, 4);
  }
  while(((uint32_t)total_bytes_read != fsize) && ((nread = read(f_sockd, filebuffer, fsize_tmp)) > 0)){
    if(write(fd, filebuffer, nread) != nread){
      perror("write list file");
      onexit(f_sockd, 0, 0, 1);
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
  }
  close(fd);
  fflush(stdin);
  fflush(stdout);
  printf("----- FILE LIST -----\n");
  if((fp = fopen("listfiles.txt", "r+")) == NULL){
    perror("open file for read");
    onexit(f_sockd, 0, 0, 1);
  }
  while((c=getc(fp)) != EOF){
    putchar(c);
  }
  fclose(fp);
  printf("----- END FILE LIST -----\n");
  if(remove( "listfiles.txt" ) == -1 ){
    perror("errore cancellazione file");
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  free(filebuffer);
  fflush(stdout);
}