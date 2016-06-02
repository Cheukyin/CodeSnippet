#ifndef _SCISSORPAPERROCK_ROUND_
#define _SCISSORPAPERROCK_ROUND_

#include <string>
#include <unordered_map>
#include <unordered_set>

class User;

class Round
{
public:
    bool empty() { return users_.empty(); };
    void joinUser(User* user);
    void quitUser(User* user);

private:
    std::unordered_set<User*> users_;
};


class RoundPool
{
public:
    static RoundPool& getInstance()
    {
        static RoundPool pool = RoundPool();
        return pool;
    }

    Round* openRound(const std::string& roundname);
    Round* getRound(const std::string& roundname);

private:
    std::unordered_map<std::string, Round*> rounds_;
};

#endif // _SCISSORPAPERROCK_ROUND_