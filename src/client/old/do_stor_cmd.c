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
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <fcntl.h>
#include "../ftputils.h"

int do_stor_cmd(const int f_sockd){
  int fd;
  struct stat fileStat;
  ssize_t rc, tx = 0;
  uint32_t fsize, size_to_send, fn_size = 0;
  char *client_stor_filename = NULL;
  char buf[256], dirp[256];
  off_t offset;

  memset(buf, 0, sizeof(buf));

  printf("Write the name of the file to send: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("Error fgets");
    return -1;
  }
  client_stor_filename = NULL;
  client_stor_filename = strtok(dirp, "\n");
  fn_size = strlen(client_stor_filename)+1;
  if(send(f_sockd, &fn_size, sizeof(fn_size), 0) < 0){
    perror("Error on sending the length of the file name");
    return -1;
  }
  sprintf(buf, "STOR %s", client_stor_filename);
  if(send(f_sockd, buf, fn_size+5, 0) < 0){
    perror("Error on sending the STOR request");
    return -1;
  }

  fd = open(client_stor_filename, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "Error on opening the file '%s'\n", client_stor_filename);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Error on sending 'file doesn't exist'");
      return -1;
    }
    return -1;
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Error on sending 'file does exist'");
    return -1;
  }

  fileStat.st_size = 0;
  if(fstat(fd, &fileStat) < 0){
    perror("Fstat error");
    close(fd);
    return -1;
  }
  fsize = 0;
  fsize = fileStat.st_size;
  if(send(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Error on sending the file size");
    close(fd);
    return -1;
  }
  offset = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc = sendfile(f_sockd, fd, &offset, size_to_send);
    tx += rc;
    printf("\r%zu%%", (tx * 100 / fsize));
    fflush(NULL);
    if (rc <= 0){
      perror("Error on sendfile");
      close(fd);
      return -1;
    }
    size_to_send -= rc;
  }
  close(fd);
  printf("\n");

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 34, 0) < 0){
    perror("Error on receiving the '226 File successfully transferred' confirmation");
    return -1;
  }
  printf("%s\n", buf);
  memset(buf, 0, sizeof(buf));
  return 0;
}
