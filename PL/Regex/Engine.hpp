#ifndef REGEX_ENGINE_H
#define REGEX_ENGINE_H

#include "Parser.hpp"
#include "Derivative.hpp"

namespace RE
{
    class Engine
    {
    public:
        Engine(const string& str)
            : rePtr( parse(str) ),
              deri(new Derivative),
              containNull(new NullCheck)
        {}

        string search(const string& str)
        {
            for (StrSizeType i = 0; i < str.size(); i++)
            {
                StrSizeType tail = searchHelper(rePtr, str, i);
                if(tail > i) return string(str.begin()+i, str.begin()+tail);
            }
            return "";
        }

        bool match(const string& str)
        { return matchHelper(rePtr, str, 0); }

    private:
        Parser parse;
        RegexPtr rePtr;
        DerivativePtr deri;
        NullCheckPtr containNull;
        using StrSizeType = string::size_type;

        StrSizeType searchHelper(RegexPtr L, const string& str, StrSizeType id)
        {
            StrSizeType result = containNull->check(L) ? id : 0;
            if (id >= str.size()) return result;

            StrSizeType tmp = searchHelper(deri->drv(L, str[id]), str, id + 1);
            return result < tmp ? tmp : result;
        }

        bool matchHelper(RegexPtr L, const string& str, StrSizeType id)
        {
            if (id >= str.size()) return containNull->check(L);
            return matchHelper(deri->drv(L, str[id]), str, id + 1);
        }

    }; // class Engine

} // namespace RE

#endif // REGEX_ENGINE_H