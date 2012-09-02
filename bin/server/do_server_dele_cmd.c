#define _GNU_SOURCE /* per definire get_current_dir_name */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h> /* per usare uint32_t invece di size_t */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../prototypes.h"

void do_server_dele_cmd(const int f_sockd, const int m_sockd){
  int fd, retval;
  uint32_t s_len;
  char buf[256];
  char *other = NULL, *server_dele_filename = NULL;

  memset(buf, 0, sizeof(buf));
  s_len = 0;
  if(recv(f_sockd, &s_len, sizeof(s_len), 0) < 0){
    perror("Errore durante la ricezione della lunghezza nome del file");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  if(recv(f_sockd, buf, s_len+5, 0) < 0){
    perror("Errore nella ricezione del nome del file");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  server_dele_filename = NULL;
  other = strtok(buf, " ");
  server_dele_filename = strtok(NULL, "\n");
  if(strcmp(other, "DELE") == 0){
    printf("Ricevuta richiesta DELETE\n");
  } else onexit(f_sockd, m_sockd, 0, 2);
  
  fd = open(server_dele_filename, O_WRONLY);
  if(fd < 0){
    fprintf(stderr, "Impossibile aprire il file '%s'\n", server_dele_filename);
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, m_sockd, 0, 2);
    }
    onexit(f_sockd, m_sockd, 0, 2);
  }
  retval = remove(server_dele_filename);
  if(retval != 0){
    perror("File non cancellato");
    strcpy(buf, "NO");
    if(send(f_sockd, buf, 3, 0) < 0){
      perror("Errore durante invio");
      onexit(f_sockd, m_sockd, fd, 3);
    }
    onexit(f_sockd, m_sockd, fd, 3);
  }
  strcpy(buf, "OK");
  if(send(f_sockd, buf, 3, 0) < 0){
    perror("Errore durante invio");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  memset(buf, 0, sizeof(buf));
}