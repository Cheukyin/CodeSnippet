#ifndef _NETWORKUTILS_
#define _NETWORKUTILS_

#include <unistd.h>
#include <stdint.h>

size_t readn(int fd, const void *buf, size_t n);
size_t writen(int fd, const void *buf, size_t n);
int openClientfd(const char* ip, const int port);
void make_nonblocking(int fd);

#endif // _NETWORKUTILS_
