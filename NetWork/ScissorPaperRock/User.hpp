#ifndef _SCISSORPAPERROCK_USER_
#define _SCISSORPAPERROCK_USER_

#include "Round.hpp"
#include "UserGesture.hpp"
#include <unordered_set>
#include <string>

enum class UserStat{ LOGIN, ROUND, LOGOUT };

struct Session;

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
    bool cast(UserGesture gesture);
    void logout();

    std::string username()
    { return username_; }
    int score()
    { return score_; }
    std::string round()
    { return roundname_; }
    UserGesture gesture()
    { return gesture_; }

    std::string error_;

    Session* session;

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