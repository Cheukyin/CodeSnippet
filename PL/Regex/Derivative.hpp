#ifndef REGEX_DERIVATIVE_H
#define REGEX_DERIVATIVE_H

#include "Visitor.hpp"

namespace RE
{
    struct NullCheck : public Visitor, public std::enable_shared_from_this<NullCheck>
    {
        // NullCheck(<Empty>) = False
        // NullCheck(<Null>) = True
        // NullCheck(<Char c>) = False
        // NullCheck(<Alt L R>) = NullCheck(L) | NullCheck(R)
        // NullCheck(<Seq F L>) = NullCheck(F) & NullCheck(L)
        // NullCheck(<Rep L>) = True

        bool containNull;

        bool check(const RegexPtr& re)
        {
            visit(re);
            return containNull;
        }

        void visit(const RegexPtr& re) override
        { re->accept( shared_from_this() ); }

        void visit(const EmptyPtr& re) override
        { containNull = false; }

        void visit(const NullPtr& re) override
        { containNull = true; }

        void visit(const CharPtr& re) override
        { containNull = false; }

        void visit(const AltPtr& re) override
        {
            re->left->accept( shared_from_this() );
            bool leftContainNull = containNull;
            re->right->accept( shared_from_this() );

            containNull =  containNull || leftContainNull;
        }

        void visit(const SeqPtr& re) override
        {
            re->first->accept( shared_from_this() );
            bool leftContainNull = containNull;
            re->last->accept( shared_from_this() );

            containNull = containNull && leftContainNull;
        }

        void visit(const RepPtr& re) override
        { containNull = true; }

        void visit(const GroupPtr& re) override
        { re->re->accept( shared_from_this() ); }
    };

    using NullCheckPtr = std::shared_ptr<NullCheck>;

    struct Derivative : public Visitor, public std::enable_shared_from_this<Derivative>
    {
        // D(Empty, c) = Empty
        // D(Null, c) = Empty
        // D(c, c) = Null
        // D('.', c) = Null
        // D(c', c) = Empty, if c' != c

        // D(<Alt L R>, c) = <Alt D(L, c) D(R, c)>
        // D(<Seq F L>, c) = if ContainNull(F). <Alt <Seq D(F, c) L> D(L, c)>
        //                   else.              <Seq D(F, c) L>
        // D(<Rep L>) = < Seq D(L, c) <Rep L> >

        RegexPtr r;
        char c;
        NullCheckPtr containNull;

        RegexPtr drv(const RegexPtr& re, char ch)
        {
            containNull = NullCheckPtr(new NullCheck);
            r = re; c = ch;
            visit(re);
            return r;
        }

        void visit(const RegexPtr& re) override
        { re->accept( shared_from_this() ); }

        void visit(const EmptyPtr& re) override
        { r = EmptyPtr(new Empty); }

        void visit(const NullPtr& re) override
        { r = EmptyPtr(new Empty); }

        void visit(const CharPtr& re) override
        {
            if (re->ch == c)        r = NullPtr(new Null);
            else if (re->ch == '.') r = NullPtr(new Null);
            else                    r = EmptyPtr(new Empty);
        }

        void visit(const AltPtr& re) override
        {
            re->left->accept( shared_from_this() );
            RegexPtr l = r;
            re->right->accept( shared_from_this() );
            r = AltPtr( new Alt(l, r) );
        }

        void visit(const SeqPtr& re) override
        {
            re->first->accept( shared_from_this() );
            r = SeqPtr(new Seq(r, re->last));
            RegexPtr f = r;
            if ( containNull->check(re->first) )
            {
                re->last->accept( shared_from_this() );
                r = AltPtr( new Alt(f, r) );
            }
        }

        void visit(const RepPtr& re) override
        {
            re->re->accept( shared_from_this() );
            r = SeqPtr( new Seq(r, re) );
        }

        void visit(const GroupPtr& re) override
        { re->re->accept( shared_from_this() ); }
    };

    using DerivativePtr = std::shared_ptr<Derivative>;

} // namespace RE

#endif // REGEX_DERIVATIVE_H