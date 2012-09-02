#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../prototypes.h"

int check_login_details(char *u, char *p){

	FILE *fp = NULL;
	char *tmp, *tmp2, *line = NULL;

	line = malloc(256);
	if(line == NULL){
		perror("malloc");
		free(u);
		free(p);
		return -1;
	}

	fp = fopen("/etc/ftputils/auth", "r");
	if(fp == NULL){
		printf("Errore apertura file\n");
		free(u);
		free(p);
		free(line);
		return -1;
	}

	while(fgets(line, 255, fp) != NULL){
		tmp = strtok(line, " ");
		if(tmp == NULL){
			perror("strtok 1");
			free(u);
			free(p);
			free(line);
			free(fp);
			return -1;
		}

		tmp2 = strtok(NULL, "\n"); /* con fgets ultimo carattere è \n (se pwd=12 => 12\n) quindi devo tagliare prima di \n */
		if(tmp2 == NULL){
			perror("strtok 2");
			free(u);
			free(p);
			free(line);
			free(fp);
			return -1;
		}
		if((strcmp(tmp,u) == 0) && (strcmp(tmp2,p) == 0)){
			free(line);
			free(fp);
			return 0;
		}
		else{
			continue;
		}
	}
    return -1;
}