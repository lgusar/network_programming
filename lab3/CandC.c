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
#include "bot.h"

#define UDP_PORT 5555
#define TCP_PORT 80
#define BACKLOG 10
#define BOTS_MAX 16
#define PAYLOAD_MAX 1024

void usage(){
    fprintf(stderr, "Usage: ./CandC [tcp_port]\n");
    exit(1);
}

void print_commands(){
    printf("pt    bot klijentima šalje poruku PROG_TCP (struct MSG:1 10.0.0.20 1234\\n)\n");
    printf("ptl   bot klijentima šalje poruku PROG_TCP (struct MSG:1 127.0.0.1 1234\\n)\n");
    printf("pu    bot klijentima šalje poruku PROG_UDP (struct MSG:2 10.0.0.20 1234\\n)\n");
    printf("pul   bot klijentima šalje poruku PROG_UDP (struct MSG:2 127.0.0.1 1234\\n)\n");
    printf("r     bot klijentima šalje poruku RUN s adresama lokalnog računala:\n");
    printf("      struct MSG:3 127.0.0.1 vat localhost 6789\\n\n");
    printf("r2    bot klijentima šalje poruku RUN s adresama računala iz IMUNES-a:\n");
    printf("      struct MSG:3 20.0.0.11 1111 20.0.0.12 2222 20.0.0.13 dec-notes\n");
    printf("s     bot klijentima šalje poruku STOP (struct MSG:4)\n");
    printf("l     lokalni ispis adresa bot klijenata\n");
    printf("n     šalje poruku: ’NEPOZNATA’\\n\n");
    printf("q     bot klijentima šalje poruku QUIT i završava s radom (struct MSG:0)\n");
    printf("h     ispis naredbi\n");
}

void pt(int udp_sock, struct bot *bots, int number_of_bots){
    struct msg prog_tcp;
    memset(&prog_tcp, 0, sizeof(prog_tcp));

    prog_tcp.command = 1;
    strcpy(prog_tcp.entry[0].ip_address, "10.0.0.20");
    strcpy(prog_tcp.entry[0].port_number, "1234");

    for(int i = 0; i < number_of_bots; i++){
        
        struct sockaddr_in addr;
	    struct addrinfo hints, *res;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
	
	    w_getaddrinfo(bots[i].ip_address, NULL, &hints, &res);
        
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(ots[i].port_number));
        addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;

        w_sendto(udp_sock, &prog_tcp, 39, 0, (struct sockaddr *)&addr, sizeof(addr));

        freeaddrinfo(res);
    }
}

void q(int udp_sock, struct bot *bots, int number_of_bots){
    struct msg prog_tcp;
    memset(&prog_tcp, 0, sizeof(prog_tcp));

    prog_tcp.command = 0;

    for(int i = 0; i < number_of_bots; i++){
        
        struct sockaddr_in addr;
	    struct addrinfo hints, *res;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
	
	    w_getaddrinfo(bots[i].ip_address, NULL, &hints, &res);
        
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(bots[i].port_number));
        addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;

        w_sendto(udp_sock, &prog_tcp, 1, 0, (struct sockaddr *)&addr, sizeof(addr));

        freeaddrinfo(res);
    }
}

void process_stdin(int stdin_fd, int udp_sock, int tcp_sock, struct bot *bots, int number_of_bots, bool quit_flag){
    char buf[PAYLOAD_MAX];
    memset(buf, 0, PAYLOAD_MAX);

    read(stdin_fd, buf, PAYLOAD_MAX);

    if(!strcmp(buf, "pt")){
        pt(udp_sock, bots, number_of_bots);
    }
    
    else if(!strcmp(buf, "pt1")){
        //do something
    }

    else if(!strcmp(buf, "pu")){
        //do something
    }

    else if(!strcmp(buf, "pu1")){
        //do something
    }

    else if(!strcmp(buf, "r")){
        //do something
    }

    else if(!strcmp(buf, "r2")){
        //do something
    }

    else if(!strcmp(buf, "s")){
        //do something
    }

    else if(!strcmp(buf, "l")){
        //do something
    }

    else if(!strcmp(buf, "n")){
        //do something
    }

    else if(!strcmp(buf, "q")){
        printf(" --> QUIT\n");
        printf("Kraj programa.\n");
        q(udp_sock, bots, number_of_bots);
        quit_flag = true;
    }

    else if(!strcmp(buf, "h")){
        //do something
    }

    else{
        printf("Nepoznata naredba: %s\n", buf);
        print_commands();
    }
}

void init_sockets(int udp_port, int tcp_port, int *udp_sock, int *tcp_sock){
    struct sockaddr_in serv_addr;

    *udp_sock = w_socket(AF_INET, SOCK_DGRAM, 0);
    *tcp_sock = w_socket(AF_INET, SOCK_STREAM, 0);

    int on = 1;
    w_setsockopt(*tcp_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(*tcp_sock, (struct sockaddr *)&serv_addr, sizeof serv_addr);
    
    memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(udp_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(*udp_sock, (struct sockaddr *)&serv_addr, sizeof serv_addr);

	w_listen(*tcp_sock, BACKLOG);
}

int main(int argc, char **argv){
    if(argc > 2) usage();

    int udp_port = UDP_PORT;
    int tcp_port = TCP_PORT;

    if(argc == 2){
        tcp_port = atoi(argv[1]);
    }

    int udp_sock;
    int tcp_sock;
    int stdin_fd = 0;

    init_sockets(udp_port, tcp_port, &udp_sock, &tcp_sock);

    fd_set master;
    fd_set read_fds;
    int fdmax;

    FD_SET(stdin_fd, &master);
	FD_SET(tcp_sock, &master);
	FD_SET(udp_sock, &master);
	
	if(tcp_sock > udp_sock) fdmax = tcp_sock;
	else fdmax = udp_sock;

    struct bot bots[BOTS_MAX];
    int number_of_bots = 0;

    bool quit_flag = false;

    for(;;){
        read_fds = master;

        w_select(fdmax+1, &read_fds, NULL, NULL, NULL);

        for(int i = 0; i < fdmax + 1; i++){

            if(FD_ISSET(i, &read_fds)){

                if(i == stdin_fd){
                    process_stdin(stdin_fd, udp_sock, tcp_sock, bots, number_of_bots, quit_flag);
                    if(quit_flag) goto exiting;
                }

                if(i == udp_sock){
                    //do something
                }

                if(i == tcp_sock){
                    //do something
                }
            }
        }
    }


exiting:

	close(tcp_sock);
	close(udp_sock);

    return 0;
}