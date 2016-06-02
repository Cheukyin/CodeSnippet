#ifndef _SCISSORPAPERROCK_SERVER_
#define _SCISSORPAPERROCK_SERVER_

/* For sockaddr_in */
#include <netinet/in.h>
#include <map>

struct Session;

class Server
{
public:
    Server(int p): port(p) {}

    void run();

private:
    struct sockaddr_in sin;
    int port;

    int epfd;
    int numOpenFds;

    int prepare();
    int do_read(Session* s);
    int do_write(Session* s);
    int do_accecpt(int fd);
};

#endif // _SCISSORPAPERROCK_SERVER_