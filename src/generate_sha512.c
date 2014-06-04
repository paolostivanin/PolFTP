#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <gcrypt.h>

int main(void){
	static struct termios oldt, newt;
 	char *input = NULL, *compare = NULL;
 	gcry_md_hd_t hd;
	char sha512hash[129];
	const char *name = gcry_md_algo_name(GCRY_MD_SHA512);
	int i, algo = gcry_md_map_name(name);

 	tcgetattr( STDIN_FILENO, &oldt);
  	newt = oldt;
  	
 	printf("Password: ");
 	newt.c_lflag &= ~(ECHO);
 	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 	if(scanf("%m[^\n]%*c", &input) == EOF){
 		perror("Scanf input");
 		return EXIT_FAILURE;
 	}
 	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
 	printf("\nRetype password: ");
 	newt.c_lflag &= ~(ECHO);
 	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
 	if(scanf("%m[^\n]%*c", &compare) == EOF){
 		perror("Scanf compare");
 		return EXIT_FAILURE;
 	}
 	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
 	if(strcmp(input, compare) != 0){
 		printf("\nPassword doesn't match\n");
 		free(input);
 		free(compare);
 		return EXIT_FAILURE;
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
 	printf("\nWrite this hash inside the auth file (see the README file):\n%s\n", sha512hash);

	gcry_md_close(hd);
	
	return 0;
}
