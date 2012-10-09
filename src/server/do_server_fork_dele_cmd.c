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
#include <sys/stat.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../prototypes.h"

int do_server_fork_dele_cmd(const int f_sockd){
  int fd, retval;
  uint32_t s_len;
  char buf[256];
  char *other = NULL, *server_dele_filename = NULL;

  memset(buf, 0, sizeof(buf));
  s_len = 0;
  if(recv(f_sockd, &s_len, sizeof(s_len), 0) < 0){
    perror("Error on receiving file name length");
    return -1;
  }
  if(recv(f_sockd, buf, s_len+5, 0) < 0){
    perror("Error on receiving the file name");
    return -1;
  }
  other = NULL;
  server_dele_filename = NULL;
  other = strtok(buf, " ");
  server_dele_filename = strtok(NULL, "\n");
  if(strcmp(other, "DELE") == 0){
    printf("Received DELE request\n");
  } else return -1;
  
  fd = open(server_dele_filename, O_WRONLY);
  if(fd < 0){
    fprintf(stderr, "Cannot open the file: '%s'\n", server_dele_filename);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Error on sending 'fail to open file'");
      return -1;
    }
    return -1;
  }
  retval = remove(server_dele_filename);
  if(retval != 0){
    perror("File not deleted");
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Error on sending the 'fail to remove file'");
      return -1;
    }
    close(fd);
    return -1;
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Error on sending the DELE confirmation");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}