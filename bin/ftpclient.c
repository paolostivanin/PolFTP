/* Descrizione: Client FTP sviluppato come progetto per il corso di Reti di Calcolatori (laurea SSRI presso DTI Crema)
 * Sviluppatori: Filippo Roncari, Paolo Stivanin, Stefano Agostini.
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

void validate_input(const char *h, const char *u, const char *p);
void get_pass(char *p);

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

	/* Immissione di hostname, username e password; controllo inoltre i 'return code' dei vari fscanf e, se non sono 0, esco */
	fprintf(stdout,"--> Inserisci <hostname>: ");
	if(fscanf(stdin, "%s", host) == EOF){
		fprintf(stdout, "\nErrore, impossibile leggere i dati\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout,"\n--> Inserisci <username>: ");
	if(fscanf(stdin, "%s", user) == EOF){
		fprintf(stdout, "\nErrore, impossibile leggere i dati\n");
		exit(EXIT_FAILURE);
	};
	fprintf(stdout, "\n--> Inserisci <password>: ");
	get_pass(pass);
	
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

void validate_input(const char *h, const char *u, const char *p){
	/* controllo la lunghezza delle stringhe inserite (+1 per carattere terminazione \0) */
	if((strlen(h)+1) > 64){
		fprintf(stderr,"\n--> ERRORE: L'hostname deve essere minore di 64 caratteri.\n");
		exit(EXIT_FAILURE);
	}
	
	if((strlen(u)+1) > 64){
		fprintf(stderr,"\n--> ERRORE: Lo username deve essere minore di 64 caratteri.\n");
		exit(EXIT_FAILURE);
	}
	
	if((strlen(p)+1) > 64){
		fprintf(stderr,"\n--> ERRORE: La password deve essere minore di 64 caratteri.\n");
		exit(EXIT_FAILURE);
	}
}

void get_pass(char *p){
	/* Grazie a termios.h posso disabilitare l'echoing del terminale (password nascosta) */
	struct termios term, term_orig;
   	tcgetattr(STDIN_FILENO, &term);
    	term_orig = term;
    	term.c_lflag &= ~ECHO;
    	tcsetattr(STDIN_FILENO, TCSANOW, &term);
    	/* Leggo la password e controllo il 'return code' di fscanf */
    	if(fscanf(stdin, "%s", p) == EOF){
		fprintf(stdout, "\nErrore, impossibile leggere i dati\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
		exit(EXIT_FAILURE);
	};
    	/* Reimposto il terminale allo stato originale */
    	tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
}
