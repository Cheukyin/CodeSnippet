#include "NetworkUtils.hpp"
#include "Message.hpp"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <utility>
#include <algorithm>

char* recvMsg(int fd)
{
    int len;
    readn(fd, &len, sizeof(Msg::len_t));
    len = ntohl(len);

    int dataLen = len - sizeof(Msg::type_t);
    if(dataLen == 0) return nullptr;

    MsgType type;
    readn(fd, &type, sizeof(Msg::type_t));

    char* data = (char*)malloc(dataLen);
    readn(fd, data, dataLen);

    return data;
}

void displayStatus(int clientfd)
{
    char* data = recvMsg(clientfd);
    std::cout << data << std::endl;
    free(data);
}

void sendCmd(int clientfd, MsgType cmd, int dataLen, const char* data)
{
    char *msg = encodeMsg(cmd, dataLen, data);
    size_t wn = writen(clientfd, msg, dataLen + Msg::HeadLen);
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

    std::string cmd;
    while(true)
    {
        std::getline(std::cin, cmd);
        auto p = parseCmd(cmd);
        sendCmd(clientfd, p.first, p.second.size() + 1, p.second.c_str());
        displayStatus(clientfd);
    }

    close(clientfd);
    return 0;
}