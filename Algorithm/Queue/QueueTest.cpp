#include "Queue.hpp"
#include <assert.h>

int main()
{
    CYTL::Queue<int> Q(5);

    assert( !Q.isFull() );
    assert( Q.isEmpty() );

    assert( Q.enqueue(1) );
    assert( Q.enqueue(2) );
    assert( Q.enqueue(3) );
    assert( Q.enqueue(4) );
    assert( Q.enqueue(5) );

    assert( !Q.enqueue(6) );
    assert( !Q.isEmpty() );
    assert( Q.isFull() );

    assert(Q.front() == 1);
    assert( Q.dequeue() );
    assert(Q.front() == 2);
    assert( Q.dequeue() );
    assert(Q.front() == 3);
    assert( Q.dequeue() );
    assert(Q.front() == 4);
    assert( Q.dequeue() );
    assert(Q.front() == 5);
    assert( Q.dequeue() );

    assert( !Q.dequeue() );
    assert( Q.isEmpty() );
    assert( !Q.isFull() );

    assert( Q.enqueue(2) );
    assert( Q.enqueue(3) );

    assert(Q.front() == 2);
    assert( Q.dequeue() );
    assert(Q.front() == 3);
    assert( Q.dequeue() );

    return 0;
}