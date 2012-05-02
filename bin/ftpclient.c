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

void get_pass(char **pass);

int main(){
	
	char *host, *user, *pass=NULL;

	host = (char *) calloc(64, sizeof(char)); /* spazio per max 64 caratteri e inizializzo a 0 (maggior sicurezza) */
	if(!host){
		fprintf(stdout, "\nErrore di allocazione della memoria\n");
		exit(EXIT_FAILURE);
	}; 		   /* se malloc ritorna NULL allora termino l'esecuzione */

	user = (char *) calloc(64, sizeof(char));
	if(!user){
		fprintf(stdout, "\nErrore di allocazione della memoria\n");
		exit(EXIT_FAILURE);
	};

	pass = (char *) calloc(64, sizeof(char));
	if(!pass){
		fprintf(stdout, "\nErrore di allocazione della memoria\n");
		exit(EXIT_FAILURE);
	};

	/* Immissione di hostname, username e password.
	 * Controllo inoltre i 'return code' dei vari fscanf e, se non sono 0, esco.
	 * Per evitare buffer overflow imposto limite massimo a 64 caratteri
	 */
	fprintf(stdout,"--> Inserisci hostname: ");
	if(fscanf(stdin, "%63s", host) == EOF){
		fprintf(stdout, "\nErrore, impossibile leggere i dati\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout,"\n--> Inserisci username: ");
	if(fscanf(stdin, "%63s", user) == EOF){
		fprintf(stdout, "\nErrore, impossibile leggere i dati\n");
		exit(EXIT_FAILURE);
	};
	fprintf(stdout, "\n--> Inserisci password: ");
	get_pass(&pass);
	
	/* Stampo a video le informazioni immesse */
	fprintf(stdout, "\n\nHost: %s\nUser: %s\nPass: %s\n\n", host,user,pass);

	/* Libero la memoria occupata */
	free(host);
	free(user);
	free(pass);

	return EXIT_SUCCESS;
}

void get_pass(char **pass){
	/* Grazie a termios.h posso disabilitare l'echoing del terminale (password nascosta) */
	struct termios term, term_orig;
   	tcgetattr(STDIN_FILENO, &term);
    	term_orig = term;
    	term.c_lflag &= ~ECHO;
    	tcsetattr(STDIN_FILENO, TCSANOW, &term);
    	/* Leggo la password e controllo il 'return code' di fscanf */
    	if(fscanf(stdin, "%63s", *pass) == EOF){
		fprintf(stdout, "\nErrore, impossibile leggere i dati\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
		exit(EXIT_FAILURE);
	};
    	/* Reimposto il terminale allo stato originale */
    	tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
}
