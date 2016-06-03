#ifndef _SCISSORPAPERROCK_SESSION_
#define _SCISSORPAPERROCK_SESSION_

#include "User.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct Session
{
    static const int BUFFERSIZE = 512;

    User* user;

    int fd;
    int epfd;

    int readlen;

    Session(int fd): user(nullptr), fd(fd)
    {}

    Session(int fd, int epfd)
        : user(new User),
          fd(fd), epfd(epfd),
          readlen(0), writelen(0)
    {
        user->session = this;
    }

    ~Session()
    { if(user) delete user; }

    // connection closed, return -1
    // read nothing, return 0
    // error, return -2
    // else, return bytes read
    int readBuf(int bytes);
    void drainBuf(char* dest, int bytes);
    void lookaheadBuf(char* dest, int bytes);
    int writeBuf(const char* buf, int bytes);
    int sendBuf();

private:
    char readbuffer[BUFFERSIZE];

    char writebuffer[BUFFERSIZE];
    int writelen;

};

#endif // _SCISSORPAPERROCK_SESSION_