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

void do_mkd_cmd(const int f_sockd){
  uint32_t dir_name_len = 0;
  char *client_dir_name = NULL, *conferma = NULL;
  char buf[256], tmp_buf[256];

  memset(tmp_buf, 0, sizeof(tmp_buf));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nome della cartella da creare: ");
  if(fgets(tmp_buf, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  client_dir_name = strtok(tmp_buf, "\n");
  dir_name_len = strlen(client_dir_name)+1;
  if(send(f_sockd, &dir_name_len, sizeof(dir_name_len), 0) < 0){
    perror("Errore invio lunghezza dirname");
    onexit(f_sockd, 0, 0, 1);
  }
  sprintf(buf, "MKD %s", client_dir_name);
  if(send(f_sockd, buf, dir_name_len+4, 0) < 0){
    perror("Errore durante l'invio del nome della cartella");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, 3, 0) < 0){
    perror("Errore ricezione conferma cartella");
    onexit(f_sockd, 0 ,0 ,1);
  }
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "NO") == 0){
    printf("ERRORE: la cartella non può essere creata\n");
    onexit(f_sockd, 0, 0, 1);
  } else printf("250 MKD OK\n");
  memset(tmp_buf, 0, sizeof(tmp_buf));
  memset(buf, 0, sizeof(buf));
}