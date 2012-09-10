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
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../../prototypes.h"

int do_server_fork_syst_cmd(const int f_sockd){
  char buf[256];
  char *sysname = NULL, *other = NULL;
  uint32_t buf_len = 0;

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 5, 0) < 0){
    perror("Error on receiving SYST cmd");
    return -1;
  }
  other = NULL;
  other = strtok(buf, "\0");
  if(strcmp(other, "SYST") == 0){
    printf("Received SYST request\n");
  } else return -1; 
  get_syst(&sysname);
  sprintf(buf, "%s", sysname);
  buf_len = strlen(buf)+1;
  if(send(f_sockd, &buf_len, sizeof(buf_len), 0) < 0){
    perror("Error on sending buffer length");
    return -1;
  }
  if(send(f_sockd, buf, buf_len, 0) < 0){
    perror("Error on sending the buffer");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}
