#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include "wrapper_functions.h"
#include "tftp_packets.h"

#define MAX_MODE 9

void usage()
{
    fprintf(stderr, "Usage: ./tftpserver [-d] port_name_or_number\n");
    exit(1);
}

void logger(char *string, bool daemon, int exit_status)
{
    if(daemon){
        //TODO
    }
    else{
        printf("TFTP ERROR %d %s\n", exit_status, string);
    }
}

int initialize(char *port)
{
    struct sockaddr_in addr;   
    int sockfd = w_socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = INADDR_ANY;

    w_bind(sockfd, (struct sockaddr *)&addr, sizeof addr);

    return sockfd;
}

void send_error_packet(int sockfd, struct sockaddr_in *cli_addr, socklen_t addrlen, char *message, uint16_t errcode)
{
    struct error_packet packet;
    memset(&packet, 0, sizeof packet);
    packet.opcode = 5;
    packet.error_code = errcode;
    strcpy(packet.error_message, message);

    w_sendto(sockfd, &packet, sizeof packet, 0, (struct sockaddr *)cli_addr, addrlen);
}

void send_data_packet(int sockfd, struct sockaddr_in *cli_addr, socklen_t addrlen, uint16_t block_nr, char *data){

    struct data_packet packet;
    memset(&packet, 0, sizeof packet);

    packet.opcode = 3;
    packet.block_nr = block_nr;
    strcpy(packet.data, data);

    w_sendto(sockfd, &packet, sizeof packet, 0, (struct sockaddr *)cli_addr, addrlen);
}

void process_request(struct rq_packet packet, struct sockaddr_in *cli_addr, socklen_t addrlen, bool daemon)
{
    int clifd = w_socket(AF_INET, SOCK_DGRAM, 0);
    FILE *fd;

    char filename[MAX_DATA]; 
    char mode[MAX_MODE];
    uint16_t block_nr = 1;

    for(int i = 0; i < MAX_DATA + 2; i++){
        if(packet.data[i] == '\0'){
            filename[i] = packet.data[i];
            break;
        }
        filename[i] = packet.data[i];
    }

    for(int j = 0, i++; i < MAX_DATA + 2; i++, j++){
        if(packet.data[i] == '\0'){
            mode[j] = packet.data[i];
            break;
        }
        mode[j] = packet.data[i];
    }

    printf("%s->%s", inet_ntoa(cli_addr->sin_addr), filename);

    fd = open(filename, "r");

    for(int i = 0; i < MAX_MODE; i++){
        if(mode[i] == '\0') break;
        mode[i] = tolower(mode[i]);
    }

    if(strcmp(mode, "netascii")){

        long filesize;
        char *text;
        char buffer[MAX_DATA];

        fseek(fd, 0L, SEEK_END);
        filesize = ftell(fd);
        rewind(fd);

        text = calloc(1, filesize + 1);
        fread(text, 1, filesize, fd);

        int i = 0;
        while(*text != EOF){
            if(i == MAX_DATA){
                send_data_packet(clifd, cli_addr, addrlen, block_nr, buffer);
                block_nr++;
                i = 0;
            }
            
            if(*text == '\n'){
                buffer[i] = '\r';
                i++;
                if(i == MAX_DATA){
                    send_data_packet(clifd, cli_addr, addrlen, block_nr, buffer);
                    block_nr++;
                    i = 0;
                }
                buffer[i] == '\n';
                text++;
                i++;
                continue;
            }

            buffer[i] = *text;
            text++;
            i++;
        }

        send_data_packet(clifd, cli_addr, addrlen, block_nr, buffer);
        block_nr++;

        free(text);
    }

    else if(strcmp(mode, "octet")){
        
        char buffer[MAX_DATA];

        while(fread(buffer, 1, MAX_DATA, fd) == MAX_DATA){
            send_data_packet(clifd, cli_addr, addrlen, block_nr, buffer);
            block_nr++;
        }

        send_data_packet(clifd, cli_addr, addrlen, block_nr, buffer);
                block_nr++;
    }
    
    else{
        logger("Unknown mode", daemon, 1);
        send_error_packet(clifd);
        exit(1);
    }

    close(clifd);
    fclose(fd);
}

void start(int listener, bool daemon)
{
    struct sockaddr_in cli_addr;
    socklen_t addrlen;
    struct rq_packet packet;

    while(true){
        int bytes_recv = w_recvfrom(listener, &packet, sizeof packet, 0, (struct sockaddr *)&cli_addr, addrlen);
        
        uint16_t opcode = ntohs(packet.opcode);
        if(opcode == 1){
            if(fork() == 0){
                process_request(&packet, &cli_addr, addrlen, daemon);
                exit(0);
            }
        }
        else{
            logger("Wrong opcode ", daemon, 2);
        }
    }
}

int main(int argc, char **argv)
{
    if(argc > 3 || argc < 2){
        fprintf(stderr, "TFTP ERROR ");
        usage();
    }

    bool daemon = false;
    char ch;
    while((ch = getopt(argc, argv, "d")) != -1){
        switch(ch){
            case 'd':
                daemon = true;
                break;
            default:
                break;
        }
    }

    char *port = argv[argc-1];
    int listener = initialize(port);

    start(listener, daemon);
    
    close(listener);
}