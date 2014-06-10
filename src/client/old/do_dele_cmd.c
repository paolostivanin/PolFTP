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


int do_dele_cmd(const int f_sockd){
  uint32_t s_len;
  char *client_dele_filename = NULL, *conferma = NULL;
  char buf[256], dirp[256];

  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("File to delete: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("Fgets file name");
    return -1;
  }
  client_dele_filename = NULL;
  conferma = NULL;
  client_dele_filename = strtok(dirp, "\n");
  s_len = 0;
  s_len = strlen(client_dele_filename)+1;
  if(send(f_sockd, &s_len, sizeof(s_len), 0) < 0){
    perror("Error on sending the file name length");
    return -1;
  }
  sprintf(buf, "DELE %s", client_dele_filename);
  if(send(f_sockd, buf, s_len+5, 0) < 0){
    perror("Error on sending the file name");
    return -1;
  }
  if(recv(f_sockd, buf, 3, 0) < 0){
    perror("Error on receiving the DELE confirmation");
    return -1;
  }
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "NO") == 0){
    printf("ERROR: the file doesn't exist or cannot be deleted.\n");
    return -1;
  } else printf("250 DELE OK\n");
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  return 0;
}