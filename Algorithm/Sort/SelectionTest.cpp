#include "Selection.hpp"
#include <vector>
#include <assert.h>
#include <iostream>

int main()
{
    std::vector<int> vec{4,5,1,2,7,8,9,3,6};
    assert(*CYTL::select(vec.begin(), vec.end(), 4) == 5);
    assert(*CYTL::select(vec.begin(), vec.end(), 6) == 7);
    assert(*CYTL::select(vec.begin(), vec.end(), 2) == 3);
    assert(*CYTL::select(vec.begin(), vec.end(), 8) == 9);
    assert(*CYTL::select(vec.begin(), vec.end(), 3) == 4);

    vec = {2,2,2,2,2,2};
    assert(*CYTL::select(vec.begin(), vec.end(), 2) == 2);
    assert(*CYTL::select(vec.begin(), vec.end(), 3) == 2);
    assert(*CYTL::select(vec.begin(), vec.end(), 4) == 2);
    assert(*CYTL::select(vec.begin(), vec.end(), 5) == 2);

    vec = {2,2,2,2,2,1};
    assert(*CYTL::select(vec.begin(), vec.end(), 0) == 1);
    assert(*CYTL::select(vec.begin(), vec.end(), 3) == 2);
    assert(*CYTL::select(vec.begin(), vec.end(), 4) == 2);
    assert(*CYTL::select(vec.begin(), vec.end(), 5) == 2);

    return 0;
}