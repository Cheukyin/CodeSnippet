#include "UnitTest.hpp"
#include "ObjectPool.hpp"

using namespace RE;

TEST_CASE(ObjectPoolTest)
{
    struct Test: public CachedObject<Test>
    {};

    std::vector<Test*> vec;
    for(int i=0; i < 40; i++)
    {
        Test* n = new Test;
        vec.push_back(n);
        for(int j=0; j<i; j++)
            EXPECT_NEQ(n, vec[j]);
    }

    for(Test* n : vec)
        delete n;

    for(int i=0; i < 40; i++)
        vec.push_back(new Test);
}