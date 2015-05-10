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
#include "polftp.h"

struct _info{
	char *username;
	char *password;
};


void ftp_list(int, int, long int);
void ftp_actions(int, const char *, const char *);
void ftp_small_actions(int, const char *);
void ftp_size(int, const char *);
void ftp_quit(int);

unsigned long get_host_ip(const char *);
int login(struct _info *);
void send_info(int, const char *, const char *);
void recv_info(int);
void recv_pasv(int, char *);
int get_data_port(char *);
int parse_input(const char *);

/* ToDo:
 * - client port generated randomly;
 * - check all the parameters (TYPE A or I, STOR must be a valid file, etc)
 * - check for input on parse_input (and, please, remove the > 5 control!!)
 * RETR: 200 PORT command successful
150 Opening BINARY mode data connection for bo (5 bytes)
226 Transfer complete
5 bytes received in 0,00 secs (27,1 kB/s)

 */


int main(int argc, char *argv[]){
	if (argc != 2)
	{
		printf ("[?] Usage: %s <hostname>\n", argv[0]);
		return -1;
	}
	if (strlen (argv[1]) > 256)
	{
		fprintf(stderr, "[!] Hostname longer than 256 characters, exiting...\n");
		return -1;
	}
	
	int cmd_sock, retVal;
	int server_cmd_port = 21;
	int client_cmd_port = 0, client_data_port = 0; //bind() con 0 assegna un numero arbitrario. Usare getsockname() per ottenere la porta
	int cmd_number = -1;
	unsigned long serverIp;
	
	struct sockaddr_in client_cmd_addr, server_cmd_addr, client_tmp;
	struct _info LoginInfo;
		
	char buffer[BUFSIZE];
	char *actBuf;
	
	serverIp = get_host_ip (argv[1]);
	memset (&server_cmd_addr, 0, sizeof (server_cmd_addr));

	printf ("[+] Creating socket\n");
	if ((cmd_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("client: socket error : %d\n", errno);
		return -1;
	}
     
	printf ("[+] Binding local socket\n");
	memset (&client_cmd_addr, 0, sizeof (client_cmd_addr));
	client_cmd_addr.sin_family = AF_INET;
	client_cmd_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	client_cmd_addr.sin_port = htons (client_cmd_port);
	if (bind (cmd_sock ,(struct sockaddr *) &client_cmd_addr, sizeof (client_cmd_addr)) < 0)
	{
		fprintf (stderr, "bind: %s\n", strerror (errno));
		return -1;
	}
    
	int rt = getsockname (cmd_sock, (struct sockaddr *) &client_tmp, (socklen_t *)sizeof (client_tmp));
	if (rt == -1)
	{
		fprintf (stderr, "getosockname: %s\n", strerror (errno));
		return -1;
	}
	printf ("%d\n", client_tmp.sin_port);
    
    printf ("[+] Starting connect\n");
    memset(&server_cmd_addr, 0, sizeof(server_cmd_addr));
    server_cmd_addr.sin_family = AF_INET;
    server_cmd_addr.sin_addr.s_addr = serverIp;
    server_cmd_addr.sin_port = htons(server_cmd_port);
	if(connect(cmd_sock ,(struct sockaddr *) &server_cmd_addr, sizeof(server_cmd_addr)) < 0){
		 printf("client: connect  error :%d\n", errno);
		 return -1;
	}
    if((retVal = recv(cmd_sock, buffer, sizeof(buffer), 0)) < 0){
		printf("client: read  error :%d\n", errno);
		return -1;
	}
	buffer[retVal] = '\0';
	printf("%s", buffer);
	
	if(login(&LoginInfo) == -1){
		close(cmd_sock);
		return -1;
	}

    send_info(cmd_sock, LoginInfo.username, "USER");
    recv_info(cmd_sock);
    
    send_info(cmd_sock, LoginInfo.password, "PASS");
    recv_info(cmd_sock);
    
    free(LoginInfo.username);
    free(LoginInfo.password);
    
    actBuf = malloc(ACTBUFSIZE);
    if(actBuf == NULL){
		fprintf(stderr, "Error during memory allocation (main_actbuf_malloc)\n");
		return -1;
	}
	
    while(1){
		cmd_number = -1;
		memset(actBuf, 0, ACTBUFSIZE);
		printf("ftp:$> ");
		if(fgets(actBuf, ACTBUFSIZE, stdin) == NULL){
			fprintf(stderr, "Error while reading action from stdin (fgets_main_while)\n");
			close(cmd_sock);
			break;
		}
		
		cmd_number = parse_input(actBuf);

		if(cmd_number == LIST) ftp_list(cmd_sock, client_data_port, serverIp);
		else if(cmd_number == CWD) ftp_actions(cmd_sock, actBuf, "CWD");
		else if(cmd_number == PWD) ftp_small_actions(cmd_sock, "PWD");
		else if(cmd_number == CDUP) ftp_small_actions(cmd_sock, "CDUP");
		else if(cmd_number == SYST) ftp_small_actions(cmd_sock, "SYST");
		else if(cmd_number == FEAT) ftp_small_actions(cmd_sock, "FEAT");
		else if(cmd_number == MKD) ftp_actions(cmd_sock, actBuf, "MKD");
		else if(cmd_number == RMD) ftp_actions(cmd_sock, actBuf, "RMD");
		else if(cmd_number == DELE) ftp_actions(cmd_sock, actBuf, "DELE");
		else if(cmd_number == TYPE) ftp_actions(cmd_sock, actBuf, "TYPE");
		else if(cmd_number == SIZE) ftp_size(cmd_sock, actBuf);
		else if(cmd_number == QUIT){
			ftp_quit(cmd_sock);
			break;
		}
	}

    return 0;
}

int login(struct _info *LoginData){
	struct termios Term, TermOrig;
	int counter = 0;
	char tmpU[128] = {0};
	char tmpP[256] = {0};
	
	tcgetattr(STDIN_FILENO, &TermOrig);
	Term = TermOrig;
	Term.c_lflag &= ~(ECHO);

	again_user:
	printf("Username: ");
	if(fgets(tmpU, sizeof(tmpU)-2, stdin) == NULL){
		if(counter < 3) fprintf(stderr, "Error while reading USER, try again.\n");
		else{
			fprintf(stderr, "Error while reading from stdin (fgets_login)\n");
			return -1;
		}
		memset(tmpU, 0, sizeof(tmpU));
		counter++;
		goto again_user;
	}
	tmpU[strlen(tmpU)-1] = '\0';
	
	LoginData->username = malloc(sizeof(tmpU)+5);
	if(LoginData->username == NULL){
		fprintf(stderr, "Error on memory allocation (void login: username)\n");
		return -1;
	}
	
	counter = 0;
	again_pwd:
	printf("Password: ");
	tcsetattr(STDIN_FILENO, TCSANOW, &Term);
	if(fgets(tmpP, sizeof(tmpP)-2, stdin) == NULL){
		tcsetattr(STDIN_FILENO, TCSANOW, &TermOrig);
		if(counter < 3) fprintf(stderr, "Error while reading USER, try again.\n");
		else{
			fprintf(stderr, "Error while reading from stdin (fgets_login)\n");
			return -1;
		}
		memset(tmpP, 0, sizeof(tmpP));
		counter++;
		goto again_pwd;
	}
	tmpP[strlen(tmpP)-1] = '\0';
	tcsetattr(STDIN_FILENO, TCSANOW, &TermOrig);
	
	LoginData->password = malloc(sizeof(tmpP)+5);
	if(LoginData->password == NULL){
		fprintf(stderr, "Error on memory allocation (void login: password)\n");
		return -1;
	}
	
	snprintf(LoginData->username, sizeof(tmpU)+5, "USER %s\r\n", tmpU);
	snprintf(LoginData->password, sizeof(tmpP)+5, "PASS %s\r\n", tmpP);
				
	memset(tmpU, 0, sizeof(tmpU));
	memset(tmpP, 0, sizeof(tmpP));
	
	return 0;
}

void ftp_list(int cmd_sock, int client_data_port, long int serverIp){
	struct sockaddr_in client_data_addr, server_data_addr, client_tmp;
	char *pasvBuf;
	int dataSock, serverDataPort;
    pasvBuf = malloc(128);
    if(pasvBuf == NULL){
		fprintf(stderr, "Error on memory allocation (pasvBuf main)\n");
		return;
	}
	send_info(cmd_sock, "PASV\r\n", "PASV");
    recv_pasv(cmd_sock, pasvBuf);
    
    serverDataPort = get_data_port(pasvBuf);
    free(pasvBuf);
    
    if ((dataSock = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("client: data socket error : %d\n", errno);
		return;
	}
    client_data_addr.sin_family = AF_INET;
    client_data_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_data_addr.sin_port = htons(client_data_port);
    if (bind(dataSock ,(struct sockaddr *) &client_data_addr, sizeof(client_data_addr)) < 0){
		printf("client: bind  error dataSock:%d\n", errno);
		printf("%s\n", strerror(errno));
		return;
    }

	int rt = getsockname(dataSock, (struct sockaddr *) &client_tmp, (socklen_t *)sizeof(client_tmp));
	if(rt == -1){
		fprintf(stderr, "Error on getsockname (ftp_list): %s\n", strerror(errno));
		return;
	}
	printf("%d\n", client_tmp.sin_port);

    server_data_addr.sin_family = AF_INET;
    server_data_addr.sin_port = htons(serverDataPort);
    server_data_addr.sin_addr.s_addr = serverIp;
	if(connect(dataSock ,(struct sockaddr *) &server_data_addr, sizeof(server_data_addr)) < 0){
		 fprintf(stderr, "client: connect  error :%s\n", strerror(errno));
		 return;
	}
	
	send_info(cmd_sock, "LIST\r\n", "LIST");
    recv_info(cmd_sock);
    recv_info(dataSock);
    close(dataSock);
    recv_info(cmd_sock);
}

void ftp_actions(int cmd_sock, const char *src, const char *action){
	char *dest = malloc(ACTBUFSIZE);
	strncpy(dest, src, ACTBUFSIZE);
	dest[strlen(src)-1] = '\r';
	dest[strlen(src)] = '\n';
	dest[strlen(src)+1] = '\0';
	send_info(cmd_sock, dest, action);
	recv_info(cmd_sock);
	free(dest);
}

void ftp_size(int cmd_sock, const char *src){
	send_info(cmd_sock, "TYPE I\r\n", "TYPE");
	recv_info(cmd_sock);
	char *dest = malloc(ACTBUFSIZE);
	strncpy(dest, src, ACTBUFSIZE);
	dest[strlen(src)-1] = '\r';
	dest[strlen(src)] = '\n';
	dest[strlen(src)+1] = '\0';
	send_info(cmd_sock, dest, "SIZE");
	recv_info(cmd_sock);
	free(dest);
}

void ftp_small_actions(int cmd_sock, const char *action){
	char *buffer = malloc(10);
	snprintf(buffer, 7, "%s\r\n", action);
    send_info(cmd_sock, buffer, action);
    recv_info(cmd_sock);
    if(strcmp(action, "FEAT\n") == 0) recv_info(cmd_sock);
    free(buffer);
}

void ftp_quit(int cmd_sock){
    send_info(cmd_sock, "QUIT\r\n", "QUIT");
    recv_info(cmd_sock);
    close(cmd_sock);
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
