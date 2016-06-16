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
        int visit(N1& n) { return n.v; }
        std::string visit(N2& n) { return n.v; }
    };

    struct V2:
        public CYTL::AcyclicBaseVisitor,
        public CYTL::AcyclicVisitor<N1, int>,
        public CYTL::AcyclicVisitor<N2, std::string>
    {
        int visit(N1& n) { return n.v + 1; }
        std::string visit(N2& n) { return n.v + "2"; }
    };
}
TEST_CASE(AcyclicVisitor)
{
    AcyclicVisitorTest::N1 n1;
    AcyclicVisitorTest::N2 n2;

    AcyclicVisitorTest::V1 v1;
    EXPECT_EQ((&n1)->accept(v1), 1);
    EXPECT_EQ((&n2)->accept(v1), "N2");

    AcyclicVisitorTest::V2 v2;
    EXPECT_EQ((&n1)->accept(v2), 2);
    EXPECT_EQ((&n2)->accept(v2), "N22");
}


namespace CyclicVisitorTest
{
    struct N1; struct N2; struct N3;
    using TestVisitor = CYTL::CyclicVisitor<CYTL::TypeList<N1, N2, N3>, std::string>;

    struct N1
    { std::string v{"N1"}; DefineCyclicAccept(TestVisitor) };

    struct N2
    { std::string v{"N2"}; DefineCyclicAccept(TestVisitor) };

    struct N3
    { std::string v{"N3"}; DefineCyclicAccept(TestVisitor) };

    struct V1: public TestVisitor
    {
        std::string visit(N1& n) { return n.v; }
        std::string visit(N2& n) { return n.v; }
        std::string visit(N3& n) { return n.v; }
    };

    struct V2: public TestVisitor
    {
        std::string visit(N1& n) { return n.v + "2"; }
        std::string visit(N2& n) { return n.v + "2"; }
        std::string visit(N3& n) { return n.v + "2"; }
    };
}
TEST_CASE(CyclicVisitor)
{
    CyclicVisitorTest::N1 n1;
    CyclicVisitorTest::N2 n2;
    CyclicVisitorTest::N3 n3;

    CyclicVisitorTest::V1 v1;
    EXPECT_EQ((&n1)->accept(v1), "N1");
    EXPECT_EQ((&n2)->accept(v1), "N2");
    EXPECT_EQ((&n3)->accept(v1), "N3");

    CyclicVisitorTest::V2 v2;
    EXPECT_EQ((&n1)->accept(v2), "N12");
    EXPECT_EQ((&n2)->accept(v2), "N22");
    EXPECT_EQ((&n3)->accept(v2), "N32");
}