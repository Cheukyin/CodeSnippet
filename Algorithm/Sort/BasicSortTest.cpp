#include "BasicSort.hpp"
#include <assert.h>
#include <algorithm>
#include <vector>

int main()
{
    std::vector<int> vec{5,3,4,6,7,5,7,5,78};
    std::vector<int> vecbench(vec);

    // ---------------------------------------
    CYTL::insertionSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    vec = {5,5, 5, 5, 5, 1};
    vecbench = vec;

    CYTL::insertionSort(vec.begin(), vec.end());
    std::sort(vecbench.begin(), vecbench.end());
    assert(vec == vecbench);

    return 0;
}










