#ifndef __CYTL_QUEUE__
#define __CYTL_QUEUE__

#include <cstddef>
#include <stdlib.h>

namespace CYTL
{
    template<class T>
    class Queue
    {
    public:
        Queue(int size)
            : vecSize(size), vec( (T*)malloc(sizeof(T) * vecSize) ),
              front_(0), rear_(0),
              isEmpty_(true), isFull_(false)
        {}

        bool enqueue(const T& item)
        {
            if( isFull() ) return false;

            rear_ = (rear_+1) % vecSize;
            vec[rear_] = item;

            isEmpty_ = false;
            if(rear_ == front_) isFull_ = true;

            return true;
        }

        bool dequeue()
        {
            if( isEmpty() ) return false;

            front_ = (front_+1) % vecSize;

            isFull_ = false;
            if(front_ == rear_) isEmpty_ = true;

            return vec[front_];
        }

        T& front()
        { return vec[(front_+1) % vecSize]; }

        bool isFull()
        { return isFull_; }

        bool isEmpty()
        { return isEmpty_; }

    private:
        std::size_t vecSize;
        T* vec;
        // (front_, rear_]
        std::size_t front_;
        std::size_t rear_;

        bool isEmpty_;
        bool isFull_;
    };
}

#endif // __CYTL_QUEUE__