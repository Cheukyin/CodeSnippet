#ifndef REGEX_ASTDump_H
#define REGEX_ASTDump_H

#include "Visitor.hpp"

#define DynASTDumpCast(ptr) (std::dynamic_pointer_cast<ASTDump>(ptr))

namespace RE
{
    struct ASTDump: public Visitor, public std::enable_shared_from_this<ASTDump>
    {
        string tree;

        string dump(const RegexPtr& re)
        {
            tree = "";
            visit(re);
            return tree;
        }

        void visit(const RegexPtr& re) override
        { re->accept( shared_from_this() ); }

        void visit(const EmptyPtr& re) override
        { tree += "(Empty)"; }

        void visit(const NullPtr& re) override
        { tree += "(Null)"; }

        void visit(const CharPtr& re) override
        { tree += ("(Char " + string(1, re->ch) + ")"); }

        void visit(const AltPtr& re) override
        {
            tree += "(Alt ";
            re->left->accept( shared_from_this() );
            tree += " ";
            re->right->accept( shared_from_this() );
            tree += ")";
        }

        void visit(const SeqPtr& re) override
        {
            tree += "(Seq ";
            re->first->accept( shared_from_this() );
            tree += " ";
            re->last->accept( shared_from_this() );
            tree += ")";
        }

        void visit(const RepPtr& re) override
        {
            tree += "(Rep ";
            re->re->accept( shared_from_this() );
            tree += ")";
        }

        void visit(const GroupPtr& re) override
        {
            tree += "(Group ";
            re->re->accept( shared_from_this() );
            tree += ")";
        }
    };

    using ASTDumpPtr = std::shared_ptr<ASTDump>;

} // namespace RE

#endif // REGEX_ASTDump_H