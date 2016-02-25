#ifndef REGEX_VISITOR_H
#define REGEX_VISITOR_H

#include "AST.hpp"

namespace RE
{
    using VisitorPtr = std::shared_ptr<Visitor>;

    struct Visitor
    {
        virtual void visit(const RegexPtr& re) = 0;
        virtual void visit(const EmptyPtr& re) = 0;
        virtual void visit(const NullPtr&  re) = 0;
        virtual void visit(const CharPtr&  re) = 0;
        virtual void visit(const AltPtr&   re) = 0;
        virtual void visit(const SeqPtr&   re) = 0;
        virtual void visit(const RepPtr&   re) = 0;
        virtual void visit(const GroupPtr& re) = 0;
    };

} // namespace RE

#endif // REGEX_VISITOR_H