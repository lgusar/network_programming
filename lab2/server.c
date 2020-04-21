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
#include <getopt.h>
#include "wrapper_functions.h"

#define DEFAULT_PORT 1234
#define PAYLOAD_MAX 1024


void usage()
{
	fprintf(stderr, "Usage ./server [-t tcp_port] [-u udp_port] [-p popis]\n");
	exit(1);
}

int main(int argc, char **argv)
{
	if(argc > 7) usage();
	
	char ch;
	
	int udp_port, tcp_port;
	char popis[PAYLOAD_MAX] = "";
	
	while ((ch = getopt(argc, argv, "t:u:p:")) != -1){
		switch(ch){
			case 't':
				tcp_port = atoi(optarg);
				break;
			case 'u':
				udp_port = atoi(optarg);
				break;
			case 'p':
				strcpy(popis, optarg);
				break;
			default:
				usage();
		}
	}
	
	int sock_tcp, sock_udp;
	
	sock_tcp = w_socket(AF_INET, SOCK_STREAM, 0);
	sock_udp = w_socket(AF_INET, SOCK_DGRAM, 0);

	//tcp socket
	if(fork() == 0){
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof addr);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(tcp_port);
		w_bind(sock_tcp, (struct sockaddr *)&addr, sizeof addr);
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(udp_port);
	w_bind(sock_udp, (struct sockaddr *)&addr, sizeof addr);
	
	
	close(sock_tcp);
	close(sock_udp);
	
	return 0;
}
