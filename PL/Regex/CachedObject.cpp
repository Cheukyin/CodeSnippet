#include "UnitTest.hpp"
#include "ObjectPool.hpp"

using namespace RE;

TEST_CASE(ObjectPoolTest)
{
    NodePool *pool1 = NodePool::getPool();
    NodePool *pool2 = NodePool::getPool();
    EXPECT_EQ(pool1, pool2);

    delete pool1;
}