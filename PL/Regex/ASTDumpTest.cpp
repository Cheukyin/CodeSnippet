#include "UnitTest.hpp"
#include "ASTDump.hpp"

using namespace RE;

TEST_CASE(ASTDumpTest)
{
    VisitorPtr dumpPtr( ASTDumpPtr(new ASTDump) );
    RegexPtr rePtr;

    auto empty = EmptyPtr(new Empty) ;
    rePtr = empty;
    rePtr->accept(dumpPtr);
    EXPECT_EQ(DynASTDumpCast(dumpPtr)->tree, "(Empty)");

    auto null = NullPtr(new Null);
    auto c1 = CharPtr(new Char('d'));
    auto alt1 = AltPtr( new Alt(c1, null) );

    rePtr = alt1;
    DynASTDumpCast(dumpPtr)->tree = "";
    rePtr->accept(dumpPtr);
    EXPECT_EQ(DynASTDumpCast(dumpPtr)->tree, "(Alt (Char d) (Null))");

    auto seq1 = SeqPtr(new Seq(alt1, c1));
    auto alt2 = AltPtr(new Alt(seq1, alt1));
    rePtr = alt2;
    DynASTDumpCast(dumpPtr)->tree = "";
    rePtr->accept(dumpPtr);
    EXPECT_EQ(DynASTDumpCast(dumpPtr)->tree, "(Alt (Seq (Alt (Char d) (Null)) (Char d)) "
                                             "(Alt (Char d) (Null)))");

    auto rep1 = RepPtr(new Rep(seq1));
    auto alt3 = AltPtr(new Alt(alt2, rep1));
    rePtr = alt3;
    DynASTDumpCast(dumpPtr)->tree = "";
    rePtr->accept(dumpPtr);
    EXPECT_EQ(DynASTDumpCast(dumpPtr)->tree, "(Alt (Alt (Seq (Alt (Char d) (Null)) (Char d)) "
                                                   "(Alt (Char d) (Null))) "
                                             "(Rep (Seq (Alt (Char d) (Null)) (Char d))))");
}