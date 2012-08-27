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
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "prototypes.h"

void do_server_stor_cmd(f_sockd, m_sockd){
  int fd, total_bytes_read = 0;
  uint32_t fsize, fsize_tmp, nread = 0;
  char *filename = NULL, *other = NULL, *filebuffer = NULL;
  char buf[256], t_buf[256], tmp_name[256];

  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  memset(tmp_name, 0, sizeof(tmp_name));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore ricezione nome file");
    onexit(f_sockd, m_sockd,0 ,2);
  }
  other = NULL;
  filename = NULL;
  other = strtok(buf, " ");
  filename = strtok(NULL, "\n");
  strcpy(tmp_name, filename);

  if(strcmp(other, "STOR") == 0){
    printf("Ricevuta richiesta STOR\n");
  } else onexit(f_sockd, m_sockd, 0 ,2);
    
  memset(buf, 0, sizeof(buf));
  if(recv(f_sockd, buf, sizeof(buf), 0) < 0){
    perror("Errore ricezione nome file");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  fsize = 0;
  other = NULL;
  other = strtok(buf, "\0");
  if(strcmp(other, "ERRORE: File non esistente") == 0){
    printf("ERRORE: il file richiesto non esiste\n");
    onexit(f_sockd, m_sockd, 0 ,2);
  }

  if(recv(f_sockd, t_buf, sizeof(t_buf), 0) < 0){
    perror("Errore nella ricezione della grandezza del file");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  fsize = atoi(t_buf);
  fflush(stdout);
  fd = open(tmp_name, O_CREAT | O_WRONLY, 0644);
  if (fd  < 0) {
    perror("open");
    onexit(f_sockd, m_sockd, 0 ,2);
  }
  fsize_tmp = fsize;
  filebuffer = malloc(fsize);
  if(filebuffer == NULL){
    perror("malloc");
    onexit(f_sockd, m_sockd, fd, 3);
  }
  total_bytes_read = 0;
  nread = 0;
  while(((uint32_t)total_bytes_read != fsize) && ((nread = read(f_sockd, filebuffer, fsize_tmp)) > 0)){
    if(write(fd, filebuffer, nread) != nread){
      perror("write RETR");
      onexit(f_sockd, m_sockd, 0, 2);
    }
    total_bytes_read += nread;
    fsize_tmp -= nread;
  }
  close(fd); /* la chiusura del file va qui altrimenti client entra in loop infinito e si scrive all'interno del file */

  memset(buf, 0, sizeof(buf));
  strcpy(buf, "226 File trasferito correttamente");
  if(send(f_sockd, buf, 33, 0) < 0){
    perror("Errore invio conferma upload");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  memset(tmp_name, 0, sizeof(tmp_name));
  free(filebuffer);
}