#include "NetworkUtils.hpp"
#include "Message.hpp"
#include "Timer.hpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <utility>
#include <algorithm>
#include <sys/select.h>

char* recvMsg(int fd)
{
    int len;
    if(readn(fd, &len, sizeof(Msg::len_t)) <= 0)
    {
        std::cout << "server disconnected" << std::endl;
        exit(0);
    }
    len = ntohl(len);

    MsgType type;
    if(readn(fd, &type, sizeof(Msg::type_t)) <= 0)
    {
        std::cout << "server disconnected" << std::endl;
        exit(0);
    }

    int dataLen = len - sizeof(Msg::type_t);
    if(dataLen == 0) return nullptr;

    char* data = nullptr;
    if(dataLen > 0)
    {
        data = (char*)malloc(dataLen);
        readn(fd, data, dataLen);
    }

    return data;
}

void displayStatus(int clientfd)
{
    char* data = recvMsg(clientfd);
    if(data)
    {
        std::cout << data << std::endl;
        free(data);
    }
}

void sendCmd(int clientfd, MsgType cmd, int dataLen, const char* data)
{
    char *msg = encodeMsg(cmd, dataLen, data);
    writen(clientfd, msg, dataLen + Msg::HeadLen);
    free(msg);
}

std::pair<MsgType, std::string> parseCmd(const std::string& cmd)
{
    MsgType type = UNKNOWN;
    std::string data;
    auto it = std::find(cmd.begin(), cmd.end(), ' ');

    std::string first = std::string(cmd.begin(), it);
    if(first == "login") type = LOGIN;
    else if(first == "logout") type = LOGOUT;
    else if(first == "info") type = INFO;
    else if(first == "open") type = OPENROUND;
    else if(first == "join") type = JOINROUND;
    else if(first == "quit") type = QUITROUND;
    else if(first == "cast") type = CAST;
    else type = UNKNOWN;

    if(it == cmd.end()) return std::make_pair(type, "");

    std::string snd = std::string(it+1, cmd.end());
    return std::make_pair(type, snd);
}

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cout << "Usage: " + std::string(argv[0]) + " <ip> <port>\n";
        exit(1);
    }

    char* host = argv[1];
    int port = atoi(argv[2]);
    int clientfd = openClientfd(host, port);
    if(clientfd < 0)
    { perror("client fd"); exit(1); }


    static struct timeval tv;
    tv.tv_sec = SessionTimeout/2, tv.tv_usec = 0;
    sendCmd(clientfd, HEARTBEAT, 0, nullptr);

    int serverMissCnt = 0;

    std::string cmd;
    fd_set readset;

    while(1)
    {
        int maxfd = clientfd;
        int inputfd = 0;

        FD_ZERO(&readset);
        FD_SET(clientfd, &readset);
        FD_SET(inputfd, &readset);

        int res;
        if ((res = select(maxfd+1, &readset, NULL, NULL, &tv)) < 0)
        { perror("select"); return 1; }

        if(res == 0)
        {
            if(serverMissCnt++ >= 2)
            {
                std::cout << "Server disconnected!\n";
                exit(0);
            }

            sendCmd(clientfd, HEARTBEAT, 0, nullptr);
            tv.tv_sec = SessionTimeout/2, tv.tv_usec = 0;
        }

        if( FD_ISSET(inputfd, &readset) )
        {
            // printf("from input\n");
            std::getline(std::cin, cmd);
            auto p = parseCmd(cmd);
            sendCmd(clientfd, p.first, p.second.size() + 1, p.second.c_str());
        }

        if( FD_ISSET(clientfd, &readset) )
        {
            displayStatus(clientfd);
            serverMissCnt = 0;
            // printf("from server\n");
        }
    }

    close(clientfd);
    return 0;
}
