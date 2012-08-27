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

void do_rnm_cmd(int f_sockd){  
  char *filename = NULL, *conferma = NULL;
  char buf[256], dirp[256];
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nome del file da rinominare: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  printf("\n");
  filename = NULL;
  filename = strtok(dirp, "\n");
  sprintf(buf, "RNFR %s", filename);
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }  
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nuovo nome del file: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  printf("\n");
  filename = NULL;
  filename = strtok(dirp, "\n");
  sprintf(buf, "RNTO %s", dirp);
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, 8, 0) < 0){
    perror("Errore ricezione conferma RNM");
    onexit(f_sockd, 0 ,0 ,1);
  }
  conferma = NULL;
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "failrnm") == 0){
    printf("550 RNM FAILED\n");
    /* non serve uscire per questo errore */
  } else { printf("250 RNM OK\n"); }
}