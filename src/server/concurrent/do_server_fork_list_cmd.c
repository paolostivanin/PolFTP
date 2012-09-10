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
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <dirent.h>
#include <inttypes.h> /* per printare il tipo di dato uint32_t */
#include "../../prototypes.h"

int do_server_fork_list_cmd(const int f_sockd){
  int fpl, count, i;
  ssize_t rc_list;
  uint32_t fsize, size_to_send;
  char *other = NULL, **files;
  char buf[512], tmpname[L_tmpnam];
  FILE *fp_list;
  off_t offset_list;
  struct stat fileStat;
  char *tmpfname = NULL;

  memset(buf, 0, sizeof(buf));

  if(recv(f_sockd, buf, 5, 0) < 0){ /* i 6 caratteri sono dati da L I S T \0 */
    perror("Error on receiving the LIST request");
    return -1;
  }
  other = NULL;
  other = strtok(buf, "\0");
  if(strcmp(other, "LIST") == 0){
    printf("Received LIST request\n");
  } else return -1;

  count = file_list("./", &files);
  if(!(tmpfname = tmpnam(tmpname))){ /* genero il nome del tmpfile (tip /tmp/X6Tr4Y) */
    perror("Error tmpfile name");
    return -1;
  }
  if((fp_list = fopen(tmpfname, "w")) == NULL){
    perror("Cannot open tmp file in write mode");
    return -1;
  }

  for(i=0; i < count; i++){
    if(strcmp(files[i], "DIR ..") == 0 || strcmp(files[i], "DIR .") == 0) continue;
    else{
      fprintf(fp_list, "%s\n", files[i]);
    }
  }
  fclose(fp_list);
  free_file_list(&files, count);
  if((fpl = open(tmpfname, O_RDONLY)) < 0){
    perror("open file with open");
    return -1;
  }

  fsize = 0;
  fileStat.st_size = 0;
  if(fstat(fpl, &fileStat) < 0){
    perror("Fstat error");
    close(fpl);
    return -1;
  }
  fsize = fileStat.st_size;
  if(send(f_sockd, &fsize, sizeof(fsize), 0) < 0){
    perror("Error on sending the file size\n");
    close(fpl);
    return -1;
  }

  offset_list = 0;
  for (size_to_send = fsize; size_to_send > 0; ){
    rc_list = sendfile(f_sockd, fpl, &offset_list, size_to_send);
    if (rc_list <= 0){
      perror("sendfile");
      close(fpl);
      return -1;
    }
    size_to_send -= rc_list;
  }
  close(fpl);
  if(remove( tmpfname ) == -1 ){
    perror("Error on tmpfile deletion");
    return -1;
  }
  memset(buf, 0, sizeof(buf));
  return 0;
}