#ifndef REGEX_ENGINE_H
#define REGEX_ENGINE_H

#include "Parser.hpp"
#include "Derivative.hpp"

namespace RE
{
    class Engine
    {
    public:
        Engine(const string& str): deri(new (Derivative)), containNull(new NullCheck)
        { rePtr = parse(str); }

        string search(const string& str)
        { 
            for (StrType i = 0; i < str.size(); i++)
            {
                StrType tail = searchHelper(rePtr, str, i);
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
        using StrType = string::size_type;

        StrType searchHelper(RegexPtr L, const string& str, StrType id)
        {
            StrType result = containNull->check(L) ? id : 0;
            if (id >= str.size()) return result;

            StrType tmp = searchHelper(deri->drv(L, str[id]), str, id + 1);
            return result < tmp ? tmp : result;
        }

        bool matchHelper(RegexPtr L, const string& str, StrType id)
        {
            if (id >= str.size()) return containNull->check(L);
            return matchHelper(deri->drv(L, str[id]), str, id + 1);
        }

    }; // class Engine

} // namespace RE

#endif // REGEX_ENGINE_H