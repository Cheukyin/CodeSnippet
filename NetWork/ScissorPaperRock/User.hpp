#ifndef _SCISSORPAPERROCK_USER_
#define _SCISSORPAPERROCK_USER_

#include "Round.hpp"
#include <unordered_set>
#include <string>

enum class UserStat{ LOGIN, ROUND, LOGOUT };
enum class UserGesture{ SCISSOR, PAPER, ROCK };

class User
{
public:
    User()
        : roundpool_( RoundPool::getInstance() ),
          stat_(UserStat::LOGOUT),
          round_(nullptr),
          score_(0)
    {}

    ~User();

    bool login(const std::string& username);
    bool openRound(const std::string& roundname);
    bool joinRound(const std::string& roundname);
    bool quitRound();
    void logout();

    std::string username()
    { return username_; }
    int score()
    { return score_; }
    std::string round()
    { return roundname_; }

    std::string error_;

private:
    RoundPool& roundpool_;

    std::string username_;
    UserStat stat_;
    std::string roundname_;
    Round* round_;
    UserGesture gesture_;
    int score_;

    bool justJoinRound_();

    static std::unordered_set<std::string> usernameSet;
};

#endif // _SCISSORPAPERROCK_USER_