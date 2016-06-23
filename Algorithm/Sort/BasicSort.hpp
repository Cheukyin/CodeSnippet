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
    template<class Iterator>
    struct TemporaryBuffer
    {
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
        typedef value_type* iterator;
        typedef const value_type* const_iterator;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        TemporaryBuffer(int n)
            : n(n), buf(new value_type[n]),
              start(buf), finish(buf + n)
        {}

        TemporaryBuffer(Iterator first, Iterator last)
            : n(last - first), buf(new value_type[n]),
              start(buf), finish(buf + n)
        {
            Iterator it = first;
            for(size_type i=0; i<n; i++) *(start+i) = *(it++);
        }

        iterator begin() { return start; }
        iterator end() { return finish; }

        ~TemporaryBuffer()
        { delete [] buf; }

        size_type n;
        iterator buf;
        iterator start;
        iterator finish;
    };

    // [begin, mid+1), [mid+1, end)
    template<class Iterator, class Comparable>
    void merge(Iterator begin, Iterator mid, Iterator end, Comparable cmp)
    {
        if( !cmp(*(mid+1), *mid) ) return;

        TemporaryBuffer<Iterator> tmpbuf(begin, end);

        typename TemporaryBuffer<Iterator>::iterator auxBegin = tmpbuf.begin();
        typename TemporaryBuffer<Iterator>::iterator auxEnd = tmpbuf.end();
        typename TemporaryBuffer<Iterator>::iterator auxMid = auxBegin + (mid - begin);

        typename TemporaryBuffer<Iterator>::iterator it1 = auxBegin, it2 = auxMid+1;

        while(it1 < auxMid+1 && it2 < auxEnd)
        {
            if( cmp(*it1, *it2) ) *(begin++) = *(it1++);
            else *(begin++) = *(it2++);
        }

        while(it1 < auxMid+1) *(begin++) = *(it1++);
        while(it2 < auxEnd) *(begin++) = *(it2++);
    }

    // [begin, end]
    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void recursiveMergeSortHelper(Iterator begin, Iterator end,
                                  Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    {
        if(begin >= end) return;
        if(end-begin+1 <= 3) return insertionSort(begin, end+1, cmp);

        Iterator mid = begin + (end - begin) / 2;

        recursiveMergeSortHelper(begin, mid, cmp);
        recursiveMergeSortHelper(mid+1, end, cmp);

        merge(begin, mid, end+1, cmp);
    }

    // [begin, end)
    template<class Iterator, class Comparable = Less<typename std::iterator_traits<Iterator>::value_type> >
    void recursiveMergeSort(Iterator begin, Iterator end,
                            Comparable cmp = Less<typename std::iterator_traits<Iterator>::value_type>())
    { recursiveMergeSortHelper(begin, end-1, cmp); }

}

#endif // __CYTL__BASICSORT__