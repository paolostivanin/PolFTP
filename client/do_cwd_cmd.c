#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h> /* per usare uint32_t invece di size_t */
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include "prototypes.h"

#define BUFFGETS 255

void do_cwd_cmd(int f_sockd){
  char buf[256], dirp[256];
  char *conferma = NULL;
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("Inserire percorso: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets dir path");
    onexit(f_sockd, 0, 0, 1);
  }
  sprintf(buf, "CWD %s", dirp);
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio richiesta CWD");
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore ricezione CWD");
    onexit(f_sockd, 0, 0, 1);
  }
  conferma = NULL;
  conferma = strtok(buf, "\0");
  printf("%s", conferma);
  if(strcmp(conferma, "ERRORE: Percorso non esistente\n") == 0){
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
  memset(dirp, 0, sizeof(dirp));
}