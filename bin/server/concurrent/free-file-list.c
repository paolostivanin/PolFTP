#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include "../../prototypes.h"

void free_file_list(char ***ls, uint32_t c){
	uint32_t i;
	for(i=0; i<c; i++){
        free((*ls)[i]);
    }
    free(*ls);
}