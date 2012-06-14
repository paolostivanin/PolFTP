/* Descrizione: Server FTP sviluppato come progetto per il corso di Reti di Calcolatori (laurea SSRI presso DTI Crema)
 * Sviluppatori: Filippo Roncari e Paolo Stivanin
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>

#define MAX_BUF 10240

void do_child(int sock);

int main(int argc, char *argv[]){
    if(argc != 2){
        perror("./server <numero porta>\n");
        exit(1);
    }
    pid_t pid;
    int DescrittoreServer, DescrittoreNuovaSock, LunghezzaClient;
    int NumPorta = atoi(argv[1]);
    struct sockaddr_in serv_addr, cli_addr; /* indirizzo del server e del client */
    DescrittoreServer = socket(AF_INET, SOCK_STREAM, 0);
    if(DescrittoreServer < 0){
        perror("Errore: creazione socket\n");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr)); /* bzero scrive dei null bytes dove specificato per la lunghezza specificata */
    serv_addr.sin_family = AF_INET; /* la famiglia dei protocolli */
    serv_addr.sin_port = htons(NumPorta); /* porta */
    serv_addr.sin_addr.s_addr = INADDR_ANY; /* dato che è un server bisogna associargli l'indirizzo della macchina su cui sta girando */

    if(bind(DescrittoreServer, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("Errore di bind\n");
        close(DescrittoreServer);
        exit(1);
    }
    listen(DescrittoreServer, 5);
    LunghezzaClient = sizeof(cli_addr);
    while(1){
        DescrittoreNuovaSock = accept(DescrittoreServer, (struct sockaddr *) &cli_addr, &LunghezzaClient);
        if(DescrittoreNuovaSock < 0){
            perror("Errore: non è possibile stabilire la connessione\n");
            close(DescrittoreServer);
            close(DescrittoreNuovaSock);
            exit(1);
        }
        pid = fork();
        if(pid != 0){
            perror("Fork error");
            close(DescrittoreServer);
            close(DescrittoreNuovaSock);
            exit(1);
        }
        if(pid == 0){
            close(DescrittoreServer);
            do_child(DescrittoreNuovaSock);
            _exit(0);
        }
        else{
            close(DescrittoreNuovaSock);
        }
    }
    return EXIT_SUCCESS;
}

void do_child(int sock){
    char Buffer[MAX_BUF] = {};
    recv(sock, Buffer, sizeof(Buffer), 0);
    strcpy(Buffer, "Dati ricevuti correttamente!");
    send(sock, Buffer, strlen(Buffer)+1, 0);
}
