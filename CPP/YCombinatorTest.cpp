#include "Utils.hpp"
#include "YCombinator.hpp"
#include "UnitTest.hpp"

TEST_CASE(YCombinator)
{
    using CYTL::U2RFunc;

    // fact = λn. n == 0 ? 1 : n*fact(n-1)
    // _fact = λf. λn. n == 0 ? 1 : n*f(n-1)
    // fact = Y(_fact)
    U2RFunc< U2RFunc<int, int>, U2RFunc<int, int> >
    _fact = [](U2RFunc<int, int> f) -> U2RFunc<int, int> {
        return [f](int n) -> int {
            return n==0 ? 1 : n*f(n-1);
        };
    };
    U2RFunc<int, int> fact = CYTL::yCombinator(_fact);

    EXPECT_EQ(fact(3), 6);
    EXPECT_EQ(fact(10), 3628800);
}