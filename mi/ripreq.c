#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wrapper_functions.h"

#define MAX_ENTRY 25
#define PORT 520


struct ripentry
{
    char af_identifier[2];
    char route_tag[2];
    char ip_address[4];
    char subnet_mask[4];
    char next_hop[4];
    char metric[4];
};   

struct rippacket
{
    char command;
    char version;
    char must_be_zero[2];
    struct ripentry entries[MAX_ENTRY];
};


void usage()
{
    fprintf(stderr, "Usage: /ripreq IP_address\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 2){
        usage();
    }
    
    char *ip_address = argv[1];

    int sockfd = w_socket(AF_INET, SOCK_DGRAM, 0);
    
    struct rippacket packet;
    memset(&packet, 0, sizeof(packet));
    strcpy(packet.entries[0].ip_address, ip_address);

    printf("%s\n", packet.entries[0].ip_address);
    return 0;


}
