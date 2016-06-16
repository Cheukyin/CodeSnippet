#ifndef __CYTL_VISITOR__
#define __CYTL_VISITOR__

#include "Utils.hpp"

namespace CYTL
{
    // ---------------------------------------
    // Acyclic Visitor

    struct AcyclicBaseVisitor
    { virtual ~AcyclicBaseVisitor() {} };

    template<class VisitableType, class ReturnType = void>
    struct AcyclicVisitor
    { virtual ReturnType Visit(VisitableType&) = 0; };

    template<class ReturnType = void>
    struct BaseVisitable
    {
        virtual ~BaseVisitable() {}
        virtual ReturnType Accept(AcyclicBaseVisitor&) = 0;

    protected:
        template<class VisitableType>
        static ReturnType AcceptImpl(VisitableType& visited, AcyclicBaseVisitor& guest)
        {
            using VisitorType = AcyclicVisitor<VisitableType, ReturnType>;
            if(VisitorType* p = dynamic_cast<VisitorType*>(&guest))
                return p->Visit(visited);
            return ReturnType();
        }
    };

    #define DefineAcyclicAccept() \
        virtual auto Accept(CYTL::AcyclicBaseVisitor& guest) \
            -> decltype(AcceptImpl(*this, guest)) \
        { return AcceptImpl(*this, guest); }


    // ---------------------------------------
    // Cyclic Visitor

} // namespace CYTL

#endif // __CYTL_VISITOR__