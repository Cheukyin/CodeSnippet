#ifndef __CYTL_LIFETIMETRACKER__
#define __CYTL_LIFETIMETRACKER__

#include <assert.h>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

namespace CYTL
{
    // ------------------------
    // LifetimeTracker
    class LifetimeTracker
    {
    public:
        LifetimeTracker(unsigned int x) : longevity_(x) {}
        virtual ~LifetimeTracker() {}

        friend inline bool lifetimeCompare(unsigned int longevity, const LifetimeTracker* p);

    private:
        unsigned int longevity_;
    };

    inline bool lifetimeCompare(unsigned int longevity, const LifetimeTracker* p)
    { return p->longevity_ > longevity; }

    // ConcreteLifetimeTracker
    template<class T, class Destroyer>
    class ConcreteLifetimeTracker: public LifetimeTracker
    {
    public:
        ConcreteLifetimeTracker(T* p, unsigned int longevity, Destroyer d)
            : LifetimeTracker(longevity), pTracked_(p), destroyer_(d)
        {}

        ~ConcreteLifetimeTracker()
        { destroyer_(pTracked_); }

    private:
        T* pTracked_;
        Destroyer destroyer_;
    };

    // Deleter
    template<class T>
    struct Deleter
    {
        static void del(T* pObj)
        { delete pObj; }
    };

    using TrackerArray = LifetimeTracker**;
    extern TrackerArray pTrackerArray;
    extern unsigned int elements;

    template<class T>
    void atExitFn()
    {
        assert(elements > 0 && pTrackerArray);

        LifetimeTracker* pTop = pTrackerArray[elements - 1];
        pTrackerArray = static_cast<TrackerArray>( std::realloc(pTrackerArray, sizeof(T*) * --elements) );
        delete pTop;
    }

    // setLongevity
    template<class T, class Destroyer>
    void setLongevity(T* pDynObj, unsigned int longevity, Destroyer d = Deleter<T>::del)
    {
        TrackerArray pNewArray = static_cast<TrackerArray>( std::realloc(pTrackerArray, sizeof(T*) * (elements+1)) );
        if(!pNewArray) throw std::bad_alloc();
        pTrackerArray = pNewArray;

        LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(pDynObj, longevity, d);

        TrackerArray pos = std::upper_bound(pTrackerArray, pTrackerArray + elements, longevity, lifetimeCompare);
        std::copy_backward(pos, pTrackerArray + elements, pTrackerArray + elements+1);

        *pos = p;
        ++elements;
        std::atexit(atExitFn<T>);
    }

}

#endif // __CYTL_LIFETIMETRACKER__