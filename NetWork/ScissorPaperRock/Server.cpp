#include "Server.hpp"
#include "Session.hpp"
#include "MsgHandler.hpp"

/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>
/* for epoll */
#include <sys/epoll.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static void make_nonblocking(int fd)
{ fcntl(fd, F_SETFL, O_NONBLOCK); }

int Server::do_read(Session* s)
{
    ssize_t rn = s->readBuf( sizeof(Msg::len_t) );
    if(rn < 0) return rn;

    if(rn < sizeof(Msg::len_t)) return rn;

    Msg::len_t msgLen;
    s->lookaheadBuf((char *)&msgLen, sizeof(Msg::len_t));
    msgLen = ntohl(msgLen);

    rn = s->readBuf(sizeof(Msg::len_t) + msgLen);
    if(rn < 0) return rn;

    if(rn < sizeof(Msg::len_t) + msgLen) return rn;

    Msg* msg = (Msg*)malloc(sizeof(Msg::len_t) + msgLen);
    s->drainBuf((char*)msg, sizeof(Msg::len_t) + msgLen);

    // printf("%d\n", msgLen);

    decodeMsg(msg);

    // printf("%d, %d, %s\n", msg->len, msg->type, msg->data);

    msgDispatch(s, msg);

    free(msg);

    return sizeof(Msg::len_t) + msgLen;
}

int Server::do_write(Session* s)
{
    // printf("send now\n");
    int res = s->sendBuf();
    if(res == -2) perror("send buffer");
    return res;
}

int Server::do_accecpt(int listener)
{
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);

    if(fd < 0) perror("accept");
    else
    {
        make_nonblocking(fd);

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = new Session(fd, epfd);

        if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        { perror("epoll_ctl add listener"); }
        else
        {
            numOpenFds++;
            printf("Yeah, Accepted\n");
        }
    }

    return fd;
}

int Server::prepare()
{
    // adrress
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    // bind listener
    int listener = socket(AF_INET, SOCK_STREAM, 0);

    int option = 1;
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                  &option, sizeof(option)) < 0)
    { perror("set reuse socket"); return -1; }

    make_nonblocking(listener);
    if(bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    { perror("bind"); return -1; }
    // listen
    if(listen(listener, 16) < 0)
    { perror("listen"); return -1; }

    return listener;
}

void Server::run()
{
    int listener = prepare();
    if(listener < 0) return;

    epfd = epoll_create1(0);
    if(epfd == -1)
    { perror("epoll"); return; }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = new Session(listener);

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev) == -1)
    { perror("epoll_ctl add listener"); return; }

    int eventCnt = 20;
    struct epoll_event *evlist \
        = (struct epoll_event*)malloc(sizeof(struct epoll_event) * eventCnt);

    numOpenFds = 1;
    while(numOpenFds > 0)
    {
        if(numOpenFds > eventCnt)
        {
            eventCnt = numOpenFds;
            evlist = (struct epoll_event*)realloc(evlist,
                sizeof(struct epoll_event) * eventCnt);
        }

        int ready = epoll_wait(epfd, evlist, numOpenFds, -1);

        for(int i=0; i < ready; i++)
        {
            Session* s = (Session*)(evlist[i].data.ptr);

            int r = 0;
            if(evlist[i].events & EPOLLIN)
            {
                if(s->fd == listener) do_accecpt(listener);
                else r = do_read(s);
            }

            if(r >= 0 && evlist[i].events & EPOLLOUT)
            {
                if(s->fd == listener) continue;
                if((r = do_write(s)) < 0) perror("send buffer");
            }

            if( (r < 0) || (evlist[i].events & (EPOLLHUP | EPOLLERR)) )
                shutdownSession(s);
        }
    }

    free(evlist);
    close(epfd);
}

int Server::shutdownSession(Session* s)
{
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, s->fd, NULL) == -1)
    { perror("epoll_ctl del"); return -1; }
    close(s->fd);

    s->user->logout();

    delete s;
    numOpenFds--;

    printf("disconnect\n");

    return 0;
}