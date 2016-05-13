#include "UnitTest.hpp"
#include "NFAEngine.hpp"

using namespace RE;

TEST_CASE(NFAEngineTest)
{
    NFAEngine nullEng("");
    EXPECT_TRUE(nullEng.match(""));
    EXPECT_FALSE(nullEng.match("abcabcabcabcc"));

    NFAEngine eng0("(?:abc)**");
    EXPECT_TRUE(eng0.match(""));
    EXPECT_TRUE(eng0.match("abc"));
    EXPECT_FALSE(eng0.match("abcabcabcabcc"));

    NFAEngine eng1("a*a*[abc]aaa");
    EXPECT_TRUE(eng1.match("aaaa"));

    NFAEngine eng2("abc|(?:a*(?:cb)?)+");
    EXPECT_TRUE(eng2.match("cbcbcbaaa"));
    EXPECT_FALSE(eng2.match("aabcaa"));

    NFAEngine eng3("[012]{1,3}(?:-[34567890]{1,3}){3}");
    EXPECT_TRUE(eng3.match("21-3-456-67"));
    EXPECT_FALSE(eng3.match("21-3-67"));

    NFAEngine eng4(".{3}(?:a.)+.");
    EXPECT_TRUE(eng4.match("a3ca8a;."));
    EXPECT_FALSE(eng4.match("a3ca8b;."));
}