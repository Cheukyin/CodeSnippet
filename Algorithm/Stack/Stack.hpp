#ifndef __CYTL_STACK__
#define __CYTL_STACK__

#include <cstddef>
#include <stdlib.h>

namespace CYTL
{
    template<class T>
    class Stack
    {
    public:
        Stack()
            : vecSize(1),
              vec( (T*)malloc(sizeof(T) * vecSize) ),
              usecount(0)
        {}

        void push(const T& item)
        {
            if(++usecount > vecSize)
            {
                vecSize *= 2;
                vec = (T*)realloc(vec, sizeof(T)*vecSize);
            }
            vec[usecount-1] = item;
        }

        T& top()
        { return vec[usecount-1]; }

        void pop()
        {
            if(--usecount <= vecSize / 4)
            {
                vecSize /= 2;
                vec = (T*)realloc(vec, sizeof(T)*vecSize);
            }
        }

        bool isEmpty()
        { return !usecount; }

        ~Stack()
        { free(vec); }

    private:
        std::size_t vecSize;
        T* vec;
        std::size_t usecount;
    };

}

#endif // __CYTL_STACK__