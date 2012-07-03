#include <stdio.h>
#include "prototypes.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void clear_buf(char bf[256], char fbf[1024], char *cnf, int flag){
	if(flag == 1){
		memset(bf, 0, sizeof(bf));
	}
	if(flag == 2){
		memset(bf, 0, sizeof(bf));
		memset(fbf, 0, sizeof(fbf));
	}
	if(flag == 3){
		memset(bf, 0, sizeof(bf));
		memset(fbf, 0, sizeof(fbf));
		memset(cnf, 0, sizeof(cnf));
	}	
	if(flag == 4){
		memset(bf, 0, sizeof(bf));
		memset(cnf, 0, sizeof(cnf));		
	}
}
