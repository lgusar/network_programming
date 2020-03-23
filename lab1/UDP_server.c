#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <getopt.h>
#include <err.h>

#define BUFLEN 512

void usage(){
	err(1, "./UDP_server [-l port] [-p payload]\n");
}

int main(int argc, char **argv){
	
	char payload[BUFLEN] = ""; //default value
	int port = 1234; //default value
	char recv_message[BUFLEN];

	char ch;

	while ((ch = getopt(argc, argv, "l:p:")) != -1){
		switch(ch){
			case 'l':
				port = atoi(optarg);
				break;
			case 'p':
				strcpy(payload, optarg);
				break;
			default:
				err(1, "unknown option %c\n", ch);
		}
	}

	int sockfd;

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) err(2, "socket error\n");

	struct sockaddr_in udp_addr;

	memset(&udp_addr, 0, sizeof(udp_addr));
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(port);
	udp_addr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd, (struct sockaddr *) &udp_addr, sizeof(udp_addr));

	struct sockaddr_in bot_addr;
	socklen_t bot_len;

	while(1){
		
		bot_len = sizeof(bot_addr);

		int bytes_recv = recvfrom(sockfd, &recv_message, BUFLEN, 0, (struct sockaddr *) &bot_addr, &bot_len);
		printf("%s\n", recv_message);
		if(!(strncmp(recv_message, "HELLO\n", 6))){
			int bytes_sent = sendto(sockfd, payload, strlen(payload), 0, (struct sockaddr *) &bot_addr, bot_len);
		}
	}

}
