#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <gcrypt.h>

int main(void){
	struct termios oldt, newt;
 	char input[256], compare[256];
 	gcry_md_hd_t hd;
	char sha512hash[129];
	const char *name = gcry_md_algo_name(GCRY_MD_SHA512);
	int i, algo = gcry_md_map_name(name);

 	tcgetattr( STDIN_FILENO, &oldt);
  	newt = oldt;
  	
  	start:
 	printf("[+] Password: ");
 	newt.c_lflag &= ~(ECHO);
 	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 	if(fgets(input, sizeof(input)-2, stdin) == NULL){
 		perror("Error while reading input (fgets input)\n");
 		memset(input, 0, sizeof(input));
 		return -1;
 	}
 	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
 	printf("\n[+] Retype password: ");
 	newt.c_lflag &= ~(ECHO);
 	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 	if(fgets(compare, sizeof(compare)-2, stdin) == NULL){
 		perror("Error while reading input (fgets input)\n");
 		memset(input, 0, sizeof(input));
 		memset(compare, 0, sizeof(compare));
 		return -1;
 	}
 	input[strlen(input)-1] = '\0';
 	compare[strlen(compare)-1] = '\0';
 	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
 	if(strcmp(input, compare) != 0){
 		printf("\n[!]Password doesn't match\n");
		memset(input, 0, sizeof(input));
		memset(compare, 0, sizeof(compare));
 		goto start;
 	}
	printf("\n");
	
	gcry_md_open(&hd, algo, 0);

	gcry_md_write(hd, input, strlen(input));
	gcry_md_final(hd);

	unsigned char *sha = gcry_md_read(hd, algo);
 	for(i=0; i<64; i++){
 		sprintf(sha512hash+(i*2), "%02x", sha[i]);
 	}
 	sha512hash[128] = '\0';
 	printf("[+]Write this hash inside the auth file (see the README file):\n%s\n", sha512hash);

	gcry_md_close(hd);
	
	return 0;
}
