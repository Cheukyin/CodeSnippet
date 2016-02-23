#ifndef REGEX_VISITOR_H
#define REGEX_VISITOR_H

#include "AST.hpp"

namespace RE
{
    using VisitorPtr = std::shared_ptr<Visitor>;

    struct Visitor
    {
        virtual void visit(EmptyPtr& re) = 0;
        virtual void visit(NullPtr&  re) = 0;
        virtual void visit(CharPtr&  re) = 0;
        virtual void visit(AltPtr&   re) = 0;
        virtual void visit(SeqPtr&   re) = 0;
        virtual void visit(RepPtr&   re) = 0;
    };

} // namespace RE

#endif // REGEX_VISITOR_H