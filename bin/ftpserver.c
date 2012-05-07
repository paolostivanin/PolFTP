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
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <net/if.h>  


#define BUFFERSIZE 50
#define PORT 21
#define LINK_NUM 2

#define USER 0
#define PASS 1
#define SYST 2
#define TYPE 3
#define PWD  4
#define CWD  5
#define PASV 6
#define PORTN 7
#define LIST 8
#define RETR 9
#define STOR 10
#define QUIT 11
#define DELE 12
#define MKD 13
#define UNKOWN 13

#define CMDNOTFOUNT -1
#define CMDNUM 14
#define WLAN_NAME "wlan0"

const char* commands[] = {"USER","PASS","SYST","TYPE","PWD","CWD","PASV","PORT","LIST","RETR","STOR","QUIT","DELE","MKD"};

int main(){
}
