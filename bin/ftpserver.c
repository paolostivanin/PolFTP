/* Descrizione: Server FTP sviluppato come progetto per il corso di Reti di Calcolatori (laurea SSRI presso DTI Crema)
 * Sviluppatori: Filippo Roncari, Paolo Stivanin, Stefano Agostini.
 * Copyright: 2012
 * Licenza: GNU AGPL v3 <http://www.gnu.org/licenses/agpl-3.0.html>
 * Sito web: <https://github.com/polslinux/FTPUtils>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <net/if.h>

#define MAXCOUNT 1024

int main(int argc, char* argv[])
{
    int sfd,nsfd,cn;
    pid_t c;
    char buf[MAXCOUNT];
    socklen_t clen;
    struct sockaddr_in caddr,saddr;

    sfd = socket(AF_INET,SOCK_STREAM,0);
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    saddr.sin_port = htons(7500);

    bind(sfd,(struct sockaddr*) &saddr,0);

    listen(sfd,10);
    for (; ;) {
        clen = sizeof(caddr);
        nsfd = accept(sfd,(struct sockaddr*) &caddr, &clen);
        if( (c = fork()) == 0) {
            close(sfd);
            memset(buf,0,sizeof(buf));
            cn = recv(nsfd,buf,sizeof(buf),0);
            if ( cn == 0) {
                perror("Reading from the client socket failed\n PROGRAM CRASH :\n");
                exit(1);
            }
            buf[cn] = '\0';
            send(nsfd,buf,strlen(buf),0);
            close(nsfd);
            exit(0);
        }
    }
    return 0;
}