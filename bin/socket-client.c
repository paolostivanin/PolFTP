#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	
	/* Controllo che vi sia argv[0], argv[1] e argv[2] */
	if(argc != 4){
		perror("./client <hostname> <numero porta> <nomefile>\n");
		exit(1);
	}

	int DescrittoreClient, fd; /* descrittore del socket */
	int NumPorta = atoi(argv[2]); /* numero di porta */
	struct sockaddr_in serv_addr; /* indirizzo del server */
	char nread, Buffer[1024] = {}; /* contiene i dati di invio e ricezione */
	struct hostent *hp; /* con la struttura hostent definisco l'hostname del server */
	char *filename = argv[3];
	
	hp = gethostbyname(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
	serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
	serv_addr.sin_port = htons(NumPorta); /* la porta */
	serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr; /* memorizzo il tutto nella struttura serv_addr */
	
	/* int socket(int dominio, int tipo, int protocollo) */
	DescrittoreClient = socket(AF_INET, SOCK_STREAM, 0);
	if(DescrittoreClient < 0){
		perror("Errore nella creazione della socket");
		exit(1);
	}
	/* int connect (int descrittore_socket, struct sockaddr* indirizzo_server, int lunghezza_record_indirizzo) */
	connect(DescrittoreClient, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(connect < 0){
		perror("Errore nella connessione");
		close(DescrittoreClient);
		exit(1);
	}
	strcpy(Buffer, filename);
	/* int send(int descrittore_socket, const void* buffer, int lunghezza_messaggio, unsigned int opzioni) */
	send(DescrittoreClient, Buffer, strlen(Buffer), 0);
	fd = open(filename, O_CREAT | O_WRONLY,0644);
	if (fd  < 0) {
		perror("open");
		exit(1);
	}
	/* int recv (int descrittore_socket, const void* buffer, int dimensione_buffer, unsigned int opzioni) */
	while ((nread = read(DescrittoreClient, Buffer, sizeof(Buffer))) != 0) {
		// e li memorizza sul file 
		write(fd, Buffer, nread);
	}
	printf("File ricevuto\n");
	close(DescrittoreClient);
	return EXIT_SUCCESS;
}

/* Info utili
struct sockaddr_in{
   short sin_family; 		--> famiglia indirizzi DEVE essere AF_INET o (AF_INET6 per ipv6)
   unsigned short sin_port; --> contiene il numero della porta
   struct in_addr sin_addr; --> contiene l'indirizzo ip
   char sin_zero[8];		--> padding per rendere la struttura grande come sockaddr
};
--------------------------------------------------------------------------------------
struct hostent{
	char *h_name 		--> contiene l'hostname REALE
	char **h_aliases 	--> lista di aliases a cui si può accedere con un array. Ultimo elemento contiene NULL
	int h_addrtype		--> l'indirizzo risultante che DEVE essere di tipo AF_INET
	int length			--> lunghezza indirizzo in bytes (4 per ipv4)
	char **h_addr_list	--> lista indirizzi IP host. Anche se è di tipo char** in realtà è un array di struct in_addr*s. Ultimo elemento è NULL 
	h_addr 				--> un alias per il h_addr_list[0] dato che la maggior parte delle volte basta 1 solo IP
};
--------------------------------------------------------------------------------------
struct in_addr { 
	in_addr_t	s_addr; indirizzo nell'ordine dei byte di rete
};
--------------------------------------------------------------------------------------
I DOMINI di socket sono tre: AF_UNIX (protocolli interni di Unix), AF_ISO (protocolli ISO) e AF_INET (protocolli usati da internet).
Il TIPO indica il tipo di comunicazione cioè in che modo debbano essere scambiati i dati. Può assumere diversi valori fra cui SOCK_STREAM
(connessione permanente e bidirezionale basata su un flusso di byte: i dati vengono mantenuti in ordine e non sono persi o duplicati)
e SOCK_DGRAM (scambio di datagram, ovvero pacchetti di byte di lunghezza massima fissata, non affidabile al 100% perchè la connessione
non è continua e quindi i pacchetti stessi possono essere duplicati e/o non arrivare in ordine).
PROTOCOLLO indica il particolare protocollo da usare con il socket. Normalmente assume valore nullo (0), cioè il protocollo usato 
è quello di default per la combinazione di dominio e tipo specificata con gli altri due parametri. 
--------------------------------------------------------------------------------------
serv_addr.sin_addr.s_addr = ((struct in_addr*)(hp->h_addr)) -> s_addr; 
è come dire: copia h_addr di hp in s_addr di serv_addr. Questo richiede un cast
Essenzialmente ti dà l'accesso al membro della struttura quando hai un puntatore a tale struttura. Inoltre hp->h_addr ti dà l'accesso al membro
h_addr della struttura hostent alla quale hp punta.
Infine si fa il casting di questo membro al tipo in_addr* e lo si dereferenzia per quindi poter accedere al membro s_addr della struttura in_addr 
 --------------------------------------------------------------------------------------
 */
