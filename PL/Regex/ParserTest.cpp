#include "Parser.hpp"
#include "ASTDump.hpp"
#include "UnitTest.hpp"

using namespace RE;

// only works in Release!!!!!!!!!!!!!!!!!

TEST_CASE(ParserTest)
{
    Parser parse;
    RegexPtr rePtr;
    string re;

    ASTDumpPtr printer(new ASTDump);

    re = "a";
    rePtr = parse(re);
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, "(Char a)");

    re = "abc";
    rePtr = parse(re);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, "(Seq (Char a) "
                                   "(Seq (Char b) (Char c)))");

    re = "a|" + re;
    rePtr = parse(re);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, "(Alt (Char a) "
                                   "(Seq (Char a) "
                                         "(Seq (Char b) (Char c))))");

    re = "ab|ec|eee";
    rePtr = parse(re);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, "(Alt (Seq (Char a) (Char b)) "
                                  "(Alt (Seq (Char e) (Char c)) "
                                       "(Seq (Char e) "
                                            "(Seq (Char e) (Char e)))))");

    re = "(?:ab|b)";
    rePtr = parse(re);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, "(Alt (Seq (Char a) (Char b)) (Char b))");

    re = "a(?:ab|b)";
    rePtr = parse(re);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, "(Seq (Char a) "
                                  "(Alt (Seq (Char a) (Char b)) (Char b)))");

    re = "a(?:ab|b)c";
    string re_res = "(Seq (Char a) "
                    "(Seq (Alt (Seq (Char a) (Char b)) (Char b)) (Char c)))";
    rePtr = parse(re);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re_res);

    string re1 = "[|[a*+?{(})]c";
    string re1_res = "(Seq (Alt (Char |) (Alt (Char [) (Alt (Char a) "
                     "(Alt (Char *) (Alt (Char +) (Alt (Char ?) "
                     "(Alt (Char {) (Alt (Char () (Alt (Char }) (Char ))))))))))) (Char c))";
    rePtr = parse(re1);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re1_res);

    string re2 = "(?:(?:" + re1 + ")|" + re + ")";
    string re2_res = "(Alt " + re1_res + " " + re_res + ")";
    rePtr = parse(re2);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re2_res);

    string re3 = re2 + "{1, 3}";
    string re3_res = "(Alt (Seq "+re2_res+" (Null)) "
        "(Alt (Seq "+re2_res+" (Seq "+re2_res+" (Null))) "
        "(Seq "+re2_res+" (Seq "+re2_res+" (Seq "+re2_res+" (Null))))))";
    rePtr = parse(re3);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re3_res);

    string re4 = re3 + "{1, 3}";
    string re4_res = "(Alt (Seq " + re3_res + " (Null)) "
        "(Alt (Seq " + re3_res + " (Seq " + re3_res + " (Null))) "
        "(Seq " + re3_res + " (Seq " + re3_res + " (Seq " + re3_res + " (Null))))))";
    rePtr = parse(re4);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re4_res);

    string re5 = re4 + "{0, 3}";
    string re5_res = "(Alt (Null) (Alt (Seq " + re4_res + " (Null)) "
        "(Alt (Seq " + re4_res + " (Seq " + re4_res + " (Null))) "
        "(Seq " + re4_res + " (Seq " + re4_res + " (Seq " + re4_res + " (Null)))))))";
    rePtr = parse(re5);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re5_res);

    string re6 = re5 + "*";
    string re6_res = "(Rep "+re5_res+")";
    rePtr = parse(re6);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re6_res);

    re6 = re6 + "+";
    re6_res = "(Seq "+re6_res+" (Rep "+re6_res+"))";
    rePtr = parse(re6);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re6_res);

    re6 = re6 + "?";
    re6_res = "(Alt (Null) " + re6_res + ")";
    rePtr = parse(re6);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re6_res);

    re6 = re6 + "{2}";
    re6_res = "(Seq "+re6_res + " (Seq " + re6_res + " (Null)))";
    rePtr = parse(re6);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re6_res);

    re6 = re6 + "a|bc|(?:a|b*|c)c|a";
    re6_res = "(Alt (Seq "+re6_res+
        " (Char a)) (Alt (Seq (Char b) (Char c)) (Alt (Seq (Alt (Char a) "
        "(Alt (Rep (Char b)) (Char c))) (Char c)) (Char a))))";
    rePtr = parse(re6);
    printer->tree = "";
    rePtr->accept(printer);
    EXPECT_EQ(printer->tree, re6_res);
}
