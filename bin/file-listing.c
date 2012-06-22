#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int main (int argc, char *argv[]){
  DIR *dp;
  struct stat fileStat;
  struct dirent *ep;
  int file=0;
  dp = opendir ("./");

  if (dp != NULL){
    while ((ep = readdir(dp))){
      char *c = ep->d_name; /* metto il nome del file nel puntatore */
      char d = *c; /* prendo il primo carattere */
      if((file=open(ep->d_name,O_RDONLY)) < -1){ /* apro il file readonly */
        perror("open file");
      }
      if(fstat(file,&fileStat)){ /* info del file nell struttura fileStat */
        perror("fstat");
      }
      if(S_ISDIR(fileStat.st_mode)){ /* se è una directory NON lo listo */
        continue;
      }
      else{
        if(d != '.'){ /* altrimenti se il nome NON inizia con . lo listo */
          puts (ep->d_name);
        }
        else{
          continue; /* altrimenti non lo listo */
        }
      }
    }      
  (void) closedir (dp);
  }
  else
    perror ("Couldn't open the directory");

  return 0;
}