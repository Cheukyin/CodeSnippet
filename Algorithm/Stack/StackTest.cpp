#include "Stack.hpp"
#include <assert.h>

int main()
{
    CYTL::Stack<int> stack;

    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    stack.push(5);

    assert(stack.top() == 5);
    assert( !stack.isEmpty() );

    stack.pop();
    stack.pop();
    stack.pop();
    assert(stack.top() == 2);

    stack.push(4);
    stack.push(5);
    assert(stack.top() == 5);

    return 0;
}
