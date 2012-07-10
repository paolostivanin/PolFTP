#define _POSIX_SOURCE
#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void get_syst(char **sysn){
	struct utsname uts;
	uint32_t len;
  	char *tmp;
  	if (uname(&uts) < 0){
    	perror("uname() error");
    	exit(EXIT_FAILURE);
  	}
	len=strlen(uts.sysname)+1;
	tmp = (char *)malloc(len);
  	strcpy(tmp, uts.sysname);
 	*sysn = tmp;
}