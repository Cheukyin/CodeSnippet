/* For socket functions */
#include <sys/socket.h>
/* For fcntl */
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <iostream>

class Server
{
public:
    Server(int p): port(p) {};
    ~Server() {};

    void run();

private:
    struct sockaddr_in sin;
    int port;

    int epfd;
    int numOpenFds;

    int eventCnt;
    struct epoll_event evlist;

    std::string pidname;

    int prepare();
    int do_accecpt(int fd);
};

int Server::prepare()
{
    // adrress
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(port);

    // bind listener
    int listener = socket(AF_INET, SOCK_STREAM, 0);

    int option = 1;
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEPORT,
                  &option, sizeof(option)) < 0)
    { perror("set reuse socket"); return -1; }

    if(bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    { perror("bind"); return -1; }
    // listen
    if(listen(listener, 16) < 0)
    { perror("listen"); return -1; }

    return listener;
}

int Server::do_accecpt(int listener)
{
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);

    if(fd < 0) perror("accept");
    else
    {
        std::cout << pidname << " ";
        printf("Yeah, Accepted\n");
    }

    return fd;
}

void Server::run()
{
    // fork here, no thundering herd
    pidname = "Father";
    if(fork() == 0) pidname = "Child";

    int listener = prepare();
    if(listener < 0) return;

    epfd = epoll_create1(0);
    if(epfd == -1)
    { perror("epoll"); return; }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listener;

    // fork here, thundering herd occurs
    // pidname = "Father";
    // if(fork() == 0) pidname = "Child";

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev) == -1)
    { perror("epoll_ctl add listener"); return; }

    // fork here, thundering herd occurs
    // pidname = "Father";
    // if(fork() == 0) pidname = "Child";

    while(true)
    {
        int ready = epoll_wait(epfd, &evlist, numOpenFds, -1);

        std::cout << pidname << " ";
        printf("Epoll wait return\n");

        if(evlist.events & EPOLLIN)
        {
            std::cout << pidname << " ";
            printf("Begin to accept\n");
            if(evlist.data.fd == listener)
            { do_accecpt(listener); continue; }
        }
    }
}

int main()
{
    Server server(3331);
    server.run();
    return 0;
}