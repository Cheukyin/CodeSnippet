#ifndef __CYTL__BASICSORT__
#define __CYTL__BASICSORT__

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

    // [begin, end)

    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void insertionSort(Iterator begin, Iterator end,
                       Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return;

        for(Iterator iter = begin+1; iter < end; iter++)
        {
            for(Iterator it = iter; it >= begin; it--)
            {
                if( cmp(*it, *(it-1)) ) swap(*it, *(it-1));
                else break;
            }
        }
    }


    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void selectionSort(Iterator begin, Iterator end,
                       Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return;

        for(Iterator iter = begin; iter < end; iter++)
        {
            Iterator smallestIter = iter;
            for(Iterator it = iter+1; it < end; it++)
            {
                if( cmp(*it, *smallestIter) )
                    smallestIter = it;
            }

            if(smallestIter != iter) swap(*smallestIter, *iter);
        }
    }


    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void shellSort(Iterator begin, Iterator end,
                   Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return;

        size_t N = end - begin;

        size_t h = 1;
        while(h < N/3) h = 3*h + 1;

        while(h >= 1)
        {
            for(Iterator iteri = begin + h; iteri < end; iteri++)
            {
                for(Iterator iterj = iteri; iterj >= begin + h; iterj -= h)
                {
                    if( cmp(*iterj, *(iterj-h)) ) swap(*iterj, *(iterj-h));
                    else break;
                }
            }

            h /= 3;
        }
    }


    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    Iterator medianOfThree(Iterator begin, Iterator mid, Iterator end,
                           Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if( cmp(*begin, *mid) )
        {
            if( cmp(*mid, *end) ) return mid;
            else if( cmp(*begin, *end) ) return end;
            else return begin;
        }
        else
        {
            if( cmp(*begin, *end) ) return begin;
            else if( cmp(*mid, *end) ) return end;
            else return mid;
        }
    }

    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void quickSort(Iterator begin, Iterator end,
                   Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return;

        Iterator median = medianOfThree(begin, begin + (end-begin) / 2, end-1);

        typename std::iterator_traits<Iterator>::value_type pivotValue = *median;
        if(begin != median) *median = *begin;

        Iterator lo = begin, hi = end-1;
        while(lo < hi)
        {
            while( lo < hi && cmp(pivotValue, *hi) ) hi--;
            if(lo < hi) *(lo++) = *hi;
            while( lo < hi && cmp(*lo, pivotValue) ) lo++;
            if(lo < hi) *(hi--) = *lo;
        }
        *lo = pivotValue;

        quickSort(begin, lo, cmp);
        quickSort(lo+1, end, cmp);
    }


}

#endif // __CYTL__BASICSORT__