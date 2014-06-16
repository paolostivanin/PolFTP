#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>


unsigned long get_host_ip(const char *name){
	struct addrinfo hints, *res;
	struct in_addr addr;
	int err;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	if ((err = getaddrinfo(name, NULL, &hints, &res)) != 0){
		printf("error %d\n", err);
		return 1;
	}

	addr.s_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;

	freeaddrinfo(res);

	return addr.s_addr;
}
