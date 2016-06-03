#include "Fifo.hpp"
#include "NetworkUtils.hpp"
#include "Message.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <utility>


void sendFifoMsg(int fifofd, Session* session, Msg* msg)
{
    size_t msgLen = sizeof(Msg::len_t) + msg->len;

    writen(fifofd, (char*)&session, sizeof(session));
    writen(fifofd, (char*)msg, msgLen);
}

std::pair<Session*, Msg*> decodeFifoMsg(int fifofd)
{
    Session* s;
    readn(fifofd, (char*)&s, sizeof(s));

    int msgLen;
    readn(fifofd, (char*)&msgLen, sizeof(Msg::len_t));

    Msg* msg = (Msg*)malloc(sizeof(Msg::len_t) + msgLen);
    readn(fifofd, ((char*)&msg) + sizeof(Msg::len_t), msgLen);

    return std::make_pair(s, msg);
}