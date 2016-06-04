#include "Session.hpp"
#include "User.hpp"
#include "MsgHandler.hpp"
#include "Fifo.hpp"
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_map>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int requestFifofd;
int responseFifofd;

static std::unordered_map<Session*, User*> s2u;

static User* user;

static void sigintHandler(int sign_no)
{
    close(requestFifofd);
    close(responseFifofd);
    for(auto& p : s2u)
        if(p.second) delete user;

    printf("child exit\n");
    exit(0);
}

void loginHandler(Session* session, const std::string& username)
{
    MsgType type = STATUS;
    std::string info = "Login Successfully";

    assert(user == nullptr);
    user = new User;
    user->session = session;

    if( !user->login(username) )
    {
        info = user->error_;
        delete user;
    }
    else s2u[session] = user;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void logoutHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Logout Successfully";

    if(!user) return;
    user->logout();

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void shutdownHandler(Session* session)
{
    if(!user) return;

    user->logout();

    auto it = s2u.find(session);
    if(it != s2u.end())
    {
        delete user;
        s2u.erase(it);
    }
}

void infoHandler(Session* session, const std::string& infotype)
{
    MsgType type = STATUS;
    std::string info = "Unkown Info Type";

    if(!user) return;

    if(infotype == "username")
        info = user->username();
    else if(infotype == "score")
        info = std::to_string( user->score() );
    else if(infotype == "round")
        info = user->round();

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void openRoundHandler(Session* session, const std::string& roundname)
{
    MsgType type = STATUS;
    std::string info = "Open Round Successfully";

    if(!user) return;

    if( !user->openRound(roundname) )
        info = user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void joinRoundHandler(Session* session, const std::string& roundname)
{
    MsgType type = STATUS;
    std::string info = "Join Round Successfully";

    if(!user) return;

    if( !user->joinRound(roundname) )
        info = user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void quitRoundHandler(Session* session)
{
    MsgType type = STATUS;
    std::string info = "Quit Round Successfully";

    if(!user) return;

    if( !user->quitRound() )
        info = user->error_;

    size_t dataLen = info.size() + 1;
    Msg* msg = wrapMsg(type, dataLen, info.c_str());
    sendFifoMsg(responseFifofd, session, msg);
    free(msg);
}

void castHandler(Session* session, const std::string& gesture)
{
    MsgType type = STATUS;
    std::string info = "Cast Successfully";

    if(!user) return;

    UserGesture gest;
    if(gesture == "scissor") gest = UserGesture::SCISSOR;
    else if(gesture == "paper") gest = UserGesture::PAPER;
    else if(gesture == "rock") gest = UserGesture::ROCK;
    else gest = UserGesture::UNKNOWN;

    if( gest != UserGesture::UNKNOWN && !user->cast(gest) )
    {
        info = user->error_;
        size_t dataLen = info.size() + 1;
        Msg* msg = wrapMsg(type, dataLen, info.c_str());
        sendFifoMsg(responseFifofd, session, msg);
        free(msg);
    }
}

void roundtimeoutHandler(const std::string& roundname)
{
    RoundPool& roundpool_( RoundPool::getInstance() );
    Round* round = roundpool_.getRound(roundname);
    if(!round) return;

    round->timeout();
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
    signal(SIGINT, sigintHandler);

    requestFifofd = ::open(requestFifoName, O_RDONLY);
    responseFifofd = ::open(responseFifoName, O_WRONLY);
    if(requestFifofd < 0 || responseFifofd < 0)
    { perror("open"); throw; }

    std::pair<Session*, Msg*> p;
    while(true)
    {
        p = recvFifoMsg(requestFifofd);
        Session* session = p.first;
        Msg* msg = p.second;

        auto it = s2u.find(session);
        if(it == s2u.end()) user = nullptr;
        else user = it->second;

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
                break;
            case ROUNDTIMEOUT:
                roundtimeoutHandler( std::string(msg->data) );
                break;

            default:
                unknownMsgHandler(session);
                break;
        }

        free(msg);
    }
}