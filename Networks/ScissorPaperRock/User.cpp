#include "User.hpp"
#include <iostream>

std::unordered_set<std::string> User::usernameSet;

bool User::login(const std::string& username)
{
    if(stat_ != UserStat::LOGOUT)
    {
        error_ = "status is not LOGOUT";
        return false;
    }

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TRICK: add this to avoid "floating point exception"
    if(usernameSet.empty()) usernameSet.reserve(1);
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if(usernameSet.find(username) != usernameSet.end())
    {
        error_ = "Username Already In Used";
        return false;
    }

    username_ = username;
    usernameSet.insert(username);
    stat_ = UserStat::LOGIN;

    std::cout << username_ << " login\n";

    return true;
}

bool User::openRound(const std::string& roundname)
{
    if(stat_ == UserStat::LOGOUT)
    {
        error_ = "status is LOGOUT";
        return false;
    }
    if(stat_ != UserStat::LOGIN)
    {
        error_ = "You haven't quit";
        return false;
    }

    quitRound();
    roundname_ = roundname;
    round_ = roundpool_.openRound(roundname);

    if(!round_)
    {
        error_ = "Round already exists";
        return false;
    }

    std::cout << username_ << " open round "
              << roundname_ << "\n";

    return justJoinRound_();
}

bool User::joinRound(const std::string& roundname)
{
    if(stat_ == UserStat::LOGOUT)
    {
        error_ = "status is LOGOUT";
        return false;
    }
    if(stat_ != UserStat::LOGIN)
    {
        error_ = "You haven't quit";
        return false;
    }

    quitRound();
    roundname_ = roundname;
    round_ = roundpool_.getRound(roundname);

    if(round_)
        std::cout << username_ << " join round "
                  << roundname_ << "\n";

    return justJoinRound_();
}

bool User::cast(UserGesture gesture)
{
    if(stat_ == UserStat::LOGOUT)
    {
        error_ = "status is LOGOUT";
        return false;
    }
    if(stat_ != UserStat::ROUND)
    {
        error_ = "You haven't joined a round";
        return false;
    }

    gesture_ = gesture;
    round_->registerGesture(gesture);

    std::string gestureName;
    switch(gesture)
    {
        case UserGesture::SCISSOR:
            gestureName = "scissor"; break;
        case UserGesture::PAPER:
            gestureName = "paper"; break;
        case UserGesture::ROCK:
            gestureName = "rock"; break;

        default: break;
    }
    std::cout << username_ << " cast "
              << gestureName << "\n";

    return true;
}

bool User::quitRound()
{
    if(stat_ == UserStat::LOGOUT)
    {
        error_ = "status is LOGOUT";
        return false;
    }

    if(round_ != nullptr)
    {
        std::cout << username_ << " quit round "
                  << roundname_ << "\n";

        round_->quitUser(this);
        if( round_->empty() )
        {
            roundpool_.delRound(roundname_);
            delete round_;
        }
        round_ = nullptr;
        stat_ = UserStat::LOGIN;
        return true;
    }
    else
    {
        error_ = "Round Not Found";
        return false;
    }
}

void User::logout()
{
    if(stat_ == UserStat::LOGOUT)
    {
        error_ = "status is LOGOUT";
        return;
    }

    auto it = usernameSet.find(username_);
    if(it != usernameSet.end()) usernameSet.erase(it);

    quitRound();

    stat_ = UserStat::LOGOUT;

    std::cout << username_ << " logout\n";
}

bool User::justJoinRound_()
{
    if(round_ == nullptr)
    {
        error_ = "Round Not Found";
        stat_ = UserStat::LOGIN;
        return false;
    }

    stat_ = UserStat::ROUND;
    round_->joinUser(this);
    return true;
}

User::~User()
{
    auto it = usernameSet.find(username_);
    if(it != usernameSet.end()) usernameSet.erase(it);

    if(stat_ == UserStat::ROUND && round_)
        round_->quitUser(this);
}