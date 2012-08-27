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

void do_syst_cmd(int f_sockd){
  char buf[256];
  char *conferma = NULL;
  memset(buf, 0, sizeof(buf));
  strcpy(buf, "SYST\n");
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio richiesta SYST");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore durante la ricezione risposta SYST");
    onexit(f_sockd, 0, 0, 1);
  }
  conferma = strtok(buf, "\n");
  printf("SYST type: %s\n", conferma);
  conferma = NULL;
  memset(buf, 0, sizeof(buf));
}
