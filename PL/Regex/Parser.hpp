#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include "AST.hpp"

namespace RE
{
    using std::shared_ptr;
    using std::string;

    // only works in Release!!!!!!!!!!!!!!!!!
    class Parser
    {
        // <Regex>   ::=  <Seq> ( '|' <Regex> )?
        //                ""
        // <Seq>     ::=  <Factor> <Seq>?
        // <Factor>  ::=  ( <CapturedRe> | <CharSet> | <Char> ) <Op>*

        // <CapturedRe> ::=  '(' '?:'? <Regex> ')'
        // <CharSet> ::=  '[' <CharNotInclude]>* ']'
        // <Char>    ::=  <any single character>
        // <Op>      ::=  '*' | '+' | '?' | <Range>
        // <Range>   ::=  '{' <Num> ( ',' <Num> )? '}'

        // <Num>     ::=  [0-9]+

    public:
        RegexPtr operator()(const string& str);

    private:
        string::const_iterator iter;
        int group;

        RegexPtr regex();
        RegexPtr seq();
        RegexPtr factor();
        RegexPtr capturedRe();
        RegexPtr charSet();
        std::pair<int, int> range();
        int num();
    };

} // namespace RE

#endif // REGEX_PARSER_H