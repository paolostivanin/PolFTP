#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "../../prototypes.h"

int hash_pwd(char *, char *);

int check_login_details(char *u, char *p){

	int retval = -1;
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
			fclose(fp);
			return -1;
		}

		tmp2 = strtok(NULL, "\n"); /* con fgets ultimo carattere è \n (se pwd=12 => 12\n) quindi devo tagliare prima di \n */
		if(tmp2 == NULL){
			perror("strtok 2");
			free(u);
			free(p);
			free(line);
			fclose(fp);
			return -1;
		}
		retval = hash_pwd(p, tmp2);
		if((strcmp(tmp,u) == 0) && (retval == 0)){
			free(line);
			fclose(fp);
			return 0;
		}
		else{
			continue;
		}
	}
    return -1;
}

/* -> for(i=0; i<SHA256_DIGEST_LENGTH; i++){ printf("%02x", md[i]); } 
 *    stampa il digest, un array binario di lunghezza SHA256_DIGEST_LENGTH stampato a video in hex.
 * -> tocheck è una stringa di lunghezza (SHA256_DIGEST_LENGTH *2 + 1) contenente il valore in hex enunciato in ascii */
int hash_pwd(char *to_hash, char *tocheck){
	int i, n=1;
	static char outputBuffer[65];
	size_t length = strlen((const char*)to_hash);
	SHA256_CTX context;
 	unsigned char md[SHA256_DIGEST_LENGTH];
 	SHA256_Init(&context);
 	SHA256_Update(&context, (unsigned char *)to_hash, length);
 	SHA256_Final(md, &context);
 	here:
 	n++;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++){
    	sprintf(outputBuffer + (i * 2), "%02x", md[i]);
    }
    SHA256_Init(&context);
 	SHA256_Update(&context, (unsigned char*)outputBuffer, strlen(outputBuffer));
 	SHA256_Final(md, &context);
 	if(n==50000){
 		for(i = 0; i < SHA256_DIGEST_LENGTH; i++){
    		sprintf(outputBuffer + (i * 2), "%02x", md[i]);
    	}
    	if(strcmp(outputBuffer, tocheck) == 0) return 0;
    	else return -1;
	}
	goto here;
}