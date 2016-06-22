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

    // ------------------------------------------------------
    // insertion sort
    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void insertionSort(Iterator begin, Iterator end,
                       Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return;

        for(Iterator iter = begin+1; iter < end; iter++)
        {
            typename std::iterator_traits<Iterator>::value_type target = *iter;
            Iterator it = iter;
            for(; it > begin; it--)
            {
                if( cmp(target, *(it-1)) ) *it = *(it-1);
                else break;
            }
            *it = target;
        }
    }


    // ------------------------------------------------------
    // selection sort
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


    // ------------------------------------------------------
    // shell sort
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
                typename std::iterator_traits<Iterator>::value_type target = *iteri;
                Iterator iterj = iteri;
                for(; iterj >= begin + h; iterj -= h)
                {
                    if( cmp(target, *(iterj-h)) ) *iterj = *(iterj-h);
                    else break;
                }
                *iterj = target;
            }

            h /= 3;
        }
    }


    // ------------------------------------------------------
    // quick sort
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


    // ------------------------------------------------------
    // recursive merge sort
    template<class Iterator, class Comparable>
    void merge(Iterator begin, Iterator mid, Iterator end, Iterator auxBegin, Comparable cmp)
    {
        Iterator it1 = begin, it2 = mid+1;

        while(it1 < mid+1 && it2 < end)
        {
            if( cmp(*it1, *it2) ) *auxBegin = *(it1++);
            else *auxBegin = *(it2++);
        }

        while(it1 < mid+1) *auxBegin = *(it1++);
        while(it2 < end) *auxBegin = *(it2++);
    }

    template<class Iterator, class Comparable>
    void recursiveMergeSortHelper(Iterator begin, Iterator end, Iterator auxBegin, Comparable cmp)
    {
        if(begin >= end) return;

        Iterator mid = begin + (end - begin) / 2;
        Iterator auxMid = auxBegin + (end - begin) / 2;
        Iterator auxEnd = auxBegin + (end - begin);

        recursiveMergeSortHelper(begin, mid+1, auxBegin, cmp);
        recursiveMergeSortHelper(mid+1, end, auxMid+1, cmp);

        merge(auxBegin, auxMid, auxEnd, begin, cmp);
    }

    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void recursiveMergeSort(Iterator begin, Iterator end,
                            Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        Iterator auxBegin = new typename std::iterator_traits<Iterator>::value_type[end - begin];

        for(Iterator it1 = begin, it2 = auxBegin; it1 < end; it1++, it2++)
            *it2 = *it1;

        recursiveMergeSortHelper(begin, end, auxBegin, cmp);
    }

}

#endif // __CYTL__BASICSORT__