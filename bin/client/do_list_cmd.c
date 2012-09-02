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

void do_list_cmd(const int f_sockd){
  int fd;
  uint32_t fsize, fsize_tmp, total_bytes_read = 0, nread = 0;
  char *tmpfname = NULL;
  char c, buf[256], tmpname[L_tmpnam];
  FILE *fp;
  void *fbuf = NULL;
  
  memset(buf, 0, sizeof(buf));
  fsize = 0;

  strcpy(buf, "LIST");
  if(send(f_sockd, buf, 5, 0) < 0){
    perror("Errore durante l'invio richiesta LIST");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Errore nella ricezione della grandezza del file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  if(!(tmpfname = tmpnam(tmpname))){ /* genero il nome del tmpfile (tip /tmp/X6Tr4Y) */
    perror("Errore nome tmp file");
    exit(1);
  }
  if((fd = open(tmpfname, O_CREAT | O_WRONLY,0644)) < 0){
    perror("open file list");
    onexit(f_sockd, 0, 0, 1);
  }
  fsize_tmp = fsize;
  fbuf = malloc(fsize);
  if(fbuf == NULL){
    perror("malloc");
    onexit(f_sockd, 0, fd, 4);
  }
  while((total_bytes_read != fsize) && ((nread = read(f_sockd, fbuf, fsize_tmp)) > 0)){
    if(write(fd, fbuf, nread) != nread){
      perror("write list file");
      onexit(f_sockd, 0, 0, 1);
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
  }
  close(fd);
  printf("----- FILE LIST -----\n");
  if((fp = fopen(tmpfname, "r+")) == NULL){
    perror("open file for read");
    onexit(f_sockd, 0, 0, 1);
  }
  while((c=getc(fp)) != EOF){
    putchar(c);
  }
  fclose(fp);
  printf("----- END FILE LIST -----\n");
  if(remove( tmpfname ) == -1 ){
    perror("errore cancellazione file");
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
  free(fbuf);
}