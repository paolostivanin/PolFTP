#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "polftp.h"

int parse_input(const char *src){
	int cmd = -1;
	char *cmdString;
	char *cpString = strdup(src);
	if(strlen(src) > 5){
		cmdString = strtok(cpString, " ");
		if(*cmdString == 'C' || *cmdString == 'R') cmdString[3] = '\0';
		else cmdString[4] = '\0';
	}
	else{
		cmdString = malloc(5);
		memcpy(cmdString, src, strlen(src)-1);
		cmdString[strlen(src)-1] = '\0';
	}
	if(strcmp(cmdString, "LIST") == 0){
		cmd = LIST;
		free(cmdString);
	}
	else if(strcmp(cmdString, "SYST") == 0){
		cmd = SYST;
		free(cmdString);
	}
	else if(strcmp(cmdString, "PWD") == 0){
		cmd = PWD;
		free(cmdString);
	}
	else if(strcmp(cmdString, "FEAT") == 0){
		cmd = FEAT;
		free(cmdString);
	}
	else if(strcmp(cmdString, "CDUP") == 0){
		cmd = CDUP;
		free(cmdString);
	}
	else if(strcmp(cmdString, "QUIT") == 0){
		cmd = QUIT;
		free(cmdString);
	}
	else if(strcmp(cmdString, "CWD") == 0){
		cmd = CWD;
	}
	else if(strcmp(cmdString, "MKD") == 0){
		cmd = MKD;
	}
	else if(strcmp(cmdString, "RMD") == 0){
		cmd = RMD;
	}
	else if(strcmp(cmdString, "DELE") == 0){
		cmd = DELE;
	}
	else if(strcmp(cmdString, "SIZE") == 0){
		cmd = SIZE;
	}
	else if(strcmp(cmdString, "TYPE") == 0){
		cmd = TYPE;
	}

	free(cpString);
	return cmd;
}
