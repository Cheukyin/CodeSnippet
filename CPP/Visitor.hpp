#ifndef __CYTL_VISITOR__
#define __CYTL_VISITOR__

#include "Utils.hpp"

namespace CYTL
{
    // ---------------------------------------
    // Acyclic Visitor

    // AcyclicBaseVisitor
    struct AcyclicBaseVisitor
    { virtual ~AcyclicBaseVisitor() {} };

    // AcyclicVisitor
    template<class VisitableType, class ReturnType = void>
    struct AcyclicVisitor
    { virtual ReturnType visit(VisitableType&) = 0; };

    // BaseVisitable
    template<class ReturnType = void>
    struct BaseVisitable
    {
        virtual ~BaseVisitable() {}
        virtual ReturnType accept(AcyclicBaseVisitor&) = 0;

    protected:
        template<class VisitableType>
        static ReturnType acceptImpl(VisitableType& visited, AcyclicBaseVisitor& visitor)
        {
            using VisitorType = AcyclicVisitor<VisitableType, ReturnType>;
            if(VisitorType* p = dynamic_cast<VisitorType*>(&visitor))
                return p->visit(visited);
            return ReturnType();
        }
    };

    #define DefineAcyclicAccept() \
        virtual auto accept(CYTL::AcyclicBaseVisitor& visitor) \
            -> decltype(acceptImpl(*this, visitor)) \
        { return acceptImpl(*this, visitor); }


    // ---------------------------------------
    // Cyclic Visitor

    // CyclicVisitorUnit
    template<class VisitableType, class ReturnType = void>
    struct CyclicVisitorUnit
    {
        virtual ReturnType visit(VisitableType&) = 0;
    	virtual ~CyclicVisitorUnit() {}
    };

    // CyclicVisitor
    template<class L, class ReturnType = void>
    struct CyclicVisitor: GenScatterHierarchy<L, TypeBind2nd<CyclicVisitorUnit, ReturnType>::template Type>
    {
        template<class VisitableType>
        ReturnType visit(VisitableType& visitable)
        { return static_cast<CyclicVisitorUnit<VisitableType, ReturnType>&>(*this).visit(visitable); }
    };

    #define DefineCyclicAccept(CyclicVisitorType) \
        virtual auto accept(CyclicVisitorType& visitor) \
            -> decltype( visitor.visit(*this) ) \
        { return visitor.visit(*this); }

} // namespace CYTL

#endif // __CYTL_VISITOR__