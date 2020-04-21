#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "wrapper_functions.h"

#define DEFAULT_PORT 1234


void usage()
{
	fprintf(stderr, "Usage ./server [-t tcp_port] [-u udp_port] [-p popis]\n");
	exit(1);
}

int main(int argc, char **argv)
{
	if(argc > 7) usage();
	
	int sock_tcp, sock_udp;
	
	sock_tcp = w_socket(AF_INET, SOCK_STREAM, 0);
	sock_udp = w_socket(AF_INET, SOCK_DGRAM, 0);

	
	
	
	
	close(sock_tcp);
	close(sock_udp);
	
	return 0;
}
