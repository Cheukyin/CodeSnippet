#include "Round.hpp"
#include "User.hpp"
#include "Message.hpp"
#include "Session.hpp"

void Round::joinUser(User* user)
{
    users_.insert(user);
}

void Round::quitUser(User* user)
{
    auto it = users_.find(user);
    if(it != users_.end())
        users_.erase(it);

    judge();
}

void Round::registerGesture(UserGesture g)
{
    switch(g)
    {
        case UserGesture::SCISSOR:
            scissorCnt++;
            break;
        case UserGesture::PAPER:
            paperCnt++;
            break;
        case UserGesture::ROCK:
            rockCnt++;
            break;

        default: break;
    }

    judge();
}

bool Round::judge()
{
    if(users_.size() == 1) return false;
    if(scissorCnt + paperCnt + rockCnt < users_.size()) return false;

    int gestureClassNum = (scissorCnt > 0) + (paperCnt > 0) + (rockCnt > 0);

    std::unordered_map<int, std::string> gesture2result;

    if(gestureClassNum == 1 || gestureClassNum == 3)
    {
        gesture2result[CastGesture2Int(UserGesture::SCISSOR)] = "Even";
        gesture2result[CastGesture2Int(UserGesture::ROCK)] = "Even";
        gesture2result[CastGesture2Int(UserGesture::PAPER)] = "Even";
    }
    else if(scissorCnt > 0 && paperCnt > 0)
    {
        gesture2result[CastGesture2Int(UserGesture::SCISSOR)] = "Win";
        gesture2result[CastGesture2Int(UserGesture::PAPER)] = "Lose";
    }
    else if(scissorCnt > 0 && rockCnt > 0)
    {
        gesture2result[CastGesture2Int(UserGesture::SCISSOR)] = "Lose";
        gesture2result[CastGesture2Int(UserGesture::ROCK)] = "Win";
    }
    else if(paperCnt > 0 && rockCnt > 0)
    {
        gesture2result[CastGesture2Int(UserGesture::ROCK)] = "Lose";
        gesture2result[CastGesture2Int(UserGesture::PAPER)] = "Win";
    }

    MsgType type = STATUS;
    std::string info;
    for(User* user : users_)
    {
        info = gesture2result[CastGesture2Int( user->gesture() )];

        size_t dataLen = info.size() + 1;
        char* msg = encodeMsg(type, dataLen, info.c_str());
        user->session->writeBuf(msg, Msg::HeadLen + dataLen);
        free(msg);
    }

    scissorCnt = 0, rockCnt = 0, paperCnt = 0;

    return true;
}


Round* RoundPool::openRound(const std::string& roundname)
{
    if(rounds_.find(roundname) != rounds_.end())
        return nullptr;
    rounds_[roundname] = new Round;
    return rounds_[roundname];
}

Round* RoundPool::getRound(const std::string& roundname)
{
    if(rounds_.find(roundname) == rounds_.end())
        return nullptr;
    return rounds_[roundname];
}

void RoundPool::delRound(const std::string& roundname)
{
    auto it = rounds_.find(roundname);
    if(it == rounds_.end()) return;
    rounds_.erase(it);
}