#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gcrypt.h>
#include "../ftputils.h"

int hash_pwd(const char *, const char *);

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
		printf("Error: the 'auth' file cannot be opened for read\n");
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

int hash_pwd(const char *toHash, const char *toCheck){
 	gcry_md_hd_t hd;
	char sha512hash[129];
	const char *name = gcry_md_algo_name(GCRY_MD_SHA256);
	int ret, i, algo = gcry_md_map_name(name);
	
	gcry_md_open(&hd, algo, 0);

	gcry_md_write(hd, toHash, strlen(toHash));
	gcry_md_final(hd);

	unsigned char *sha = gcry_md_read(hd, algo);
 	for(i=0; i<64; i++){
 		sprintf(sha512hash+(i*2), "%02x", sha[i]);
 	}
 	sha512hash[128] = '\0';
 	if(strcmp(sha512hash, toCheck) == 0) ret = 0;
 	else ret = -1;
 	
 	gcry_md_close(hd);
 	
 	return ret;
}
