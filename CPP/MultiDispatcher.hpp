#ifndef __CYTL_MULTIDISPATCHER__
#define __CYTL_MULTIDISPATCHER__

#include "Utils.hpp"

namespace CYTL
{
    // ---------------------------------------
    // StaticMultiDispatcher

    template<class Executor, bool isSymmetric,
             class BaseLhs, class TypesLhs,
             class BaseRhs, class TypesRhs,
             class OriginTypeList> // Only used when isSymmetric == True && TypesLhs == TypesRhs
    struct _StaticMultiDispatcher
    {
        using Head = TypeCar<TypesLhs>;
        using Tail = TypeCdr<TypesLhs>;

        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        {
            if(Head* p = dynamic_cast<Head*>(&lhs))
                return _StaticMultiDispatcher<Executor, isSymmetric,
                                              BaseLhs, NullType,
                                              BaseRhs, TypesRhs,
                                              OriginTypeList
                                              >::dispatchRhs(*p, rhs, exec);
            else return _StaticMultiDispatcher<Executor, isSymmetric,
                                               BaseLhs, Tail,
                                               BaseRhs, TypesRhs,
                                               OriginTypeList
                                               >::dispatch(lhs, rhs, exec);

        }
    };

    template<class Executor, bool isSymmetric,
             class BaseLhs,
             class BaseRhs, class TypesRhs,
             class OriginTypeList> // Only used when isSymmetric == True && TypesLhs == TypesRhs
    struct _StaticMultiDispatcher<Executor, isSymmetric,
                                  BaseLhs, NullType,
                                  BaseRhs, TypesRhs,
                                  OriginTypeList>
    {
        using Head = TypeCar<TypesRhs>;
        using Tail = TypeCdr<TypesRhs>;

        template<bool swapArgs, class ConcreteLhs, class ConcreteRhs>
        struct InvocationTraits
        {
            static typename Executor::ReturnType dispatch(ConcreteLhs& lhs, ConcreteRhs& rhs, Executor& exec)
            { return exec.exec(lhs, rhs); }
        };

        template<class ConcreteLhs, class ConcreteRhs>
        struct InvocationTraits<true, ConcreteLhs, ConcreteRhs>
        {
            static typename Executor::ReturnType dispatch(ConcreteLhs& lhs, ConcreteRhs& rhs, Executor& exec)
            { return exec.exec(rhs, lhs); }
        };

        template<class ConcreteLhs>
        static typename Executor::ReturnType dispatchRhs(ConcreteLhs& lhs, BaseRhs& rhs, Executor exec)
        {
            if(Head* p = dynamic_cast<Head*>(&rhs))
            {
                static const bool swapArgs = isSymmetric
                                          && (IndexOf<Head, OriginTypeList>::value
                                             < IndexOf<ConcreteLhs, OriginTypeList>::value);
                return InvocationTraits<swapArgs, ConcreteLhs, Head>::dispatch(lhs, *p, exec);
            }
            else return _StaticMultiDispatcher<Executor, isSymmetric,
                                               BaseLhs, NullType,
                                               BaseRhs, Tail,
                                               OriginTypeList
                                               >::dispatchRhs(lhs, rhs, exec);
        }

        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor, bool isSymmetric,
             class BaseLhs, class TypesLhs,
             class BaseRhs,
             class OriginTypeList> // Only used when isSymmetric == True && TypesLhs == TypesRhs
    struct _StaticMultiDispatcher<Executor, isSymmetric,
                                  BaseLhs, TypesLhs,
                                  BaseRhs, NullType,
                                  OriginTypeList>
    {
        static typename Executor::ReturnType dispatchRhs(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor, bool isSymmetric,
             class BaseLhs,
             class BaseRhs,
             class OriginTypeList> // Only used when isSymmetric == True && TypesLhs == TypesRhs
    struct _StaticMultiDispatcher<Executor, isSymmetric,
                                  BaseLhs, NullType,
                                  BaseRhs, NullType,
                                  OriginTypeList>
    {
        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }

        static typename Executor::ReturnType dispatchRhs(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor,
             class BaseLhs, class TypesLhs,
             class BaseRhs = BaseLhs, class TypesRhs = TypesLhs>
    using StaticMultiDispatcher = _StaticMultiDispatcher<Executor, false,
                                                         BaseLhs, TypePartialOrder<TypesLhs>,
                                                         BaseRhs, TypePartialOrder<TypesRhs>,
                                                         TypePartialOrder<TypesLhs> >;

    template<class Executor,
             class Base, class Types>
    using StaticSymmetricDispatcher = _StaticMultiDispatcher<Executor, true,
                                                             Base, TypePartialOrder<Types>,
                                                             Base, TypePartialOrder<Types>,
                                                             TypePartialOrder<Types> >;

} // namespace CYTL

#endif // __CYTL_MULTIDISPATCHER