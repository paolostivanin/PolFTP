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

void do_pwd_cmd(const int f_sockd){
  char buf[256];
  char *conferma = NULL;
  uint32_t pwd_buf_len = 0;

  memset(buf, 0, sizeof(buf));
  strcpy(buf, "PWD");
  if(send(f_sockd, buf, 4, 0) < 0){
	perror("Errore durante l'invio richiesta PWD");
		onexit(f_sockd, 0, 0, 1);
	}
	memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, &pwd_buf_len, sizeof(pwd_buf_len), MSG_WAITALL) < 0){
    perror("Errore ricezione lunghezza buffer");
    onexit(f_sockd, 0, 0, 1);
  }
	if(recv(f_sockd, buf, pwd_buf_len, 0) < 0){
   	perror("Errore ricezione PWD");
   	onexit(f_sockd, 0, 0, 1);
  }
	conferma = strtok(buf, "\0");
  printf("%s\n", conferma);
  conferma = NULL;
  memset(buf, 0, sizeof(buf));
}