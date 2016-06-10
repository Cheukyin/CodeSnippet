#include "CppUtils.hpp"

int main()
{
    // arraySize test
    int keyVals[] = {1, 2, 3, 4, 5, 6};
    static_assert(arraySize(keyVals) == 6, "arraySize error");
}










