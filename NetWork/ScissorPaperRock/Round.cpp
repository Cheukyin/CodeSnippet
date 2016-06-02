#include "Round.hpp"

void Round::joinUser(User* user)
{
    users_.insert(user);
}

void Round::quitUser(User* user)
{
    auto it = users_.find(user);
    if(it != users_.end())
        users_.erase(it);
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