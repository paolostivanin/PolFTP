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
#include <dirent.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../../prototypes.h"

int do_server_fork_cwd_cmd(const int f_sockd){
  uint32_t path_len = 0;
  char buf[256];
  char *cd_path = NULL, *path = NULL;
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &path_len, sizeof(path_len), MSG_WAITALL) < 0){
    perror("Error on receiving path length");
    return -1;
  }
  if(recv(f_sockd, buf, path_len+4, 0) < 0){
    perror("Error on receiving CWD cmd");
    return -1;
  }
  cd_path = strtok(buf, " ");
  path = strtok(NULL, "\0");
  if(strcmp(cd_path, "CWD") == 0){
    printf("Received CWD request\n");
  } else return -1;
  if(chdir(path) < 0){
    perror("chdir");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "250 CWD command successful. PWD: %s\n", (char *)(intptr_t)get_current_dir_name());
  path_len = strlen(buf)+1;
  if(send(f_sockd, &path_len, sizeof(path_len), 0) < 0){
    perror("Error on sending buffer length");
    return -1;
  }
  if(send(f_sockd, buf, path_len, 0) < 0){
    perror("Error on sending the buffer");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}