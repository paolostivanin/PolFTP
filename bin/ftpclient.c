#include <stdio.h>
#include <stdlib.h>

int main(){
char *host, *user, *pass;
host = malloc(64 * sizeof(char)); /* space for 64 chars */
if(host == NULL){
	printf("\n--> ERRORE: memoria non allocata\n");
	return EXIT_FAILURE;
}
user = malloc(64 * sizeof(char)); /* space for 64 chars */
if(user == NULL){
	printf("\n--> ERRORE: memoria non allocata\n");
	return EXIT_FAILURE;
}
pass = malloc(64 * sizeof(char)); /* space for 64 chars */
if(pass == NULL){
	printf("\n--> ERRORE: memoria non allocata\n");
	return EXIT_FAILURE;
}
puts("Inserisci <host user pwd>:");
scanf("%s %s %s", host,user,pass);
printf("Host: %s\nUser: %s\nPWD: %s\n", host,user,pass);
free(host);
free(user);
free(pass);
return EXIT_SUCCESS;
}
