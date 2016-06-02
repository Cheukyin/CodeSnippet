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

    std::cout << username_ << " join round "
              << roundname_ << "\n";

    return justJoinRound_();
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
        if( round_->empty() ) delete round_;
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

    if(round_) round_->quitUser(this);
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
    if(stat_ == UserStat::ROUND)
        round_->quitUser(this);
}