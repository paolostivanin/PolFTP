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

int do_rmd_cmd(const int f_sockd){
  uint32_t len_dirname = 0;
  char *dir_name_delete = NULL, *conferma = NULL;
  char buf[256], tmp_buf_rmd[256];

  memset(tmp_buf_rmd, 0, sizeof(tmp_buf_rmd));
  memset(buf, 0, sizeof(buf));
  printf("Directory name (to delete): ");
  if(fgets(tmp_buf_rmd, BUFFGETS, stdin) == NULL){
    perror("Fgets dir name");
    return -1;
  }
  dir_name_delete = strtok(tmp_buf_rmd, "\n");
  len_dirname = strlen(dir_name_delete)+1;
  if(send(f_sockd, &len_dirname, sizeof(len_dirname), 0) < 0){
    perror("Error on sending the dir name length");
    return -1;
  }
  sprintf(buf, "RMD %s", dir_name_delete);
  if(send(f_sockd, buf, len_dirname+4, 0) < 0){
    perror("Error on sending the RMD request");
    return -1;
  }
  if(recv(f_sockd, buf, 3, 0) < 0){
    perror("Error on receving the RMD confirmation");
    return -1;
  }
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "NO") == 0){
    printf("ERROR: the directory cannot be deleted\n");
    return -1;
  } else printf("250 RMD OK\n");
  memset(tmp_buf_rmd, 0, sizeof(tmp_buf_rmd));
  memset(buf, 0, sizeof(buf));
  return 0;
}