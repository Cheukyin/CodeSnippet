#include "Message.hpp"

void decodeMsg(Msg* msg)
{
    msg->len = ntohl(msg->len);
    msg->type = ntohl(msg->type);
}

char* encodeMsg(MsgType type, size_t dataLen, const char* data)
{
    Msg* msg = (Msg*)malloc(Msg::HeadLen + dataLen);
    msg->type = type;
    msg->len = sizeof(Msg::type_t) + dataLen;
    if(data) memcpy(msg->data, data, dataLen);

    msg->len = htonl(msg->len);
    msg->type = htonl(msg->type);

    return (char*)msg;
}