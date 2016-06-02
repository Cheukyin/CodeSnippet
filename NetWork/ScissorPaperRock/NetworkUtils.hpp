#ifndef _NETWORKUTILS_
#define _NETWORKUTILS_

#include <arpa/inet.h>
/* For sockaddr_in */
#include <netinet/in.h>
/* For socket functions */
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

size_t readn(int fd, const void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = (char*)buf;

    while(nleft > 0)
    {
        if((nread = read(fd, bufp, nleft)) < 0)
        {
            if(errno == EINTR) nread = 0;
            else return -1;
        }
        else if(nread == 0) break;

        nleft -= nread;
        bufp += nread;
    }

    return n - nleft;
}

size_t writen(int fd, const void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = (char*)buf;

    while(nleft > 0)
    {
        if((nwritten = write(fd, bufp, nleft)) <= 0)
        {
            if(errno == EINTR) nwritten = 0;
            else return -1;
        }

        nleft -= nwritten;
        bufp += nwritten;
    }

    return n;
}

int openClientfd(const char* ip, const int port)
{
    struct sockaddr_in serveraddr;

    int clientfd;
    if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    bzero((char*)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ip);

    if(connect(clientfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;
    return clientfd;
}

#endif // _NETWORKUTILS_