#include "Message.hpp"

void decodeMsg(Msg* msg)
{
    msg->len = ntohl(msg->len);
    msg->type = ntohl(msg->type);
}

Msg* wrapMsg(MsgType type, size_t dataLen, const char* data)
{
    Msg* msg = (Msg*)malloc(Msg::HeadLen + dataLen);
    msg->type = type;
    msg->len = sizeof(Msg::type_t) + dataLen;
    if(data) memcpy(msg->data, data, dataLen);

    return msg;
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

void encodeMsg(Msg* msg)
{
    msg->len = htonl(msg->len);
    msg->type = htonl(msg->type);
}