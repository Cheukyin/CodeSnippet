#include "Session.hpp"
#include "User.hpp"
#include "MsgHandler.hpp"
#include "Fifo.hpp"
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int requestFifofd;
static int responseFifofd;

void loginHandler(Session* session, const std::string& username)
{
    MsgType type = STATUS;
    std::string info = "Login Successfully";

    if( !session->user->login(username) )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void logoutHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Logout Successfully";

    session->user->logout();

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void shutdownHandler(Session* session)
{
    session->user->logout();
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
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void openRoundHandler(Session* session, const std::string& roundname)
{
    MsgType type = STATUS;
    std::string info = "Open Round Successfully";

    if( !session->user->openRound(roundname) )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void joinRoundHandler(Session* session, const std::string& roundname)
{
    MsgType type = STATUS;
    std::string info = "Join Round Successfully";

    if( !session->user->joinRound(roundname) )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void quitRoundHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Quit Round Successfully";

    if( !session->user->quitRound() )
        info = session->user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void castHandler(Session* session, const std::string& gesture)
{
    MsgType type = STATUS;
    std::string info = "Cast Successfully";

    UserGesture gest;
    if(gesture == "scissor") gest = UserGesture::SCISSOR;
    else if(gesture == "paper") gest = UserGesture::PAPER;
    else if(gesture == "rock") gest = UserGesture::ROCK;
    else gest = UserGesture::UNKNOWN;

    if( gest != UserGesture::UNKNOWN && !session->user->cast(gest) )
    {
        info = session->user->error_;
        size_t dataLen = info.size() + 1;
        Msg* msg = wrapMsg(type, dataLen, info.c_str());
        sendFifoMsg(responseFifofd, session, msg);
        free(msg);
    }
}

void unknownMsgHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Unkown CMD";

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}


void msgDispatch()
{
    requestFifofd = ::open(requestFifoName, O_RDONLY);
    responseFifofd = ::open(requestFifoName, O_WRONLY);
    if(requestFifofd < 0 || responseFifofd < 0)
    { perror("open"); throw; }

    std::pair<Session*, Msg*> p;
    while(true)
    {
        p = recvFifoMsg(requestFifofd);
        Session* session = p.first;
        Msg* msg = p.second;

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
            case SHUTDOWN:
                shutdownHandler(session);

            default:
                unknownMsgHandler(session);
                break;
        }

        free(msg);
    }
}