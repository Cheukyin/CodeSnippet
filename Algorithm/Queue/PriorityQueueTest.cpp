#include "PriorityQueue.hpp"
#include <assert.h>
#include <iostream>

int main()
{
    CYTL::PriorityQueue<int> maxQ;

    assert( maxQ.isEmpty() );

    maxQ.push(1);
    assert( !maxQ.isEmpty() );
    assert(maxQ.size() == 1);
    assert(maxQ.top() == 1);

    maxQ.push(8);
    maxQ.push(6);
    maxQ.push(1);
    maxQ.push(8);
    maxQ.push(2);
    maxQ.push(4);
    maxQ.push(4);
    maxQ.push(5);
    // maxQ.push(4);

    assert( !maxQ.isEmpty() );
    assert(maxQ.size() == 9);
    // std::cout << maxQ.top() << std::endl;
    assert(maxQ.top() == 8);

    maxQ.pop();
    assert(maxQ.size() == 8);
    assert(maxQ.top() == 8);

    maxQ.pop();
    assert(maxQ.size() == 7);
    assert(maxQ.top() == 6);

    maxQ.pop();
    maxQ.pop();
    assert(maxQ.size() == 5);
    assert(maxQ.top() == 4);

    maxQ.pop();
    maxQ.pop();
    assert(maxQ.size() == 3);
    assert(maxQ.top() == 2);

    maxQ.pop();
    maxQ.pop();
    assert(maxQ.size() == 1);
    assert(maxQ.top() == 1);

    maxQ.push(5);
    assert(maxQ.size() == 2);
    assert(maxQ.top() == 5);

    maxQ.pop();
    maxQ.pop();
    assert( maxQ.isEmpty() );

    return 0;
}