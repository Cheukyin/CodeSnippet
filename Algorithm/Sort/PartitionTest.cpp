#include "Partition.hpp"
#include <assert.h>
#include <vector>
#include <iostream>

bool isOdd(int x)
{ return x % 2; }

bool lessThan5(int x)
{ return x < 5; }

int main()
{
    std::vector<int> vec{1,2,3,4,5,6,7,8};

    CYTL::partition(vec.begin(), vec.end(), isOdd);
    assert(vec == (std::vector<int>{1,7,3,5,6,4,8,2}));

    vec = {8,7,6,5,4,3,2,1};
    CYTL::partition(vec.begin(), vec.end(), lessThan5);
    assert(vec == (std::vector<int>{1,2,3,4,5,6,7,8}));

    return 0;
}










