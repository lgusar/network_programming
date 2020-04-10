/*
    TODO
    tcpklijent [-s server] [-p port] [-c] filename

    dohvaćeni podaci spremaju se u datoteku filename (obavezan argument)
    opcija -c znači da se dohvaćeni podaci dodaju na kraj datoteke filename (append)

    ako se program pozove bez -c a u direktoriju već postoji datoteka filename
    ispisuje se poruka na stderr i tcpklijent završava s radom exit status != 0

    ako se pozove s opcijom -c ali korisnik nema dozvolu pisanja u filename
    opet stderr, exit status != 0

    ako se pozove s opcijom -c ali ne postoji datoteka filename u trenutnom direktoriju
    prema poslužitelju se šalje zahtjev s offsetom 0

    kad primi datoteku/server prekine konekciju, tcpklijent završava s radom s 
    statusom koji primi od servera (0x00, 0x01, 0x02, 0x03)
*/

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "wrapper_functions.h"

#define DEFAULT_SERVER "127.0.0.1"
#define DEFAULT_PORT 1234
#define MAX_DATA 256


void usage()
{
    fprintf(stderr, "Usage: ./tcpklijent [-s server] [-p port] [-c] filename\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if(argc < 2){
        usage();
    }
    
    char *server = DEFAULT_SERVER;
    int port = DEFAULT_PORT;

    char ch;
    int cflag = 0;

    while((ch = getopt(argc, argv, "s:p:c")) != -1){
        switch(ch){
            case 's':
                server = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'c':
                cflag = 1;
                break;
            default:
                usage();
        }
    }

    char *filename = argv[argc-1];
    FILE *fp;
    int offset = 0;

    fp = fopen(filename, "a");

    if(cflag){
        struct stat st;
        stat(filename, &st);
        offset = st.st_size;
    }

    offset = htonl(offset);

    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t sin_size;
    struct addrinfo hints, *res;
    char buf[MAX_DATA];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags |= AI_CANONNAME;

    w_getaddrinfo(server, NULL, &hints, &res);

    sockfd = w_socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);

    w_connect(sockfd, (struct sockaddr *)&serv_addr, sizeof serv_addr);
    
    w_send(sockfd, &offset, 4, 0);
    w_send(sockfd, filename, strlen(filename), 0);
    
    w_recv(sockfd, buf, MAX_DATA, 0);
    int status = atoi(buf);
    
    while(w_recv(sockfd, buf, MAX_DATA, 0) > 0){
        fprintf(fp, "%s", buf);
    }

    fclose(fp);
    close(sockfd);
    freeaddrinfo(res);
    return status;
}
