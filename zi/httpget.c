#include <stdio.h>
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
#include <sys/stat.h>
#include <fcntl.h>
#include "wrapper_functions.h"

#define HTTP 80
#define MAXBUFFER 8192


void usage(){
    fprintf(stderr, "Usage: httpget url1 p1 url2 p2 url3 p3\n");
    exit(1);
}

void get_address(char *url, char *address){
    char *delim = "/";
    char tmp[strlen(url)];

    strcpy(tmp, url);
    
    char *token;
    token = strtok(tmp, delim);
    token = strtok(NULL, delim);

    strcpy(address, token);
}

void get_filepath(char *url, char *filepath){
    char *delim = "/";
    char tmp[strlen(url)];

    strcpy(tmp, url);
    
    char *token;
    token = strtok(tmp, delim);
    token = strtok(NULL, delim);
    token = strtok(NULL, delim);

    while(token != NULL){
		strcat(filepath, "/");
		strcat(filepath, token);
		token = strtok(NULL, delim);
	}
}

void get_file(char *url, char *log){
    char address[128];
    get_address(url, address);
    char filepath[128];
    get_filepath(url, filepath);

    char getpacket[1024];
    sprintf(getpacket, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", filepath, address);
    char buffer[MAXBUFFER];

    int port = HTTP;
    
    int sock = w_socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;

    w_getaddrinfo(address, NULL, &hints, &res);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;

    w_connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    w_send(sock, getpacket, strlen(getpacket), 0);

    w_recv(sock, buffer, MAXBUFFER, 0);
	
    char *status;
    char tmp[MAXBUFFER];
    char *delim = "\r\n";
    strcpy(tmp, buffer);
    
    status = strtok(buffer, delim);

    if(!strcmp(status, "HTTP/1.1 200 OK")){

        char *content = strstr(buffer, "\r\n\r\n");
        if(content != NULL){
            content += 4;
        }
        
        int fd = open(log, 'w');

        fputs(content, fd);

        close(fd);
    }

    close(sock);
}

int main(int argc, char **argv){

    if(argc != 7){
        fprintf(stderr, "./httpget: pogresan broj argumenata\n");
        usage();
    }

    char *url1 = argv[1];
    char *url2 = argv[3];
    char *url3 = argv[5];

    char *p1 = argv[2];
    char *p2 = argv[4];
    char *p3 = argv[6];

    get_file(url1, "log1");
    get_file(url2, "log1");
    get_file(url3, "log3");

    return 0;
}
