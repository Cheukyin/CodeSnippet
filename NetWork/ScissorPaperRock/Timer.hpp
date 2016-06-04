#ifndef __SCISSORPAPERROCK_TIMER__
#define __SCISSORPAPERROCK_TIMER__

#include <time.h>
#include <queue>
#include <vector>
#include <memory>
#include <functional>

const int SessionTimeout = 10; // sec
const int RoundTimeout = 20; // sec

struct Timer
{
    std::function<void()> cb;

    Timer(): isActive(true) {}

    void setPeriod(int prd)
    {
        period = prd;
        expires = time(NULL) + period;
    }

    void deactive() { isActive = false; }
    void active() { isActive = true; }

private:
    time_t expires; // sec
    int period; // sec

    bool isActive;

    friend struct TimerQueue;
    friend struct TimerCmp;
};

using TimerPtr = std::shared_ptr<Timer>;

struct TimerCmp
{
    bool operator()(const TimerPtr t1, const TimerPtr t2)
    { return t1->expires > t2->expires; }
};


struct TimerQueue
{
    std::priority_queue<TimerPtr, std::vector<TimerPtr>, TimerCmp> timerQ;

    // return sec
    int updateTimers(time_t current);

    // return sec
    int pushTimer(const TimerPtr& timer);
};

#endif // __SCISSORPAPERROCK_TIMER__














