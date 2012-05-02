/* Descrizione: Client FTP sviluppato come progetto per il corso di Reti di Calcolatori (laurea SSRI presso DTI Crema)
 * Sviluppatori: Paolo Stivanin, Filippo Roncari, Stefano Agostini.
 * Copyright: 2012
 * Licenza: GNU GPL v3 <http://www.gnu.org/licenses/gpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

char validate_input(char *h, char *u, char *p);
void disable_term(void);
void enable_term(void);

int main(){
	
	char *host, *user, *pass;

	host = malloc(64); /* spazio per max 64 caratteri */
	if(!host) abort(); /* se malloc ritorna NULL allora termino l'esecuzione */
	host[63] = '\0';   /* evitare un tipo di buffer overflow impostando l'ultimo byte come NUL byte */

	user = malloc(64);
	if(!user) abort();
	user[63] = '\0';

	pass = malloc(64);
	if(!pass) abort();
	pass[63] = '\0';

	/* Immissione di hostname, username e password */
	fprintf(stdout,"--> Inserisci <hostname>: ");
	fscanf(stdin, "%s", host);
	fprintf(stdout,"\n--> Inserisci <username>: ");
	fscanf(stdin, "%s", user);
	fprintf(stdout, "\n--> Inserisci <password>: ");
	
	disable_term();
    	fscanf(stdin, "%s", pass);
	enable_term();
	
	validate_input(host,user,pass);
	
	/* Stampo a video le informazioni immesse */
	fprintf(stdout, "\n\nHost: %s\nUser: %s\nPass: %s\n\n", host,user,pass);

	/* Azzero il buffer della password e libero la memoria occupata */
	memset(pass,0,(strlen(pass)+1));
	free(host);
	free(user);
	free(pass);

	return EXIT_SUCCESS;
}

char validate_input(char *h, char *u, char *p){
	/* controllo la lunghezza delle stringhe inserite (+1 per carattere terminazione \0) */
	if((strlen(h)+1) > 64){
		fprintf(stderr,"\n--> ERRORE: L'hostname deve essere minore di 64 caratteri.\n");
		return EXIT_FAILURE;
	}
	
	if((strlen(u)+1) > 64){
		fprintf(stderr,"\n--> ERRORE: Lo username deve essere minore di 64 caratteri.\n");
		return EXIT_FAILURE;
	}
	
	if((strlen(p)+1) > 64){
		fprintf(stderr,"\n--> ERRORE: La password deve essere minore di 64 caratteri.\n");
		return EXIT_FAILURE;
	}
}

void disable_term(){
	/* Grazie a termios.h posso disabilitare l'echoing del terminale (password nascosta) */
	struct termios term, term_orig;
   	tcgetattr(STDIN_FILENO, &term);
    	term_orig = term;
    	term.c_lflag &= ~ECHO;
    	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void enable_term(){
    	/* Reimposto il terminale allo stato originale (altrimenti l'echoing resta disabilitato) */
    	struct termios term, term_orig;
    	tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
}

