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

void do_stor_cmd(const int f_sockd){
  int fd;
  struct stat fileStat;
  ssize_t rc;
  uint32_t fsize, size_to_send, fn_size = 0;
  char *client_stor_filename = NULL;
  char buf[256], dirp[256];
  off_t offset;

  memset(buf, 0, sizeof(buf));

  printf("Inserire il nome del file da inviare: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  client_stor_filename = NULL;
  client_stor_filename = strtok(dirp, "\n");
  fn_size = strlen(client_stor_filename)+1;
  if(send(f_sockd, &fn_size, sizeof(fn_size), 0) < 0){
    perror("Errore durante invio lunghezza nome file");
    onexit(f_sockd, 0, 0, 1);
  }
  sprintf(buf, "STOR %s", client_stor_filename);
  if(send(f_sockd, buf, fn_size+5, 0) < 0){
    perror("Errore invio nome file");
    onexit(f_sockd, 0, 0, 1);
  }

  fd = open(client_stor_filename, O_RDONLY);
  if(fd < 0){
    fprintf(stderr, "Impossibile aprire file file '%s'\n", client_stor_filename);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, 0, 0, 1);
    }
    onexit(f_sockd, 0, 0, 1);
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, 0, 0, 1);
  }

  fileStat.st_size = 0;
  if(fstat(fd, &fileStat) < 0){
    perror("Errore fstat");
    onexit(f_sockd, 0, fd, 4);
  }
  fsize = 0;
  fsize = fileStat.st_size;
  if(send(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Errore durante l'invio della grandezza del file\n");
    onexit(f_sockd, 0, fd, 4);
  }
  offset = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc = sendfile(f_sockd, fd, &offset, size_to_send);
    if (rc <= 0){
      perror("sendfile");
      onexit(f_sockd, 0, fd, 4);
    }
    size_to_send -= rc;
  }
  close(fd); /* la chiusura del file va qui altrimenti rischio loop infinito e scrittura all'interno del file */

  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, 33, 0) < 0){
    perror("Errore ricezione conferma file");
    onexit(f_sockd, 0, 0, 1);
  }
  printf("%s\n", buf);
  memset(buf, 0, sizeof(buf));
}