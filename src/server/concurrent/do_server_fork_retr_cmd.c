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

int do_server_fork_retr_cmd(const int f_sockd){
  int fd, rc;
  uint32_t fsize, size_to_send, fn_size;
  char *filename = NULL, *other = NULL;
  char buf[512];
  off_t offset;
  struct stat fileStat;

  memset(buf, 0, sizeof(buf));
  fn_size = 0;
  if(recv(f_sockd, &fn_size, sizeof(fn_size), MSG_WAITALL) < 0){
    perror("Error on receiving the file name length");
    return -1;
  }
  if(recv(f_sockd, buf, fn_size+5, 0) < 0){
    perror("Error on receiving the file name");
    return -1;
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");

  if(strcmp(other, "RETR") == 0){
    printf("Received RETR request\n");
  } else return -1;

  fd = open(filename, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "Cannot open the file '%s'\n", filename);
    strcpy(buf, "NO\0");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Error on sending NO");
      close(fd);
      return -1;
    }
    return -1;
  }
  strcpy(buf, "OK\0");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Error on sending OK");
    close(fd);
    return -1;
  }

  fsize = 0;
  fileStat.st_size = 0;
  if(fstat(fd, &fileStat) < 0){
  perror("Fstat error");
    close(fd);
    return -1;
  }
  fsize = fileStat.st_size;
  if(send(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Error on sending the file size\n");
    close(fd);
    return -1;
  }
  offset = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc = sendfile(f_sockd, fd, &offset, size_to_send);
    if (rc <= 0){
      perror("sendfile");
      close(fd);
      return -1;
    }
    size_to_send -= rc;
  }
  close(fd); /* la chiusura del file va qui altrimenti rischio loop infinito e scrittura all'interno del file */
  
  memset(buf, 0, sizeof(buf));
  strcpy(buf, "226 File successfully transferred");
  if(send(f_sockd, buf, 34, 0) < 0){
    perror("Error on sending 226 message");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}