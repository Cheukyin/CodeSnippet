#ifndef REGEX_AST_H
#define REGEX_AST_H

#include <iostream>
#include <memory>
#include <string>

namespace RE
{
    using std::string;

    struct Visitor;

    using VisitorPtr = std::shared_ptr<Visitor>;

    struct Regex
    { virtual void accept(const VisitorPtr& visitor) = 0; };

    using RegexPtr = std::shared_ptr<Regex>;

    // no string
    struct Empty : public Regex, public std::enable_shared_from_this<Empty>
    { void accept(const VisitorPtr& visitor) override; };

    // "", empty string
    struct Null : public Regex, public std::enable_shared_from_this<Null>
    { void accept(const VisitorPtr& visitor) override; };

    // 'a', a single character
    struct Char : public Regex, public std::enable_shared_from_this<Char>
    {
        char ch;
        void accept(const VisitorPtr& visitor) override;

        Char(char ch) : ch(ch) {}
    };

    // L | R
    struct Alt : public Regex, public std::enable_shared_from_this<Alt>
    {
        RegexPtr left, right;
        void accept(const VisitorPtr& visitor) override;

        Alt(const RegexPtr& l, const RegexPtr& r) : left(l), right(r) {}
    };

    // AB
    struct Seq : public Regex, public std::enable_shared_from_this<Seq>
    {
        RegexPtr first, last;
        void accept(const VisitorPtr& visitor) override;

        Seq(const RegexPtr& f, const RegexPtr& l) : first(f), last(l) {}
    };

    // A*
    struct Rep : public Regex, public std::enable_shared_from_this<Rep>
    {
        RegexPtr re;
        void accept(const VisitorPtr& visitor) override;

        Rep(const RegexPtr& re) :re(re) {}
    };
    
    using EmptyPtr = std::shared_ptr<Empty>;
    using NullPtr  = std::shared_ptr<Null>;
    using CharPtr  = std::shared_ptr<Char>;
    using AltPtr   = std::shared_ptr<Alt>;
    using SeqPtr   = std::shared_ptr<Seq>;
    using RepPtr   = std::shared_ptr<Rep>;

} // namespace RE

#endif // REGEX_AST_H