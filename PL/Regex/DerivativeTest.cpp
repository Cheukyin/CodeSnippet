#include "UnitTest.hpp"
#include "Derivative.hpp"
#include "ASTDump.hpp"
using namespace RE;

TEST_CASE(NullCheckTest)
{
    NullCheckPtr containNull(new NullCheck);
    RegexPtr re;

    re = EmptyPtr(new Empty);
    EXPECT_FALSE(containNull->check(re));

    re = SeqPtr(new Seq( NullPtr(new Null), NullPtr(new Null) ));
    EXPECT_TRUE(containNull->check(re));

    re = SeqPtr(new Seq(re, CharPtr(new Char('d'))));
    EXPECT_FALSE(containNull->check(re));

    re = AltPtr(new Alt(re, RepPtr(new Rep(re))));
    EXPECT_TRUE(containNull->check(re));
}

TEST_CASE(DerivativeTest)
{
    DerivativePtr d(new Derivative);
    ASTDumpPtr printer(new ASTDump);
    RegexPtr re;

    re = EmptyPtr(new Empty);
    string res = printer->dump(d->drv(re, 'c'));
    string res_expected = printer->dump(re);
    EXPECT_EQ(res_expected, res);

    re = CharPtr(new Char('c'));
    res = printer->dump(d->drv(re, 'd'));
    RegexPtr re_expected = EmptyPtr(new Empty);
    res_expected = printer->dump(re_expected);
    EXPECT_EQ(res_expected, res);

    re_expected = NullPtr(new Null);
    res = printer->dump(d->drv(re, 'c'));
    res_expected = printer->dump(re_expected);
    EXPECT_EQ(res_expected, res);

    re = RepPtr(new Rep(re));
    re_expected = SeqPtr(new Seq(re_expected, re));
    res = printer->dump(d->drv(re, 'c'));
    res_expected = printer->dump(re_expected);
    EXPECT_EQ(res_expected, res);

    re = re_expected;
    res = printer->dump(d->drv(re, 'c'));
    EXPECT_EQ(res, "(Alt (Seq (Empty) (Rep (Char c))) (Seq (Null) (Rep (Char c))))");

    re = d->drv(re, 'c');
    res = printer->dump(d->drv(re, 'c'));
    EXPECT_EQ(res, "(Alt (Seq (Empty) (Rep (Char c))) "
        "(Alt (Seq (Empty) (Rep (Char c))) (Seq (Null) (Rep (Char c)))))");
}