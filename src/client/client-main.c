#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include "../ftputils.h"

long int get_host_ip(const char *);
void send_info(int, const char *, const char *);
void recv_info(int);
void recv_pasv(int, char *);
int get_data_port(char *);


int main(int argc, char *argv[]){
	if(argc != 2){
		printf("[!] Usage: %s <hostname>\n", argv[0]);
		return -1;
	}
	if(strlen(argv[1]) > 256){
		printf("WTF?!? A hostname bigger than 256 chars? What are you trying to do? ;)\nIt's better to stop the client...\n");
		return -1;
	}
	int cmdSock, dataSock, retVal;
	int serverCmdPort = 21, serverDataPort;
	int clientCmdPort, clientDataPort;
	struct sockaddr_in client_cmd_addr, server_cmd_addr;
	struct sockaddr_in client_data_addr, server_data_addr; 
	long int serverIp;
	char buffer[BUFSIZE];
	
	serverIp = get_host_ip(argv[1]);
	memset(&server_cmd_addr, 0, sizeof(server_cmd_addr));

    printf("[+] Creating socket\n");
    if ((cmdSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("client: socket error : %d\n", errno);
		return -1;
	}
     
    printf("[+] Binding local socket\n");
    memset(&client_cmd_addr, 0, sizeof(client_cmd_addr));
    client_cmd_addr.sin_family = AF_INET;
    client_cmd_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_cmd_addr.sin_port = htons(clientCmdPort);
    if(bind(cmdSock ,(struct sockaddr *) &client_cmd_addr, sizeof(client_cmd_addr)) < 0){
		printf("client: bind  error :%d\n", errno);
		return -1;
    }
                                             
    printf("[+] Starting connect\n");
    memset(&server_cmd_addr, 0, sizeof(server_cmd_addr));
    server_cmd_addr.sin_family = AF_INET;
    server_cmd_addr.sin_addr.s_addr = htonl(serverIp);
    server_cmd_addr.sin_port = htons(serverCmdPort);
	if(connect(cmdSock ,(struct sockaddr *) &server_cmd_addr, sizeof(server_cmd_addr)) < 0){
		 printf("client: connect  error :%d\n", errno);
		 return -1;
	}
    if((retVal = recv(cmdSock, buffer, sizeof(buffer), 0)) < 0){
		printf("client: read  error :%d\n", errno);
		return -1;
	}
	buffer[retVal] = '\0';
	printf("%s", buffer);

    send_info(cmdSock, "USER polftp\r\n", "USER");
    recv_info(cmdSock);
    
    send_info(cmdSock, "PASS polftp!\r\n", "PASS");
    recv_info(cmdSock);
    
    char *pasbuf = malloc(128);
	send_info(cmdSock, "PASV\r\n", "PASV");
    recv_pasv(cmdSock, pasbuf);
    
    serverDataPort = get_data_port(pasbuf);
    free(pasbuf);
    
    if ((dataSock = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("client: data socket error : %d\n", errno);
		return -1;
	}
    client_data_addr.sin_family = AF_INET;
    client_data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_data_addr.sin_port = htons(clientDataPort);
    if (bind(dataSock ,(struct sockaddr *) &client_data_addr, sizeof(client_data_addr)) < 0){
		printf("client: bind  error dataSock:%d\n", errno);
		printf("%s\n", strerror(errno));
		return -1;
    }
   
    server_data_addr.sin_family = AF_INET;
    server_data_addr.sin_port = htons(serverDataPort);
    server_data_addr.sin_addr.s_addr = htonl(serverIp);
	if(connect(dataSock ,(struct sockaddr *) &server_data_addr, sizeof(server_data_addr)) < 0){
		 printf("client: connect  error :%d\n", errno);
		 return -1;
	}
	
	send_info(cmdSock, "LIST\r\n", "LIST"); //per ogni list devo rifare pasv, connect, etc
    recv_info(cmdSock);
    recv_info(dataSock);
    close(dataSock);
    recv_info(cmdSock);
    
    send_info(cmdSock, "QUIT\r\n", "QUIT");
    recv_info(cmdSock);
	
    close(cmdSock);
    return 0;
}

void send_info(int sockfd, const char *data, const char *cmd){
    fprintf(stdout, "[+] Sending %s to ftp server\n", cmd);
    if(send(sockfd, data, strlen(data), 0) < 0){
		printf("client: write  error :%d\n", errno);
		return;
	} 
}

void recv_info(int sockfd){
	int r;
	char buffer[BUFSIZE];
    if((r = recv(sockfd, buffer, BUFSIZE, 0)) < 0){
		printf("client: read  error :%d\n", errno);
		return;
	}
	buffer[r] = '\0';
	printf("%s", buffer);
}

void recv_pasv(int sockfd, char *pasvBuf){
	int r;
    if((r = recv(sockfd, pasvBuf, 128, 0)) < 0){
		printf("client: read  error :%d\n", errno);
		return;
	}
	pasvBuf[r] = '\0';
}

int get_data_port(char *toCut){
	//227 Entering Passive Mode (o1,o2,o3,o4,p1,p2).\r\n
	int i=0, np1, np2;
	char p1[4], p2[4];
	toCut[strlen(toCut)-4] = '\0';
	char *token = strtok(toCut, ",");
	i+=1;
	while(token != NULL){
		token = strtok(NULL, ",");
		i+=1;
		if(i==5){
			strncpy(p1, token, 4);
			p1[strlen(p1)] = '\0';
		}
		if(i==6){
			strncpy(p2, token, 4);
			p2[strlen(p2)] = '\0';
		}
	}
	sscanf(p1, "%d", &np1);
	sscanf(p2, "%d", &np2);
	
	return np1*256+np2;
}
