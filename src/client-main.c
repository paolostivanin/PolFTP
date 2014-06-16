#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include "../ftputils.h"

struct _info{
	char *username;
	char *password;
};

long int get_host_ip(const char *);
void login(struct _info *);
void pasv_and_list(int, int, long int);
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
	int cmdSock, retVal;
	int serverCmdPort = 21;
	int clientCmdPort = 10102, clientDataPort = 10103;
	struct sockaddr_in client_cmd_addr, server_cmd_addr;
	
	struct _info LoginInfo;
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
    server_cmd_addr.sin_addr.s_addr = serverIp;
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
	
	login(&LoginInfo);

    send_info(cmdSock, LoginInfo.username, "USER");
    recv_info(cmdSock);
    
    send_info(cmdSock, LoginInfo.password, "PASS");
    recv_info(cmdSock);
    
    free(LoginInfo.username);
    free(LoginInfo.password);
    
    int exit = FALSE;
    char actBuf[256];
    while(exit == FALSE){
		memset(actBuf, 0, sizeof(actBuf));
		printf("ftp:$> ");
		fgets(actBuf, sizeof(actBuf), stdin);
		if(strcmp(actBuf, "LIST\n") == 0) pasv_and_list(cmdSock, clientDataPort, serverIp);
		else if(strcmp(actBuf, "QUIT\n") == 0) exit = TRUE;
	} 
    
    send_info(cmdSock, "QUIT\r\n", "QUIT");
    recv_info(cmdSock);
	
    close(cmdSock);
    return 0;
}

void login(struct _info *LoginData){
	struct termios Term, TermOrig;
	char tmpU[128] = {0};
	char tmpP[256] = {0};
	
	tcgetattr(STDIN_FILENO, &TermOrig);
	Term = TermOrig;
	Term.c_lflag &= ~(ECHO);

	printf("Username: ");
	fgets(tmpU, sizeof(tmpU)-2, stdin);
	tmpU[strlen(tmpU)-1] = '\0';
	
	LoginData->username = malloc(sizeof(tmpU)+5);
	if(LoginData->username == NULL){
		fprintf(stderr, "Error on memory allocation (void login: username)\n");
		return;
	}
	
	printf("Password: ");
	tcsetattr(STDIN_FILENO, TCSANOW, &Term);
	fgets(tmpP, sizeof(tmpP)-2, stdin);
	tmpP[strlen(tmpP)-1] = '\0';
	tcsetattr(STDIN_FILENO, TCSANOW, &TermOrig);
	
	LoginData->password = malloc(sizeof(tmpP)+5);
	if(LoginData->password == NULL){
		fprintf(stderr, "Error on memory allocation (void login: password)\n");
		return;
	}
	
	snprintf(LoginData->username, sizeof(tmpU)+5, "USER %s\r\n", tmpU);
	snprintf(LoginData->password, sizeof(tmpP)+5, "PASS %s\r\n", tmpP);
				
	memset(tmpU, 0, sizeof(tmpU));
	memset(tmpP, 0, sizeof(tmpP));
}

void pasv_and_list(int cmdSock, int clientDataPort, long int serverIp){
	struct sockaddr_in client_data_addr, server_data_addr;
	char *pasvBuf;
	int dataSock, serverDataPort;
    pasvBuf = malloc(128);
    if(pasvBuf == NULL){
		fprintf(stderr, "Error on memory allocation (pasvBuf main)\n");
		return;
	}
	send_info(cmdSock, "PASV\r\n", "PASV");
    recv_pasv(cmdSock, pasvBuf);
    
    serverDataPort = get_data_port(pasvBuf);
    free(pasvBuf);
    
    if ((dataSock = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("client: data socket error : %d\n", errno);
		return;
	}
    client_data_addr.sin_family = AF_INET;
    client_data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_data_addr.sin_port = htons(clientDataPort);
    if (bind(dataSock ,(struct sockaddr *) &client_data_addr, sizeof(client_data_addr)) < 0){
		printf("client: bind  error dataSock:%d\n", errno);
		printf("%s\n", strerror(errno));
		return;
    }
   
    server_data_addr.sin_family = AF_INET;
    server_data_addr.sin_port = htons(serverDataPort);
    server_data_addr.sin_addr.s_addr = serverIp;
	if(connect(dataSock ,(struct sockaddr *) &server_data_addr, sizeof(server_data_addr)) < 0){
		 printf("client: connect  error :%d\n", errno);
		 return;
	}
	
	send_info(cmdSock, "LIST\r\n", "LIST"); //per ogni list devo rifare pasv, connect, etc
    recv_info(cmdSock);
    recv_info(dataSock);
    close(dataSock);
    recv_info(cmdSock);
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

void recv_pasv(int sockfd, char *buffer){
	int r;
    if((r = recv(sockfd, buffer, 128, 0)) < 0){
		printf("client: read  error :%d\n", errno);
		return;
	}
	buffer[r] = '\0';
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
