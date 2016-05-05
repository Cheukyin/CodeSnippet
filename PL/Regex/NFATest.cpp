#include "Parser.hpp"
#include "ASTDump.hpp"
#include "NFA.hpp"
#include "UnitTest.hpp"

using namespace RE;

TEST_CASE(NFATest)
{
    Parser parse;
    string re;
    NFAPtr nfa;

    NFAConstructorPtr nfaCons(new NFAConstructor);

    re = "";
    nfa = nfaCons->construct( parse(re) );
    EXPECT_EQ(nfa->bfs(), "");

    re = "a";
    nfa = nfaCons->construct( parse(re) );
    EXPECT_EQ(nfa->bfs(), "a");

    re = "abc";
    nfa = nfaCons->construct( parse(re) );
    EXPECT_EQ(nfa->bfs(), "a0b0c");

    re = "a|abc";
    nfa = nfaCons->construct( parse(re) );
    EXPECT_EQ(nfa->bfs(), "00aa00b0c");

    re = "a|(ab)*c";
    nfa = nfaCons->construct( parse(re) );
    EXPECT_EQ(nfa->bfs(), "00a000ac0b");

    re = "a**";
    nfa = nfaCons->construct( parse(re) );
    EXPECT_EQ(nfa->bfs(), "00a");
}