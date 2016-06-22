#ifndef __CYTL__SELECTION__
#define __CYTL__SELECTION__

#include "Comparable.hpp"
#include <iterator>
#include <cstddef>

namespace CYTL
{
    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    Iterator select(Iterator begin, Iterator end, std::size_t k,
                    Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return end;
        if(end - begin < k) return end;

        Iterator orginBegin = begin;

        while(begin < end)
        {
            typename std::iterator_traits<Iterator>::value_type pivotValue = *begin;

            Iterator lo = begin, hi = end-1;
            while(lo < hi)
            {
                while( lo < hi && cmp(pivotValue, *hi) ) hi--;
                if(lo < hi) *(lo++) = *hi;
                while( lo < hi && cmp(*lo, pivotValue) ) lo++;
                if(lo < hi) *(hi--) = *lo;
            }
            *lo = pivotValue;

            std::size_t partitionPos = lo - orginBegin;
            if(partitionPos == k) return lo;
            else if(partitionPos < k) begin = lo + 1;
            else end = lo;
        }

        return end;
    }

}

#endif // __CYTL__SELECTION__