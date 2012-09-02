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
#include "../prototypes.h"

#define BUFFGETS 255

void do_cwd_cmd(const int f_sockd){
  uint32_t path_len = 0;
  char buf[256], dirp[256];
  char *path = NULL;

  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("Inserire percorso: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets dir path");
    onexit(f_sockd, 0, 0, 1);
  }
  path = strtok(dirp, "\n");
  path_len = strlen(path)+1;
  if(send(f_sockd, &path_len, sizeof(path_len), 0) < 0){
    perror("Errore durante invio lunghezza path");
    onexit(f_sockd, 0, 0, 1);
  }
  sprintf(buf, "CWD %s", path);
  if(send(f_sockd, buf, path_len+4, 0) < 0){
    perror("Errore durante l'invio richiesta CWD");
    onexit(f_sockd, 0, 0, 1);
  }
  memset(buf, 0, sizeof(buf));
  path_len = 0;
  if(recv(f_sockd, &path_len, sizeof(path_len), MSG_WAITALL) < 0){
    perror("Errore ricezione lunghezza buffer");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, path_len, 0) < 0){
    perror("Errore ricezione CWD");
    onexit(f_sockd, 0, 0, 1);
  }
  path = NULL;
  path = strtok(buf, "\0");
  printf("%s", path);
  memset(buf, 0, sizeof(buf));
  memset(dirp, 0, sizeof(dirp));
}