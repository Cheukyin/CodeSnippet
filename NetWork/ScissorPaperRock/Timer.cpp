#include "Timer.hpp"

// return sec
int TimerQueue::updateTimers(time_t current)
{
    TimerPtr timer;
    while( !timerQ.empty() )
    {
        timer = timerQ.top();

        if(timer->isActive)
        {
            if(current >= timer->expires)
                timer->cb();
            else break;
        }

        timerQ.pop();
    }

    if( timerQ.empty() ) return -1;

    int nextPeriod = timerQ.top()->expires - time(NULL);
    return nextPeriod < 0 ? 0 : nextPeriod;
}

// return sec
int TimerQueue::pushTimer(const TimerPtr& timer)
{
    timerQ.push(timer);

    TimerPtr timer_;
    while( !timerQ.empty() )
    {
        timer_ = timerQ.top();
        if(timer_->isActive) break;
        timerQ.pop();
    }

    if( timerQ.empty() ) return -1;

    int nextPeriod = timerQ.top()->expires - time(NULL);
    return nextPeriod < 0 ? 0 : nextPeriod;
}