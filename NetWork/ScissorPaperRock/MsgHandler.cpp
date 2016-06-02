#include "Session.hpp"
#include "User.hpp"
#include "MsgHandler.hpp"
#include <string>
#include <algorithm>

void loginHandler(Session* session, const std::string& username)
{
    MsgType type = STATUS;
    std::string info = "Login Successfully";

    if( !session->user->login(username) )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}

void logoutHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Logout Successfully";

    session->user->logout();

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}

void infoHandler(Session* session, const std::string& infotype)
{
    MsgType type = STATUS;
    std::string info = "Unkown Info Type";

    if(infotype == "username")
        info = session->user->username();
    else if(infotype == "score")
        info = std::to_string( session->user->score() );
    else if(infotype == "round")
        info = session->user->round();

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}

void openRoundHandler(Session* session, const std::string& roundname)
{
    MsgType type = STATUS;
    std::string info = "Open Round Successfully";

    if( !session->user->openRound(roundname) )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}

void joinRoundHandler(Session* session, const std::string& roundname)
{
    MsgType type = STATUS;
    std::string info = "Join Round Successfully";

    if( !session->user->joinRound(roundname) )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}

void quitRoundHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Quit Round Successfully";

    if( !session->user->quitRound() )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}

void castHandler(Session* session, const std::string& gesture)
{
}

void unknownMsgHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Unkown CMD";

    size_t dataLen = info.size() + 1;
    char* msg = encodeMsg(type, dataLen, info.c_str());
    session->writeBuf(msg, Msg::HeadLen + dataLen);
    free(msg);
}


void msgDispatch(Session* session, const Msg* msg)
{
    switch(msg->type)
    {
        case LOGIN:
            loginHandler( session, std::string(msg->data) );
            break;
        case LOGOUT:
            logoutHandler(session);
            break;
        case INFO:
            infoHandler( session, std::string(msg->data) );
            break;
        case OPENROUND:
            openRoundHandler( session, std::string(msg->data) );
            break;
        case JOINROUND:
            joinRoundHandler( session, std::string(msg->data) );
            break;
        case QUITROUND:
            quitRoundHandler(session);
            break;
        case CAST:
            castHandler( session, std::string(msg->data) );
            break;

        default:
            unknownMsgHandler(session);
            break;
    }
}