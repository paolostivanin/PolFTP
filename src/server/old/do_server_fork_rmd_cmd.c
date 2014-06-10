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
#include "../ftputils.h"

int do_server_fork_rmd_cmd(const int f_sockd){
  uint32_t len_server_dirname_todelete;
  char *other = NULL, *filename = NULL;
  char buf[256];
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &len_server_dirname_todelete, sizeof(len_server_dirname_todelete), MSG_WAITALL) < 0){
    perror("Error on receiving the dir name length");
    return -1;
  }
  if(recv(f_sockd, buf, len_server_dirname_todelete+4, 0) < 0){
    perror("Error on receiving dir name");
    return -1;
  }
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");
  if(strcmp(other, "RMD") == 0){
    printf("Received RMD request\n");
  } else return -1;
  
  if(rmdir(filename) != 0){
    fprintf(stderr, "Cannot remove directory: '%s'\n", filename);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Error on sending NO confirmation");
      return -1;
    }
    return -1;
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Error on sending OK confirmation");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}