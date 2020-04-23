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
#define PAYLOAD_MAX 1024+1
#define BACKLOG 16

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
    udp_port = tcp_port = DEFAULT_PORT;
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
	
	popis[strlen(popis)-1] = '\n';

    fd_set master;
    fd_set read_fds;
    int fdmax;
	int sock_tcp, sock_udp;
    int stdin_fd = 0;
    int clifd;
    struct sockaddr_in serv_addr;
    struct sockaddr_storage cli_addr;
    socklen_t addrlen;
    char buf[PAYLOAD_MAX];
	
	sock_tcp = w_socket(AF_INET, SOCK_STREAM, 0);
	sock_udp = w_socket(AF_INET, SOCK_DGRAM, 0);
	
	memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(sock_tcp, (struct sockaddr *)&serv_addr, sizeof serv_addr);
    
    memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(udp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(sock_udp, (struct sockaddr *)&serv_addr, sizeof serv_addr);

	w_listen(sock_tcp, BACKLOG);
	
	FD_SET(stdin_fd, &master);
	FD_SET(sock_tcp, &master);
	FD_SET(sock_udp, &master);
	
	if(sock_tcp > sock_udp) fdmax = sock_tcp;
	else fdmax = sock_udp;
	
	for(;;){
		read_fds = master;
		
		w_select(fdmax+1, &read_fds, NULL, NULL, NULL);
		
		for(int i = 0; i <= fdmax; ++i){
			if(FD_ISSET(i, &read_fds)){
				if(i == stdin_fd){
					memset(buf, 0, PAYLOAD_MAX);
					read(stdin_fd, buf, PAYLOAD_MAX);
					if(strcmp(buf, "PRINT\n") == 0)
						printf("%s\n", popis);
					else if(strstr(buf, "SET")){
						strncpy(popis, buf + 4, strlen(buf) - 5);
					}
					else if(strcmp(buf, "QUIT\n") == 0){
						printf("Quitting.\n");
						exit(0);
					}	
					else printf("Unknown command.\n");
				}
				
				if(i == sock_tcp){
					
					memset(buf, 0, PAYLOAD_MAX);
					addrlen = sizeof cli_addr;
					clifd = w_accept(sock_tcp, (struct sockaddr *)&cli_addr, &addrlen);
					
					printf("Bot connected.\n");
					
					w_recv(clifd, buf, PAYLOAD_MAX, 0);
					if(strncmp(buf, "HELLO\n", 6) == 0){
						strcpy(buf, popis);
						w_send(clifd, buf, strlen(buf), 0);
					}
					close(clifd);
				}
				
				
			}
		}
	}
	
	close(sock_tcp);
	close(sock_udp);
	
	return 0;
}
