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
#include "../prototypes.h"

#define BUFFGETS 255

int do_stor_cmd(const int f_sockd){
  int fd, sent20, sent40, sent60, sent80;
  sent20 = sent40 = sent60 = sent80 = 0;
  struct stat fileStat;
  ssize_t rc;
  uint32_t fsize, size_to_send, fn_size = 0, vxc, qxc, sxc, oxc, tmp_xc = 0;;
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
  vxc = (fsize/100)*20;
  qxc = vxc*2;
  sxc = vxc*3;
  oxc = vxc*4;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc = sendfile(f_sockd, fd, &offset, size_to_send);
    tmp_xc += rc;
    if(tmp_xc < vxc){
      if(sent20 != 1) printf("20%%\n");
      sent20 = 1;
    }
    if(tmp_xc > vxc && tmp_xc < qxc){
      if(sent40 != 1) printf("40%%\n");
      sent40 = 1;
    }
    if(tmp_xc > qxc && tmp_xc < sxc){
      if(sent60 != 1) printf("60%%\n");
      sent60 = 1;
    }
    if(tmp_xc > sxc && tmp_xc < oxc){
      if(sent80 != 1) printf("80%%\n");
      sent80 = 1;
    }
    if (rc <= 0){
      perror("Error on sendfile");
      close(fd);
      return -1;
    }
    size_to_send -= rc;
  }
  close(fd);

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 34, 0) < 0){
    perror("Error on receiving the '226 File successfully transferred' confirmation");
    return -1;
  }
  printf("%s\n", buf);
  memset(buf, 0, sizeof(buf));
  return 0;
}