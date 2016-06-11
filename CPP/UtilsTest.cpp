#include "Utils.hpp"
#include "UnitTest.hpp"

// TEST_CASE(TypeDisplayer)
// {
//     auto x = {3};
//     CYTL::TypeDisplayer<decltype(x)> xType;
// }


TEST_CASE(arraySize)
{
    int keyVals[] = {1, 2, 3, 4, 5, 6};
    static_assert(CYTL::arraySize(keyVals) == 6, "arraySize error");
}

TEST_CASE(IfThenElse)
{
    struct T; struct F;

    CYTL::StaticTypeCheckEQ<CYTL::IfThenElse<true, T, F>, T>();
    CYTL::StaticTypeCheckEQ<CYTL::IfThenElse<false, T, F>, F>();
}

TEST_CASE(EnableIf)
{
    struct T;
    CYTL::StaticTypeCheckEQ<CYTL::EnableIf<true, T>, T>();
}

TEST_CASE(RemoveQualifiers)
{
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConst<const volatile int>, volatile int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConst<int>, int>();

    CYTL::StaticTypeCheckEQ<CYTL::RemoveVolatile<const volatile int>, const int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveVolatile<int&>, int&>();

    CYTL::StaticTypeCheckEQ<CYTL::RemoveConstVolatile<const volatile int>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConstVolatile<int>, int>();

    CYTL::StaticTypeCheckEQ<CYTL::RemoveReference<int&>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveReference<int&&>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveReference<int>, int>();
}