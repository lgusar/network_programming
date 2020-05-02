#include <stdio.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <errno.h>


int w_socket(int family, int type, int protocol)
{
    int sockfd = socket(family, type, protocol);
    if(sockfd < 0){
        fprintf(stderr, "socket() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void w_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int status = bind(sockfd, addr, addrlen);
    if(status < 0){
        fprintf(stderr, "bind() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

void w_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int status = getsockname(sockfd, addr, addrlen);
    if(status < 0){
        fprintf(stderr, "getsockname() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

void w_close(int sockfd)
{
    int status = close(sockfd);
    if(status < 0){
        fprintf(stderr, "close() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

void w_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    const char *status = inet_ntop(af, src, dst, size);
    if(status == NULL){
        fprintf(stderr, "inet_ntop() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

void w_inet_pton(int af, const char *src, void *dst)
{
    int status = inet_pton(af, src, dst);
    if(status == 0){
        fprintf(stderr, "src does not contain a character string\
                         representing a valid network address in the specified address family\n");
    }
    else if(status == -1){
        fprintf(stderr, "inet_pton() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

void w_getaddrinfo(const char *node, const char *service,
                   const struct addrinfo *hints, struct addrinfo **res)
{
    int status = getaddrinfo(node, service, hints, res);
    if(status != 0){
        fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    return;
}

void w_getnameinfo(const struct sockaddr *addr, socklen_t addrlen, 
                   char *host, socklen_t hostlen, 
                   char *serv, socklen_t servlen, int flags)
{
    int status = getnameinfo(addr, addrlen, host, hostlen, serv, servlen, flags);
    if(status != 0){
        fprintf(stderr, "getnameinfo() error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    return;

}

ssize_t w_sendto(int sockfd, const void *message, size_t len,
                 int flags, const struct sockaddr *dest_addr, socklen_t dest_len)
{
    ssize_t bytes_sent = sendto(sockfd, message, len, flags, dest_addr, dest_len);
    if(bytes_sent < 0){
        fprintf(stderr, "sendto() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return bytes_sent;
}

ssize_t w_recvfrom(int sockfd, void *restrict buffer, size_t len,
                   int flags, struct sockaddr *restrict addr,
                   socklen_t *restrict addrlen)
{
    ssize_t bytes_recv = recvfrom(sockfd, buffer, len, flags, addr, addrlen);
    if(bytes_recv < 0){
        fprintf(stderr, "recvfrom() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return bytes_recv;
}

void w_listen(int sockfd, int backlog)
{
    int status = listen(sockfd, backlog);
    if(status < 0){
        fprintf(stderr, "listen() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

int w_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int cfd = accept(sockfd, addr, addrlen);
    if(cfd < 0){
        fprintf(stderr, "accept() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return cfd;
}

void w_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int status = connect(sockfd, addr, addrlen);
    if(status < 0){
        fprintf(stderr, "connect() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

ssize_t w_read(int fd, void *buf, size_t count)
{
    ssize_t bytes_read = read(fd, buf, count);
    if(bytes_read == 0){
        printf("read(): EOF\n");
    }
    else if(bytes_read < 0){
        fprintf(stderr, "read() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return bytes_read;
}

ssize_t w_send(int sockfd, const void *buf, size_t len, int flags)
{
    ssize_t bytes_sent = send(sockfd, buf, len, flags);
    if(bytes_sent < 0){
        fprintf(stderr, "send() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return bytes_sent;
}

ssize_t w_recv(int sockfd, void *buf, size_t len, int flags)
{
    ssize_t bytes_recv = recv(sockfd, buf, len, flags);
    if(bytes_recv < 0){
        fprintf(stderr, "recv() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return bytes_recv;
}

void w_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int status = getpeername(sockfd, addr, addrlen);
    if(status < 0){
        fprintf(stderr, "getpeername() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return;
}

int w_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    int return_value = select(nfds, readfds, writefds, exceptfds, timeout);
    if(return_value < 0){
        fprintf(stderr, "select() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return return_value;
}

void w_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
    int status = setsockopt(sockfd, level, optname, optval, optlen);
    if(status < 0){
        fprintf(stderr, "setsockopt() error: %s\n", strerror(errno));
    }
    return;
}