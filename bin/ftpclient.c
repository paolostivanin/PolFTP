/* Descrizione: Client FTP sviluppato come progetto per il corso di Reti di Calcolatori (laurea SSRI presso DTI Crema)
 * Sviluppatori: Filippo Roncari, Paolo Stivanin, Stefano Agostini
 * Copyright: 2012
 * Licenza: GNU GPL v3 <http://www.gnu.org/licenses/gpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

void getinfo(unsigned int a, unsigned int b, char **pStr, const int d);

int main(){
    unsigned int len_max = 8;
    unsigned int current_size = 0;
    current_size = len_max;
    char *host, *user, *pass;
    char *pStr = malloc(len_max);
    if(pStr == NULL){
        perror("\nMemory allocation\n");
        abort();
    }
    printf("Inserisci hostname: ");
    getinfo(len_max, current_size, &pStr, 0);
    if((host=malloc(strlen(pStr)+1 * sizeof(char))) == NULL) abort();
    strncpy(host, pStr, strlen(pStr)+1);
    printf("\nInserisci username: ");
    getinfo(len_max, current_size, &pStr, 0);
    if((user=malloc(strlen(pStr)+1 * sizeof(char))) == NULL) abort();
    strncpy(user, pStr, strlen(pStr)+1);
    printf("\nInserisci password: ");
    getinfo(len_max, current_size, &pStr, 1);
    if((pass=malloc(strlen(pStr)+1 * sizeof(char))) == NULL) abort();
    strncpy(pass, pStr, strlen(pStr)+1);
    printf("\n\nHostname: %s\nUsername: %s\nPassword: %s\n", host, user, pass);
    free(pStr);
    free(host);
    free(user);
    free(pass);
    return EXIT_SUCCESS;
}

void getinfo(unsigned int a, unsigned int b, char **pStr, const int d){
    unsigned int i = 0;
    int c = EOF;
    if(d == 1){
    	struct termios term, term_orig;
   	tcgetattr(STDIN_FILENO, &term);
    	term_orig = term;
    	term.c_lflag &= ~ECHO;
    	tcsetattr(STDIN_FILENO, TCSANOW, &term);
    	while((c = getchar()) != '\n'){
        	(*pStr)[i++] = (char)c;
        	if(i == b){
           		b = i+a;
            		if((*pStr = realloc(*pStr, b)) == NULL){
                		perror("\nMemory allocation error\n");
                		tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
                		abort();
            		}
        	}
    	}
    	(*pStr)[i]='\0';
    	tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
    }
    else{
    	while((c = getchar()) != '\n'){
        	(*pStr)[i++] = (char)c;
        	if(i == b){
           		b = i+a;
            		if((*pStr = realloc(*pStr, b)) == NULL){
                		perror("\nMemory allocation error\n");
                		abort();
            		}
        	}
    	}
    	(*pStr)[i]='\0';
    }
}
