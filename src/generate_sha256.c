/* Descrizione: Genera un hash data una stringa utilizzando 50000 volte sha256
 * Sviluppatore: Paolo Stivanin
 * Copyright: 2012-2014
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/sha.h>

int main(void){
	static struct termios oldt, newt;
	SHA256_CTX context;
 	unsigned char md[SHA256_DIGEST_LENGTH];
 	char *input = NULL, *compare = NULL;
 	static char hashed[65];
 	int i, n=1;

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
 	size_t length = strlen((const char*)input);
 	SHA256_Init(&context);
 	SHA256_Update(&context, (unsigned char*)input, length);
 	SHA256_Final(md, &context);
 	free(compare);
 	free(input);
 	here:
 	n++;
 	for(i=0; i<SHA256_DIGEST_LENGTH; i++){
 		sprintf(hashed+(i*2), "%02x", md[i]);
 	}
 	SHA256_Init(&context);
 	SHA256_Update(&context, (unsigned char*)hashed, strlen(hashed));
 	SHA256_Final(md, &context);
 	if(n==50000){
 		printf("\n--> Write the above hash into the file '/etc/ftputils/auth':\n");
    	for(i=0; i<SHA256_DIGEST_LENGTH; i++){
   			printf("%02x", md[i]);
 		}
 		printf("\n");
 		return EXIT_SUCCESS;
	}
	goto here;
	return 0;
}
