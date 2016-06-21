#ifndef __CYTL__COMPARABLE__
#define __CYTL__COMPARABLE__

namespace CYTL
{
    template<class T>
    struct Less
    {
        bool operator()(const T& t1, const T& t2)
        { return t1 < t2; }
    };

    template<class T>
    struct Greater
    {
        bool operator()(const T& t1, const T& t2)
        { return t1 > t2; }
    };

    template<class T>
    struct Equal
    {
        bool operator()(const T& t1, const T& t2)
        { return t1 == t2; }
    };

    template<class T>
    struct Unequal
    {
        bool operator()(const T& t1, const T& t2)
        { return t1 != t2; }
    };
}

#endif // __CYTL__COMPARABLE__
