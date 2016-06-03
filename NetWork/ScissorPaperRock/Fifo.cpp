#include "Fifo.hpp"
#include "NetworkUtils.hpp"
#include "Message.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <utility>
#include <stdio.h>

int makeFifo(const char* fifo_name)
{
    if(access(fifo_name, F_OK) == -1)
    {
        int res = mkfifo(fifo_name, 0777);
        if(res != 0)
        {
            perror("mkfifo");
            return -1;
        }
    }

    return 1;
}

void sendFifoMsg(int fifofd, Session* session, Msg* msg)
{
    size_t msgLen = sizeof(Msg::len_t) + msg->len;

    writen(fifofd, (char*)&session, sizeof(session));
    writen(fifofd, (char*)msg, msgLen);
}

std::pair<Session*, Msg*> recvFifoMsg(int fifofd)
{
    Session* s;
    readn(fifofd, (char*)&s, sizeof(s));

    int msgLen;
    readn(fifofd, (char*)&msgLen, sizeof(Msg::len_t));

    Msg* msg = (Msg*)malloc(sizeof(Msg::len_t) + msgLen);
    readn(fifofd, ((char*)&msg) + sizeof(Msg::len_t), msgLen);

    return std::make_pair(s, msg);
}