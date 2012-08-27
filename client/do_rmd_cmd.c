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

void do_rmd_cmd(int f_sockd){  
  char *filename = NULL, *conferma = NULL;
  char buf[256], dirp[256];
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
  printf("Inserire il nome della cartella da creare: ");
  if(fgets(dirp, BUFFGETS, stdin) == NULL){
    perror("fgets nome file");
    onexit(f_sockd, 0 ,0 ,1);
  }
  filename = NULL;
  conferma = NULL;
  filename = strtok(dirp, "\n");
  sprintf(buf, "RMD %s", filename);
  if(send(f_sockd, buf, strlen(buf), 0) < 0){
    perror("Errore durante l'invio del nome della cartella");
    onexit(f_sockd, 0, 0, 1);
  }
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore ricezione conferma cartella");
    onexit(f_sockd, 0 ,0 ,1);
  }
  conferma = strtok(buf, "\0");
  if(strcmp(conferma, "ERRORE: Cartella non eliminata") == 0){
    printf("ERRORE: impossibile eliminare la cartella\n");
    onexit(f_sockd, 0, 0, 1);
  } else printf("La cartella è stata eliminata correttamente\n");
  memset(dirp, 0, sizeof(dirp));
  memset(buf, 0, sizeof(buf));
}