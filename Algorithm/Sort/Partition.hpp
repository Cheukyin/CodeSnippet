#ifndef __CYTL__PARTITION__
#define __CYTL__PARTITION__

#include "Comparable.hpp"
#include <iterator>

namespace CYTL
{
    template<class T>
    void swap(T& t1, T& t2)
    {
        T tmp(t1);
        t1 = t2;
        t2 = tmp;
    }

    template<class Iterator, class Pred>
    void partition(Iterator begin, Iterator end, Pred pred)
    {
        if(begin >= end) return;

        Iterator lo = begin, hi = end-1;

        while(lo <= hi)
        {
            if( pred(*lo) ) lo++;
            else swap(*lo, *(hi--));
        }
    }

    template<class Iterator, class ThreePred>
    void threeWayPartition(Iterator begin, Iterator end, ThreePred pred)
    {
        if(begin >= end) return;

        Iterator lo = begin, mid = begin, hi = end-1;

        while(mid <= hi)
        {
            if(pred(*mid) > 0) swap(*(lo++), *(mid++));
            else if(pred(*mid) == 0) mid++;
            else swap(*mid, *(hi--));
        }
    }

}

#endif // __CYTL__PARTITION__