#ifndef _SCISSORPAPERROCK_SERVER_
#define _SCISSORPAPERROCK_SERVER_

/* For sockaddr_in */
#include <netinet/in.h>
#include <sys/epoll.h>
#include <map>

struct Session;

class Server
{
public:
    Server(int p);
    ~Server();

    void run();

private:
    struct sockaddr_in sin;
    int port;

    int epfd;
    int numOpenFds;

    int eventCnt;
    struct epoll_event *evlist;

    int requestFifofd;
    int responseFifofd;

    int childPid;

    int prepare();
    int do_read(Session* s);
    int do_write(Session* s);
    int do_accecpt(int fd);
    int do_recvResp();
    int shutdownSession(Session* s);
};

#endif // _SCISSORPAPERROCK_SERVER_