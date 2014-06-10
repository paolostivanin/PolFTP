#define _GNU_SOURCE

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
#include "../ftputils.h"


int do_list_cmd(const int f_sockd){
  ssize_t nread = 0;
  uint32_t fsize, fsize_tmp, total_bytes_read = 0;
  int tmpfname;
  char c, buf[256], tmpname[] = "/tmp/ftputilsXXXXXX";
  FILE *fp;
  void *fbuf = NULL;
  
  memset(buf, 0, sizeof(buf));
  fsize = 0;

  strcpy(buf, "LIST");
  if(send(f_sockd, buf, 5, 0) < 0){
    perror("Error on sending the LIST request");
    return -1;
  }
  if(recv(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Error on receiving the file size");
    return -1;
  }
  if((tmpfname = mkostemp(tmpname, 0)) == -1){ /* genero il nome del tmpfile (tip /tmp/X6Tr4Y) */
    perror("Error on tmp file name generation.");
    return -1;
  }
  fsize_tmp = fsize;
  fbuf = malloc(fsize);
  if(fbuf == NULL){
    perror("Error on memory allocation (malloc)");
    close(tmpfname);
    return -1;
  }
  while((total_bytes_read != fsize) && ((nread = read(f_sockd, fbuf, fsize_tmp)) > 0)){
    if(write(tmpfname, fbuf, nread) != nread){
      perror("Error on writing file");
      close(tmpfname);
      return -1;
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
  }
  close(tmpfname);
  printf("----- FILE LIST -----\n");
  if((fp = fdopen(tmpfname, "r")) == NULL){
    perror("Error on opening the tmp file for read");
    return -1;
  }
  while((c=getc(fp)) != EOF){
    putchar(c);
  }
  fclose(fp);
  printf("----- END FILE LIST -----\n");
  
  char fdPath[BUFFGETS];
  char absolutePath[BUFFGETS];
  snprintf(fdPath, BUFFGETS-1, "/proc/self/fd/%d", tmpfname);
  ssize_t numOfBytesInCompletePath = readlink(fdPath, absolutePath, BUFFGETS);
  absolutePath[numOfBytesInCompletePath] = '\0';
  if(unlink( absolutePath ) == -1 ){
    perror("Error: the tmp file cannot be deleted");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  free(fbuf);
  return 0;
}
