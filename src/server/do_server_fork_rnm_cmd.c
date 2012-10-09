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
#include "../prototypes.h"

int do_server_fork_rnm_cmd(const int f_sockd){
  uint32_t len_fname_todelete = 0;
  int renameFile = 0;
  char buf[256];
  char *other = NULL, *filename = NULL, *oldname = NULL;

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &len_fname_todelete, sizeof(len_fname_todelete), MSG_WAITALL) < 0){
    perror("Error on receiving file name length");
    return -1;
  }
  if(recv(f_sockd, buf, len_fname_todelete+5, 0) < 0){
    perror("Error on receiving RNFR request");
    return -1;
  }
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");
  oldname = strdup(filename);
  
  if(strcmp(other, "RNFR") == 0){
    printf("Received RNFR request\n");
  }
  else{
    free(oldname);
    return -1;
  }

  memset(buf, 0, sizeof(buf));
  len_fname_todelete = 0;
  if(recv(f_sockd, &len_fname_todelete, sizeof(len_fname_todelete), MSG_WAITALL) < 0){
    perror("Error on receiving file name length");
    free(oldname);
    return -1;
  }
  if(recv(f_sockd, buf, len_fname_todelete+5, 0) < 0){
    perror("Error on receiving RNTO request");
    free(oldname);
    return -1;
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\0");
  if(strcmp(other, "RNTO") == 0){
    printf("Received RNTO request\n");
  }
  else{
    free(oldname);
    return -1;
  }
  renameFile = rename(oldname, filename);
  if(renameFile == 0){
    strcpy(buf, "OK");
  }
  else{
    free(oldname);
    strcpy(buf, "NO");
  }
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Error on sending the RNM confirmation");
    free(oldname);
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  free(oldname);
  return 0;
}