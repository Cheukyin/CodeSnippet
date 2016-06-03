#ifndef _SCISSORPAPERROCK_ROUND_
#define _SCISSORPAPERROCK_ROUND_

#include "UserGesture.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>

class User;

class Round
{
public:
    Round(): scissorCnt(0), paperCnt(0), rockCnt(0) {}

    bool empty() { return users_.empty(); };
    void joinUser(User* user);
    void quitUser(User* user);
    void registerGesture(UserGesture gesture);
    bool judge();

private:
    std::unordered_set<User*> users_;

    int scissorCnt, paperCnt, rockCnt;
};


class RoundPool
{
public:
    static RoundPool& getInstance()
    {
        static RoundPool pool = RoundPool();
        return pool;
    }

    void delRound(const std::string& roundname);
    Round* openRound(const std::string& roundname);
    Round* getRound(const std::string& roundname);

private:
    std::unordered_map<std::string, Round*> rounds_;
};

#endif // _SCISSORPAPERROCK_ROUND_