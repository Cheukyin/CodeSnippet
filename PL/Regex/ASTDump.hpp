#ifndef REGEX_ASTDump_H
#define REGEX_ASTDump_H

#include "Visitor.hpp"



#define DynASTDumpCast(ptr) (std::dynamic_pointer_cast<ASTDump>(ptr))

namespace RE
{
    class ASTDump: public Visitor, public std::enable_shared_from_this<ASTDump>
    {
    public:
        string tree;

        void visit(EmptyPtr& re) override
        { tree += "(Empty)"; }

        void visit(NullPtr& re) override
        { tree += "(Null)"; }

        void visit(CharPtr& re) override
        { tree += ("(Char " + string(1, re->ch) + ")"); }

        void visit(AltPtr& re) override
        {
            tree += "(Alt ";
            re->left->accept( shared_from_this() );
            tree += " ";
            re->right->accept( shared_from_this() );
            tree += ")";
        }

        void visit(SeqPtr& re) override
        {
            tree += "(Seq ";
            re->first->accept( shared_from_this() );
            tree += " ";
            re->last->accept( shared_from_this() );
            tree += ")";
        }

        void visit(RepPtr& re) override
        {
            tree += "(Rep ";
            re->re->accept( shared_from_this() );
            tree += ")";
        }
    };

    using ASTDumpPtr = std::shared_ptr<ASTDump>;

} // namespace RE

#endif // REGEX_ASTDump_H