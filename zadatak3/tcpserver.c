/*
    TODO
    tcpserver [-p port]
    tcp server sluša dolazne konekcije na portu (default 1234)
   
    zahtjev oblika:
        prva 4 okteta su offset od početka tražene datoteke (network-byte-order)
        naziv datoteke koji završava znakom \0
    
    odgovor oblika:
        1 oktet status + sadržaj tražene datoteke
        * 0x00 - OK
        * 0x01 - tražena datoteka ne postoji
        * 0x02 - tcpserver nema prava čitati traženu datoteku
        * 0x03 - ostale greške, proizvoljna poruka
    
    nakon odgovora, zatvara se konekcija sa klijentom i čeka se sljedeća konekcija

    slijedno posluživanje, ne treba konkurentno (! možda bonus ako mi bude dosadno)

    tcpserver dohvaća datoteke samo iz trenutnog direktorija u kojem je pozvan
    ne u kojem se nalazi sam file tcpserver te ne prihvaća slanje datoteka iz 
    poddirektorija trenutnog direktorija
    ako u zahtjevu sadrži znak / ili traži nepostojeću datoteku -> 0x01
*/

#include <stdlib.h>
#include <string.h>

#include "wrapper_functions.h"

#define DEFAULT_PORT 1234
#define BACKLOG 10
#define MAXDATA 256

void usage()
{
    fprintf(stderr, "Usage: ./tcpserver [-p port]\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if(argc > 2){
        usage();
    }

    int port = DEFAULT_PORT;

    char ch;

    while((ch = getopt(argc, argv, "p:")) != -1){
        switch(ch){
            case 'p':
                port = atoi(optarg);
                break;
            default:
                usage();
        }
    }

    int servfd, clifd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t sin_size;
    char buf[MAXDATA];
    char filename[MAXDATA];
    int offset;
    
    servfd = w_socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero);

    w_bind(servfd, (struct sockaddr *)&serv_addr, sizeof serv_addr);

    w_listen(servfd, BACKLOG);

    while(1){
        sin_size = sizeof cli_addr;

        clifd = w_accept(servfd, (struct sockaddr *)&cli_addr, &sin_size);
           
        printf("Client connected\n");

        w_recv(clifd, &offset, 4, 0);
        offset = ntohl(offset);
        
        printf("Offset: %d\n", offset);

        w_recv(clifd, filename, MAXDATA, 0);
        
        printf("Filename: %s\n", filename);

        FILE *fp;
        fp = fopen(filename, "r");

        fseek(fp, offset, SEEK_SET);

        while(fgets(buf, MAXDATA, fp)){
            w_send(clifd, buf, MAXDATA, 0);     
        }

        close(clifd);
    }

    close(servfd);

    return 0;
     

}
