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

void do_syst_cmd(const int f_sockd){
  char buf[256];
  char *conferma = NULL;
  uint32_t buf_len = 0;

  memset(buf, 0, sizeof(buf));
  strcpy(buf, "SYST");
  if(send(f_sockd, buf, 5, 0) < 0){
    perror("Errore durante l'invio richiesta SYST");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, &buf_len, sizeof(buf_len), MSG_WAITALL) < 0){
    perror("Errore durante ricezione lunghezza buffer");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, buf_len, 0) < 0){
    perror("Errore durante la ricezione risposta SYST");
    onexit(f_sockd, 0, 0, 1);
  }
  conferma = strtok(buf, "\0");
  printf("SYST type: %s\n", conferma);
  conferma = NULL;
  memset(buf, 0, sizeof(buf));
}
