#include "Shuffle.hpp"
#include <vector>
#include <iostream>

int main()
{
    std::vector<int> vec{1,2,3,4,5,6,7,8,9,10};
    CYTL::shuffle(vec.begin(), vec.end());

    for(int i : vec) std::cout << i << " ";
    std::cout << "\n";

    return 0;
}