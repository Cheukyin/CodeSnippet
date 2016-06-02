#ifndef _SCISSORPAPERROCK_MESSAGE_
#define _SCISSORPAPERROCK_MESSAGE_

#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

enum MsgType{
    LOGIN,
    LOGOUT,
    INFO,
    OPENROUND,
    JOINROUND,
    QUITROUND,
    CAST, // -------- client side -------

    STATUS, // server side

    UNKNOWN
};

struct Msg
{
    using len_t = uint32_t;
    using type_t = uint32_t;
    static const size_t HeadLen = sizeof(len_t) + sizeof(type_t);

    len_t len; // not include this word
    type_t type;
    char data[0];
};

static void decodeMsg(Msg* msg)
{
    msg->len = ntohl(msg->len);
    msg->type = ntohl(msg->type);
}

static char* encodeMsg(MsgType type, size_t dataLen, const char* data)
{
    Msg* msg = (Msg*)malloc(Msg::HeadLen + dataLen);
    msg->type = type;
    msg->len = sizeof(Msg::type_t) + dataLen;
    if(data) memcpy(msg->data, data, dataLen);

    msg->len = htonl(msg->len);
    msg->type = htonl(msg->type);

    return (char*)msg;
}

#endif // _SCISSORPAPERROCK_MESSAGE_