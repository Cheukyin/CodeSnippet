#include "Server.hpp"
#include "Session.hpp"
#include "MsgHandler.hpp"
#include "Fifo.hpp"

/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <functional>

static void make_nonblocking(int fd)
{ fcntl(fd, F_SETFL, O_NONBLOCK); }

void Server::disableRoundTimer(Msg* msg)
{
    auto r2t = round2timer.find(msg->data);
    if(r2t != round2timer.end())
    {
        r2t->second->deactive();
        round2timer.erase(r2t);
    }

    free(msg);
}

void Server::startRoundTimer(Msg* msg)
{
    int reqFifofd = requestFifofd;
    std::map<std::string, TimerPtr> *round2timer_ptr(&round2timer);

    msg->type = ROUNDTIMEOUT;

    auto r2t = round2timer.find(msg->data);
    if(r2t != round2timer.end())
        r2t->second->deactive();

    TimerPtr timer(new Timer);
    timer->setPeriod(RoundTimeout);
    timer->cb = [reqFifofd, msg, round2timer_ptr](){

        auto r2t = round2timer_ptr->find(msg->data);
        if(r2t != round2timer_ptr->end())
        {
            round2timer_ptr->erase(r2t);
            r2t->second->deactive();
        }

        sendFifoMsg(reqFifofd, nullptr, msg);
        free(msg);
    };

    round2timer[msg->data] = timer;
    timeout = timerQ.pushTimer(timer);
    if(timeout > 0) timeout *= 1000;
}

void Server::sendStatus(Session* s, Msg* msg)
{
    size_t msgLen = msg->len + sizeof(Msg::len_t);
    encodeMsg(msg);
    s->writeBuf((char*)msg, msgLen);

    free(msg);
}


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


    s->timer->deactive();
    s->timer.reset( new Timer( *(s->timer) ) );
    s->timer->active();
    s->timer->setPeriod(SessionTimeout);

    timeout = timerQ.pushTimer(s->timer);
    if(timeout > 0) timeout *= 1000;


    decodeMsg(msg);
    sendFifoMsg(requestFifofd, s, msg);

    free(msg);

    return sizeof(Msg::len_t) + msgLen;
}

int Server::do_recvResp()
{
    auto p = recvFifoMsg(responseFifofd);
    Session* s = p.first;
    Msg* msg = p.second;

    if(msg->type == STARTROUNDTIMER)
        startRoundTimer(msg);
    else if(msg->type == DISABLEROUNDTIMER)
        disableRoundTimer(msg);
    else if(msg->type == STATUS)
        sendStatus(s, msg);

    return 1;
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


        Session* s = new Session(fd, epfd);
        s->timer.reset(new Timer);
        s->timer->setPeriod(SessionTimeout);
        s->timer->cb = std::bind(&Server::shutdownSession, this, s);

        timeout = timerQ.pushTimer(s->timer);
        if(timeout > 0) timeout *= 1000;


        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.ptr = s;

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

    ev.data.ptr = new Session(responseFifofd);
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, responseFifofd, &ev) == -1)
    { perror("epoll_ctl add responseFifofd"); return; }

    numOpenFds = 1;
    while(numOpenFds > 0)
    {
        if(numOpenFds > eventCnt)
        {
            eventCnt = numOpenFds;
            evlist = (struct epoll_event*)realloc(evlist,
                sizeof(struct epoll_event) * eventCnt);
        }

        int ready = epoll_wait(epfd, evlist, numOpenFds, timeout);

        if(!ready)
        {
            timeout = timerQ.updateTimers( time(NULL) );
            if(timeout > 0) timeout *= 1000;
        }

        for(int i=0; i < ready; i++)
        {
            Session* s = (Session*)(evlist[i].data.ptr);

            int r = 0;
            if(evlist[i].events & EPOLLIN)
            {
                if(s->fd == listener)
                { do_accecpt(listener); continue; }
                if(s->fd == responseFifofd)
                { do_recvResp(); continue; }
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
}

int Server::shutdownSession(Session* s)
{
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, s->fd, NULL) == -1)
    { perror("epoll_ctl del"); return -1; }
    close(s->fd);
    numOpenFds--;

    Msg msg;
    msg.len = sizeof(Msg::type_t);
    msg.type = MsgType::SHUTDOWN;
    sendFifoMsg(requestFifofd, s, &msg);

    delete s;

    printf("disconnect\n");

    return 0;
}

Server::Server(int p)
    : port(p), eventCnt(20),
      evlist( (struct epoll_event*)malloc(sizeof(struct epoll_event) * eventCnt) ),
      timeout(-1)
{
    int res = makeFifo(requestFifoName);
    if(res < 0) throw;
    res = makeFifo(responseFifoName);
    if(res < 0) throw;

    childPid = fork();
    if(childPid > 0)
    {
        msgDispatch();
    }
    else if(childPid == 0)
    {
        requestFifofd = ::open(requestFifoName, O_WRONLY);
        responseFifofd = ::open(responseFifoName, O_RDONLY);
        if(requestFifofd < 0 || responseFifofd < 0)
        { perror("open"); throw; }
    }
    else { perror("fork"); throw; }
}

Server::~Server()
{
    if(evlist)
    {
        free(evlist);
        evlist = nullptr;
    }
    close(epfd);

    close(requestFifofd);
    close(responseFifofd);

    waitpid( childPid, NULL, 0 );
}