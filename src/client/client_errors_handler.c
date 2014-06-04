#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../ftputils.h"

void client_errors_handler(const int f_sockd, const int f_retval){
	char buffer[5];
	char *is_err = NULL;
	/* invio al server la stringa ERR o OKK */
	if(f_retval < 0){
    		memset(buffer, 0, sizeof(buffer));
    		strcpy(buffer, "ERR");
    		if(send(f_sockd, buffer, 4, 0) < 0){
      			perror("Error on sending the retval");
      			return -1;
    		}
    return;
  	}
  	else{
    		memset(buffer, 0, sizeof(buffer));
    		strcpy(buffer, "OKK");
    		if(send(f_sockd, buffer, 4, 0) < 0){
      			perror("Error on sending the retval");
      			return -1;
    		}
 	}
  /* --- */
  /* aspetto conferma dal server di OKK o ERR */
  memset(buffer, 0, sizeof(buffer));
  if(recv(f_sockd, buffer, 4, MSG_WAITALL) < 0){
   		perror("Error on recv retval");
   		return -1;
  }
  is_err = strtok(buffer, "\0");
  if(strcmp(is_err, "ERR") == 0){
   		printf("An error occured on the server\n");
   		return;
  }
  /* --- */
}
