#ifndef __CYTL__PRIORITYQUEUE__
#define __CYTL__PRIORITYQUEUE__

#include "Comparable.hpp"
#include <assert.h>
#include <cstddef>
#include <stdlib.h>

namespace CYTL
{
    template<class T>
    void swap(T& t1, T& t2)
    {
        T tmp(t1);
        t1 = t2;
        t2 = tmp;
    }

    template<class ValueType, class Comparable = Less<ValueType> >
    class PriorityQueue
    {
    public:
        PriorityQueue()
            : buf_( (ValueType*)malloc( sizeof(ValueType) ) ),
              bufSize_(1),
              itemSize_(0)
        {}

        PriorityQueue(const PriorityQueue& Q)
        {
            free(Q.buf_);

            Q.buf_ = (ValueType*)malloc(sizeof(ValueType) * bufSize_);
            for(std::size_t i = 0; i < bufSize_; i++)
                Q.buf_[i] = buf_[i];

            Q.itemSize_ = itemSize_;
        }

        void push(const ValueType& item)
        {
            assert(itemSize_ < bufSize_);
            if(itemSize_ + 1 == bufSize_)
            {
                bufSize_ = 2*(itemSize_+1) + 1;
                buf_ = (ValueType*)realloc(buf_, sizeof(ValueType) * bufSize_);
            }

            buf_[++itemSize_] = item;
            swimup(itemSize_);
        }

        void pop()
        {
            swap(buf_[0], buf_[itemSize_--]);
            sinkdown(0);

            assert(itemSize_ < bufSize_);
            if(itemSize_ <= (bufSize_-1) / 4)
            {
                bufSize_ /= 2;
                buf_ = (ValueType*)realloc(buf_, sizeof(ValueType) * bufSize_);
            }
        }

        const ValueType& top()
        { return buf_[1]; }

        bool isEmpty()
        { return itemSize_ == 0; }

        std::size_t size()
        { return itemSize_; }

        ~PriorityQueue()
        { free(buf_); }


    private:
        ValueType* buf_;
        std::size_t bufSize_;
        std::size_t itemSize_;
        Comparable cmp_;

        void swimup(size_t k)
        {
            while( k > 1 && cmp_(buf_[k/2], buf_[k]) )
            {
                swap(buf_[k], buf_[k/2]);
                k /= 2;
            }
        }

        void sinkdown(size_t k)
        {
            while(2*k <= itemSize_)
            {
                size_t j = 2*k;
                if( j < itemSize_ && cmp_(buf_[j], buf_[j+1]) ) j++;
                if( !cmp_(buf_[k], buf_[j]) ) break;
                swap(buf_[k], buf_[j]);
                k=j;
            }
        }

    };

}

#endif // __CYTL__PRIORITYQUEUE__