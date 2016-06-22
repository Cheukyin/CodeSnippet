#include "Partition.hpp"
#include <assert.h>
#include <vector>
#include <iostream>

bool isOdd(int x)
{ return x % 2; }

bool lessThan5(int x)
{ return x < 5; }

int p5(int x)
{ return x<5 ? 1 : (x==5 ? 0 : -1); }

int main()
{
    //----------------------------------------------
    // partition
    std::vector<int> vec{1,2,3,4,5,6,7,8};

    CYTL::partition(vec.begin(), vec.end(), isOdd);
    assert(vec == (std::vector<int>{1,7,3,5,6,4,8,2}));

    vec = {8,7,6,5,4,3,2,1};
    CYTL::partition(vec.begin(), vec.end(), lessThan5);
    assert(vec == (std::vector<int>{1,2,3,4,5,6,7,8}));


    //---------------------------------------------------
    // three way partition
    vec = {8,7,5,5,4,3,5,1,5,10,7,5};
    CYTL::threeWayPartition(vec.begin(), vec.end(), p5);
    assert(vec == (std::vector<int>{4,3,1,5,5,5,5,5,10,7,7,8}));

    vec = {1,5,5,5,5,5,5,5,5,5,5,5};
    CYTL::threeWayPartition(vec.begin(), vec.end(), p5);
    assert(vec == (std::vector<int>{1,5,5,5,5,5,5,5,5,5,5,5}));

    return 0;
}