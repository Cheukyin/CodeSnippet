#include "Utils.hpp"
#include "UnitTest.hpp"

TEST_CASE(arraySize)
{
    int keyVals[] = {1, 2, 3, 4, 5, 6};
    static_assert(CYTL::arraySize(keyVals) == 6, "arraySize error");
}


// TEST_CASE(TypeDisplayer)
// {
//     auto x = {3};
//     CYTL::TypeDisplayer<decltype(x)> xType;
// }