#include "Session.hpp"
#include <sys/epoll.h>

// connection closed, return -1
// read nothing, return 0
// error, return -2
// else, return bytes read
int Session::readBuf(int bytes)
{
    if(bytes <= readlen)
        return bytes;

    int res = recv(fd, readbuffer + readlen,
                   BUFFERSIZE - readlen, 0);

    if(res == 0) return -1;
    if(res < 0)
        return errno == EAGAIN ? 0 : -2;

    readlen += res;
    return readlen <= bytes ? readlen : bytes;
}

void Session::drainBuf(char* dest, int bytes)
{
    if(bytes == 0 || readlen == 0) return;

    if(bytes >= readlen) bytes = readlen;

    memcpy(dest, readbuffer, bytes);
    memmove(readbuffer, readbuffer + bytes, readlen -= bytes);
}

void Session::lookaheadBuf(char* dest, int bytes)
{
    if(bytes == 0 || readlen == 0) return;

    if(bytes >= readlen) bytes = readlen;

    memcpy(dest, readbuffer, bytes);
}

int Session::writeBuf(const char* buf, int bytes)
{
    if(bytes + writelen > BUFFERSIZE)
    {
        fprintf(stderr, "write bytes exceeds buffer size");
        return -2;
    }

    memcpy(writebuffer + writelen, buf, bytes);
    writelen += bytes;

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.ptr = this;

    if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
    { perror("epoll_ctl mod write"); return -1; }

    return bytes;
}

int Session::sendBuf()
{
    if(writelen <= 0) return 0;

    ssize_t res = send(fd, writebuffer, writelen, 0);
    if(res < 0) return errno == EAGAIN ? 1 : -2;

    memmove(writebuffer, writebuffer + res, writelen -= res);

    if(writelen == 0)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = this;

        if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
        { perror("epoll_ctl del EPOLLOUT"); return -2; }
    }

    return writelen;
}