#include "Visitor.hpp"
#include "UnitTest.hpp"
#include <string>

namespace AcyclicVisitorTest
{
    struct N1: public CYTL::BaseVisitable<int>
    { int v{1}; DefineAcyclicAccept() };

    struct N2: public CYTL::BaseVisitable<std::string>
    { std::string v{"N2"}; DefineAcyclicAccept() };

    struct V1:
        public CYTL::AcyclicBaseVisitor,
        public CYTL::AcyclicVisitor<N1, int>,
        public CYTL::AcyclicVisitor<N2, std::string>
    {
        int Visit(N1& n) { return n.v; }
        std::string Visit(N2& n) { return n.v; }
    };

    struct V2:
        public CYTL::AcyclicBaseVisitor,
        public CYTL::AcyclicVisitor<N1, int>,
        public CYTL::AcyclicVisitor<N2, std::string>
    {
        int Visit(N1& n) { return n.v + 1; }
        std::string Visit(N2& n) { return n.v + "2"; }
    };
}
TEST_CASE(AcyclicVisitor)
{
    AcyclicVisitorTest::N1 n1;
    AcyclicVisitorTest::N2 n2;

    AcyclicVisitorTest::V1 v1;
    EXPECT_EQ((&n1)->Accept(v1), 1);
    EXPECT_EQ((&n2)->Accept(v1), "N2");

    AcyclicVisitorTest::V2 v2;
    EXPECT_EQ((&n1)->Accept(v2), 2);
    EXPECT_EQ((&n2)->Accept(v2), "N22");
}