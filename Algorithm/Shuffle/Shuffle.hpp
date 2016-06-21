#ifndef __CYTL__SHUFFLE__
#define __CYTL__SHUFFLE__

#include <algorithm>
#include <cstddef>
#include <stdlib.h>
#include <time.h>

namespace CYTL
{
    template<class Iterator>
    void shuffle(Iterator begin, Iterator end)
    {
        if(begin >= end) return;

        srandom(time(NULL));

        for(Iterator iter = begin+1; iter < end; iter++)
            std::swap( *iter, *( begin + random() % (iter-begin) ) );
    }
}

#endif // __CYTL__SHUFFLE__