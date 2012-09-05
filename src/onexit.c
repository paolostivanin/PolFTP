#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "prototypes.h"

void onexit(int c, int s, int file, int flag){
	if(flag == 0){
		close(c);
		exit(EXIT_FAILURE);
	}
	if(flag == 1){
		close(s);
		exit(EXIT_FAILURE);
	}
	if(flag == 2){
		close(c);
		close(s);
		exit(EXIT_FAILURE);
	}
	if(flag == 3){
		close(c);
		close(s);
		close(file);
		exit(EXIT_FAILURE);
	}
	if(flag == 4){
		close(c);
		close(file);
	}
}