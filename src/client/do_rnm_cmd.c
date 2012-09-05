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

void do_rnm_cmd(const int f_sockd){
  uint32_t len_fname = 0;
  char *file_to_delete = NULL, *conferma = NULL;
  char buf[256], tmp_buf[256];

  memset(tmp_buf, 0, sizeof(tmp_buf));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nome del file da rinominare: ");
  if(fgets(tmp_buf, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  file_to_delete = strtok(tmp_buf, "\n");
  len_fname = strlen(file_to_delete)+1;
  if(send(f_sockd, &len_fname, sizeof(len_fname), 0) < 0){
    perror("Errore invio len fname");
    onexit(f_sockd, 0, 0, 1);
  }  
  sprintf(buf, "RNFR %s", file_to_delete);
  if(send(f_sockd, buf, len_fname+5, 0) < 0){
    perror("Errore durante l'invio del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }  
  memset(tmp_buf, 0, sizeof(tmp_buf));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nuovo nome del file: ");
  if(fgets(tmp_buf, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  file_to_delete = NULL;
  file_to_delete = strtok(tmp_buf, "\n");
  len_fname = strlen(file_to_delete)+1;
  if(send(f_sockd, &len_fname, sizeof(len_fname), 0) < 0){
    perror("Errore invio len fname");
    onexit(f_sockd, 0, 0, 1);
  }
  sprintf(buf, "RNTO %s", tmp_buf);
  if(send(f_sockd, buf, len_fname+5, 0) < 0){
    perror("Errore durante l'invio del nome del file");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, 3, 0) < 0){
    perror("Errore ricezione conferma RNM");
    onexit(f_sockd, 0 ,0 ,1);
  }
  conferma = NULL;
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "NO") == 0){
    printf("550 RNM FAILED\n");
    /* non serve uscire per questo errore */
  } else { printf("250 RNM OK\n"); }
}