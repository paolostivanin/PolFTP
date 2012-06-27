#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void onexit(char *u, char *p, char *l, FILE *f, int flag);

int main(int argc, char *argv[]){
	
	FILE *fp = NULL;
	char *tmp, *tmp2, *user = NULL, *pass = NULL, *line = NULL;

	printf("Inserire utente: ");
	if(scanf("%ms", &user) == EOF){
		perror("scanf");
		return EXIT_FAILURE;
	} /* scanf %ms alloca dinamicamente la memoria */

	printf("Inserire password: ");
	if(scanf("%ms", &pass) == EOF){
		perror("scanf");
		onexit(user, NULL, NULL, NULL, 1);
		return EXIT_FAILURE;
	}

	line = malloc(256);
	if(line == NULL){
		perror("malloc");
		onexit(user, pass, NULL, NULL, 2);
		return EXIT_FAILURE;
	}

	fp = fopen("/home/pol/auth.txt", "r");
	if(fp == NULL){
		printf("Errore apertura file\n");
		onexit(user, pass, line, NULL, 3);
		return EXIT_FAILURE;
	}

	while(fgets(line, 255, fp) != NULL){
		tmp = strtok(line, " ");
		if(tmp == NULL){
			perror("strtok 1");
			onexit(user, pass, line, fp, 4);
			return EXIT_FAILURE;
		}

		tmp2 = strtok(NULL, "\n"); /* con fgets ultimo carattere è \n (se pwd=12 => 12\n) quindi devo tagliare prima di \n */
		if(tmp2 == NULL){
			perror("strtok 2");
			onexit(user, pass, line, fp, 4);
			return EXIT_FAILURE;
		}
		if((strcmp(tmp,user) == 0) && (strcmp(tmp2,pass) == 0)){
			printf("USER: %s - PASS: %s\n", tmp, tmp2);
			onexit(user, pass, line, fp, 4);
			return EXIT_SUCCESS;
		}
		else{
			continue;
		}
	}
    printf("Utente e/o password non presenti nel database\n");
    onexit(user, pass, line, fp, 4);
    return EXIT_FAILURE;
}

void onexit(char *u, char *p, char *l, FILE *f, int flag){
	if(flag == 1){
		free(u);
	}
	if(flag == 2){
		free(u);
		free(p);
	}
	if(flag == 3){
		free(u);
		free(p);
		free(l);
	}
	if(flag == 4){
		free(u);
		free(p);
		free(l);
		if(f) fclose(f);
	}
}