#ifndef _SCISSORPAPERROCK_SERVER_
#define _SCISSORPAPERROCK_SERVER_

#include "Timer.hpp"

/* For sockaddr_in */
#include <netinet/in.h>
#include <sys/epoll.h>
#include <map>
#include <string>

struct Session;
struct Msg;

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

    int timeout;
    std::map<std::string, TimerPtr> round2timer;
    TimerQueue timerQ;

    int requestFifofd;
    int responseFifofd;

    int childPid;

    int prepare();
    int do_read(Session* s);
    int do_write(Session* s);
    int do_accecpt(int fd);
    int do_recvResp();

    int shutdownSession(Session* s);

    void disableRoundTimer(Msg* msg);
    void startRoundTimer(Msg* msg);
    void sendStatus(Session* s, Msg* msg);
};

#endif // _SCISSORPAPERROCK_SERVER_