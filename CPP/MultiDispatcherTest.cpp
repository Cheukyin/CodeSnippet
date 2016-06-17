#include "MultiDispatcher.hpp"
#include "UnitTest.hpp"
#include <string>
#include <memory>

namespace
{
    struct A { virtual ~A() {} };
    struct A1: A {};
    struct A2: A {};
    struct A3: A {};
    struct A11: A1 {};

    struct B { virtual ~B() {} };
    struct B1: B {};
    struct B2: B {};
    struct B3: B {};
}

TEST_CASE(AsymmetricStaticDoubleDispatcher)
{
    struct TestExecutor
    {
        using ReturnType = std::string;

        ReturnType exec(A1&, B1&) { return "A1B1"; }
        ReturnType exec(A1&, B2&) { return "A1B2"; }
        ReturnType exec(A1&, B3&) { return "A1B3"; }

        ReturnType exec(A2&, B1&) { return "A2B1"; }
        ReturnType exec(A2&, B2&) { return "A2B2"; }
        ReturnType exec(A2&, B3&) { return "A2B3"; }

        ReturnType exec(A3&, B1&) { return "A3B1"; }
        ReturnType exec(A3&, B2&) { return "A3B2"; }
        ReturnType exec(A3&, B3&) { return "A3B3"; }

        ReturnType exec(A11&, B1&) { return "A11B1"; }
        ReturnType exec(A11&, B2&) { return "A11B2"; }
        ReturnType exec(A11&, B3&) { return "A11B3"; }


        ReturnType onError(A&, B&) { return "Error"; }
    };

    TestExecutor executor;

    using Dispatcher = CYTL::StaticDoubleDispatcher<TestExecutor,
                                                    A, CYTL::TypeList<A1, A2, A3, A11>,
                                                    B, CYTL::TypeList<B1, B2, B3> >;

    std::shared_ptr<A> a(new A1);
    std::shared_ptr<B> b(new B1);
    EXPECT_EQ(Dispatcher::dispatch(*a, *b, executor), "A1B1");

    a.reset(new A2);
    b.reset(new B3);
    EXPECT_EQ(Dispatcher::dispatch(*a, *b, executor), "A2B3");

    a.reset(new A3);
    b.reset(new B1);
    EXPECT_EQ(Dispatcher::dispatch(*a, *b, executor), "A3B1");

    a.reset(new A11);
    b.reset(new B1);
    EXPECT_EQ(Dispatcher::dispatch(*a, *b, executor), "A11B1");
}