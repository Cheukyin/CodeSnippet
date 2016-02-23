#include "Parser.hpp"
#include <assert.h>
#include <stack>

// only works in Release!!!!!!!!!!!!!!!!!

namespace RE
{
    RegexPtr Parser::operator()(const string& str)
    {
        if (!str.size()) return RegexPtr( NullPtr(new Null) );
        iter = str.begin();
        return regex();
    }

    RegexPtr Parser::regex()
    {
        if (!*iter || *iter == '|') abort();
        RegexPtr lre = seq();

        if (!*iter || *iter == ')') return lre;
        if (*iter != '|') abort();
        iter++;

        if (!*iter || *iter == '|') abort();
        RegexPtr rre = regex();

        return RegexPtr( AltPtr(new Alt(lre, rre)) );
    }

    RegexPtr Parser::seq()
    {
        RegexPtr first = factor();
        if (!*iter || *iter == '|' || *iter == ')') 
            return first;

        RegexPtr last = seq();

        return RegexPtr( SeqPtr( new Seq(first, last) ) );
    }

    RegexPtr Parser::factor()
    {
        RegexPtr re;

        if (*iter == '(') 
            re = parenRe();
        else if (*iter == '[') 
            re = charSet();
        else if (*iter != '*' && *iter != '+' && *iter != '?' && *iter != '{')
        {
            re = CharPtr(new Char(*iter));
            iter++;
        }
        else abort();
        
        while (1)
        {
            if(*iter == '*')
            { 
                re = RegexPtr( RepPtr(new Rep(re)) ); 
                iter++; 
            }
            else if(*iter == '+')
            { 
                RegexPtr r( RepPtr(new Rep(re)) );
                re = RegexPtr( SeqPtr(new Seq(re, r) ) );
                iter++; 
            }
            else if (*iter == '?')
            {
                RegexPtr r( NullPtr(new Null) );
                re = RegexPtr(AltPtr( new Alt(r, re) ) );
                iter++; 
            }
            else if (*iter == '{') 
            {
                std::pair<int, int> r = range();
                int r1 = r.first < r.second ? r.first : r.second;
                int r2 = r.first < r.second ? r.second : r.first;

                auto helper = [](int cnt, RegexPtr& re) ->RegexPtr {
                    RegexPtr tmp( NullPtr(new Null) );
                    for (int i = 0; i < cnt; i++)
                        tmp = RegexPtr( SeqPtr(new Seq(re, tmp)) );
                    return tmp;
                };

                RegexPtr tmp( helper(r2, re) );
                for (int i = r2-1; i >= r1; i--)
                    tmp = RegexPtr( AltPtr(new Alt(helper(i, re), tmp )) );
                re = tmp;
             }
            else break;
        }

        assert(*iter != '*' && *iter != '+' && *iter != '?' && *iter != '{');
        return re;
    }

    std::pair<int, int> Parser::range()
    {
        assert(*iter == '{');
        iter++;
        while (*iter && (*iter == ' ' || *iter == '\t'))
            iter++;
        if (!*iter) abort();

        int n1 = num();
        if (*iter == '}')
        {
            iter++;
            return std::pair<int, int>{n1, n1};
        }

        assert(*iter == ',');
        iter++;
        while (*iter && (*iter == ' ' || *iter == '\t'))
            iter++;
        if (!*iter) abort();

        int n2 = num();
        if (*iter != '}') abort();
        iter++;

        return std::pair<int, int>{n1, n2};
    }

    RegexPtr Parser::parenRe()
    {
        assert(*iter == '(');
        iter++;

        RegexPtr re = regex();

        if (*iter != ')') abort();
        iter++;

        return re;
    }

    RegexPtr Parser::charSet()
    {
        assert(*iter == '[');
        iter++;

        std::stack<char> chStack;
        while (*iter && *iter != ']')
            chStack.push(*iter++);
        if (!*iter) abort();
        iter++;

        if( chStack.empty() ) abort();

        char ch = chStack.top();
        chStack.pop();
        RegexPtr re( CharPtr(new Char(ch)) );

        RegexPtr l;
        while ( !chStack.empty() )
        {
            char ch = chStack.top();
            chStack.pop();

            l = CharPtr(new Char(ch));
            re = RegexPtr( AltPtr(new Alt(l, re)) );
        }

        return re;
    }

    int Parser::num()
    {
        assert(*iter >= '0' && *iter <= '9');

        int result = 0;
        while (*iter >= '0' && *iter <= '9')
            result += (*iter++ - '0');

        while (*iter && (*iter == ' ' || *iter == '\t'))
            iter++;
        if (*iter != ',' && *iter != '}') abort();

        return result;
    }

} // namespace RE