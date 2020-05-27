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
	memset(filepath, 0, 128);
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
	memset(buffer, 0, MAXBUFFER);
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
    
    status = strtok(tmp, delim);
    
    if(!strcmp(status, "HTTP/1.1 200 OK")){
        char *content = strstr(buffer, "\r\n\r\n");
        
        if(content != NULL){
            content += 4;
        }
                
        remove(log);
        
        FILE *fd = fopen(log, "w");

        fwrite(content, strlen(content), 1, fd);

        fclose(fd);
    }

    close(sock);
}

void restart(char *url, char *log){
        char address[128];
    get_address(url, address);
    char filepath[128];
    get_filepath(url, filepath);

    char getpacket[1024];
    sprintf(getpacket, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", filepath, address);
    
    char buffer[MAXBUFFER];
	memset(buffer, 0, MAXBUFFER);
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
    
    status = strtok(tmp, delim);
    
    if(!strcmp(status, "HTTP/1.1 200 OK")){
        char *content = strstr(buffer, "\r\n\r\n");
        
        if(content != NULL){
            content += 4;
        }
                
        remove(log);
        
        FILE *fd = fopen(log, "w");

        printf("+%d\n", strlen(content));

        fwrite(content, strlen(content), 1, fd);

        fclose(fd);
    }

    close(sock);
}

void refresh_file(char *url, char *log){

    FILE *fd = fopen(log, "a");

    if(fd == NULL){
        printf("-\n");
        fclose(fd);
        return;
    }
    fseek(fd, 0L, SEEK_END);
        
    int size = ftell(fd);

    fclose(fd);

    fd = fopen(log, "a");

    char address[128];
    get_address(url, address);
    char filepath[128];
    get_filepath(url, filepath);

    char getpacket[1024];
    sprintf(getpacket, "GET %s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%d-\r\nConnection: close\r\n\r\n", filepath, address, size);
    
    char buffer[MAXBUFFER];
	memset(buffer, 0, MAXBUFFER);
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
    
    status = strtok(tmp, delim);
    
    if(!strcmp(status, "HTTP/1.1 200 OK")){
        char *content = strstr(buffer, "\r\n\r\n");
        
        if(content != NULL){
            content += 4;
            fwrite(content, strlen(content), 1, fd);

            printf("+%d\n", strlen(content));
        }
        else{
            printf("+0\n");
        }
    }
    else if(!strcmp(status, "HTTP/1.1 404 Not Found")){
        printf("-\n");
    }

    fclose(fd);

}

void process_udp(char *buffer, int socknr, char *url){
	char log[5];
    if(socknr == 1){
        strcpy(log, "log1");
    }

    else if(socknr == 2){
        strcpy(log, "log2");
    }

    else if(socknr == 3){
        strcpy(log, "log3");
    }
    else strcpy(log, "aaaa");

    if(!strcmp(buffer, "stat\n")){
        
        if(access(log, F_OK) != -1){
            FILE *fd = fopen(log, "r");

            fseek(fd, 0L, SEEK_END);
        
            int size = ftell(fd);

            printf("+%d\n", size);
        }
        else printf("-\n");
    }
    
    else if(!strcmp(buffer, "refresh\n")){
        refresh_file(url, log);
    }
    
    else if(!strcmp(buffer, "restart\n")){
        restart(url, log);
    }

    else{
        printf("-\n");
    }
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
    get_file(url2, "log2");
    get_file(url3, "log3");

    int s1 = w_socket(AF_INET, SOCK_DGRAM, 0);
    int s2 = w_socket(AF_INET, SOCK_DGRAM, 0);
    int s3 = w_socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(p1));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(s1, (struct sockaddr *)&serv_addr, sizeof serv_addr);

    memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(p2));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(s2, (struct sockaddr *)&serv_addr, sizeof serv_addr);

    memset(&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(p3));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    w_bind(s3, (struct sockaddr *)&serv_addr, sizeof serv_addr);

    fd_set master;
    fd_set read_fds;
    int fdmax;

    FD_SET(s1, &master);
	FD_SET(s2, &master);
	FD_SET(s3, &master);
	
	if(s1 > s2 && s1 > s3) fdmax = s1;
	if(s2 > s1 && s2 > s3) fdmax = s2;
    if(s3 > s2 && s3 > s1) fdmax = s3;

    char buffer[20];

    for(;;){
        read_fds = master;

        w_select(fdmax+1, &read_fds, NULL, NULL, NULL);

        for(int i = 0; i < fdmax + 1; i++){

            if(FD_ISSET(i, &read_fds)){
                memset(buffer, 0, 20);
                
                if(i == s1){
                    w_recv(s1, buffer, 20, 0);
                    process_udp(buffer, 1, url1);
                }

                if(i == s2){
                    w_recv(s2, buffer, 20, 0);
                    process_udp(buffer, 2, url2);
                }

                if(i == s3){
                    w_recv(s3, buffer, 20, 0);
                    process_udp(buffer, 3, url3);
                }
			}
		}
	}
    return 0;
}
