#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <dirent.h>
#include "../prototypes.h"

uint32_t file_list(char *path, char ***ls){
	DIR *dp;
  struct stat fileStat;
  struct dirent *ep = NULL;
  uint32_t len, count = 0;
  int file = 0;
  *ls = NULL;
  dp = opendir (path);
  if(dp == NULL){
    fprintf(stderr, "Non esiste la directory: %s\n", path);
    exit(1);
  }

  ep = readdir(dp);
  while(NULL != ep){
    count++;
    ep = readdir(dp);
  }
  rewinddir(dp);

  *ls = calloc(count, sizeof(char *));
  count = 0;
  ep = readdir(dp);
  while(ep != NULL){
    if((file = open(ep->d_name, O_RDONLY)) < 0){
      perror("apertura file");
      exit(1);
    }
    if(fstat(file, &fileStat) != 0){
      perror("filestat");
      free(*ls);
      close(file);
      exit(EXIT_FAILURE);
    }
    close(file);
    if(S_ISDIR(fileStat.st_mode)){
      len = strlen(ep->d_name);
      (*ls)[count] = malloc(len+5); /* lunghezza stringa + "DIR \0" + "\0" perchè strdup elimina il \0 dal nome */
      strcpy((*ls)[count], "DIR "); /* copio DIR */
      strcat((*ls)[count++], ep->d_name); /* concateno la stringa DIR con il nome della dir */
      ep = readdir(dp);
    }
    else{
      (*ls)[count++] = strdup(ep->d_name);
      ep = readdir(dp);
    }
  }
  (void)closedir(dp);
  return count;
}
