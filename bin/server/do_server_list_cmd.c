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

#define BUFFGETS 255

void do_server_list_cmd(f_sockd, m_sockd){
  int fpl;
  uint32_t fsize, rc_list, size_to_send, count, i;
  char *other = NULL, **files;
  char buf[256], t_buf[256];
  FILE *fp_list;
  off_t offset_list;
  struct stat fileStat;
  
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
  if(recv(f_sockd, buf, 6, 0) < 0){ /* i 6 caratteri sono dati da L I S T \n \0 */
    perror("Errore nella ricezione comando LIST");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  other = NULL;
  other = strtok(buf, "\n");
  if(strcmp(other, "LIST") == 0){
    printf("Ricevuta richiesta LIST\n");
  } else onexit(f_sockd, m_sockd, 0, 2);

  count = file_list("./", &files);
  if((fp_list = fopen("listfiles.txt", "w")) == NULL){
    perror("Impossibile aprire il file per la scrittura LIST");
    onexit(f_sockd, m_sockd, 0, 2);
  }

  for(i=0; i < count; i++){
    if(strcmp(files[i], "DIR ..") == 0 || strcmp(files[i], "DIR .") == 0) continue;
    else{
      fprintf(fp_list, "%s\n", files[i]);
    }
  }
  fclose(fp_list);
  free_file_list(&files, count);
  if((fpl = open("listfiles.txt", O_RDONLY)) < 0){
    perror("open file with open");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  if(fstat(fpl, &fileStat) < 0){
    perror("Errore fstat");
    onexit(f_sockd, m_sockd, fpl, 3);
  }
  fsize = fileStat.st_size;
  snprintf(t_buf, BUFSIZ-1, "%" PRIu32, fsize);
  if(send(f_sockd, t_buf, sizeof(t_buf), 0) < 0){
    perror("Errore durante l'invio della grandezza del file\n");
    onexit(f_sockd, m_sockd, fpl, 3);
  }
  offset_list = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc_list = sendfile(f_sockd, fpl, &offset_list, size_to_send);
    if (rc_list <= 0){
      perror("sendfile");
      onexit(f_sockd, m_sockd, fpl, 3);
    }
    size_to_send -= rc_list;
  }
  close(fpl);
  if(remove( "listfiles.txt" ) == -1 ){
    perror("errore cancellazione file");
    onexit(f_sockd, m_sockd, 0, 2);
  }
  memset(buf, 0, sizeof(buf));
  memset(t_buf, 0, sizeof(t_buf));
}