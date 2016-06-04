#ifndef _SCISSORPAPERROCK_ROUND_
#define _SCISSORPAPERROCK_ROUND_

#include "UserGesture.hpp"
#include "Message.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>

class User;
struct Session;

class Round
{
public:
    Round(): scissorCnt(0), paperCnt(0), rockCnt(0) {}
    Round(const std::string& roundname)
        : roundname_(roundname), scissorCnt(0), paperCnt(0), rockCnt(0)
    {}

    bool empty() { return users_.empty(); };
    void joinUser(User* user);
    void quitUser(User* user);
    void registerGesture(UserGesture gesture);
    void timeout();

private:
    std::unordered_set<User*> users_;
    std::string roundname_;

    int scissorCnt, paperCnt, rockCnt;

    bool judge();
    void startTimer();
    void disableTimer();

    void unicast(MsgType type, const std::string& info, const Session* s);

    friend class RoundPool;
};


class RoundPool
{
public:
    static RoundPool& getInstance()
    {
        static RoundPool pool;
        return pool;
    }

    void delRound(const std::string& roundname);
    Round* openRound(const std::string& roundname);
    Round* getRound(const std::string& roundname);

private:
    std::unordered_map<std::string, Round*> rounds_;
};

#endif // _SCISSORPAPERROCK_ROUND_