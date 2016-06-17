#ifndef __CYTL_MULTIDISPATCHER__
#define __CYTL_MULTIDISPATCHER__

#include "Utils.hpp"

namespace CYTL
{
    // ---------------------------------------
    // StaticMultiDispatcher

    template<class Executor,
             class BaseLhs, class TypesLhs,
             class BaseRhs, class TypesRhs>
    struct _StaticMultiDispatcher
    {
        using Head = TypeCar<TypesLhs>;
        using Tail = TypeCdr<TypesLhs>;

        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        {
            if(Head* p = dynamic_cast<Head*>(&lhs))
                return _StaticMultiDispatcher<Executor,
                                             BaseLhs, NullType,
                                             BaseRhs, TypesRhs
                                             >::dispatchRhs(*p, rhs, exec);
            else return _StaticMultiDispatcher<Executor,
                                              BaseLhs, Tail,
                                              BaseRhs, TypesRhs
                                              >::dispatch(lhs, rhs, exec);

        }
    };

    template<class Executor,
             class BaseLhs,
             class BaseRhs, class TypesRhs>
    struct _StaticMultiDispatcher<Executor,
                                  BaseLhs, NullType,
                                  BaseRhs, TypesRhs>
    {
        using Head = TypeCar<TypesRhs>;
        using Tail = TypeCdr<TypesRhs>;

        template<class ConcreteLhs>
        static typename Executor::ReturnType dispatchRhs(ConcreteLhs& lhs, BaseRhs& rhs, Executor exec)
        {
            if(Head* p = dynamic_cast<Head*>(&rhs))
                return exec.exec(lhs, *p);
            else return _StaticMultiDispatcher<Executor,
                                               BaseLhs, NullType,
                                               BaseRhs, Tail
                                               >::dispatchRhs(lhs, rhs, exec);
        }

        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor,
             class BaseLhs, class TypesLhs,
             class BaseRhs>
    struct _StaticMultiDispatcher<Executor,
                                  BaseLhs, TypesLhs,
                                  BaseRhs, NullType>
    {
        static typename Executor::ReturnType dispatchRhs(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor,
             class BaseLhs,
             class BaseRhs>
    struct _StaticMultiDispatcher<Executor,
                                  BaseLhs, NullType,
                                  BaseRhs, NullType>
    {
        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }

        static typename Executor::ReturnType dispatchRhs(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor,
             class BaseLhs, class TypesLhs,
             class BaseRhs = BaseLhs, class TypesRhs = TypesLhs>
    using StaticMultiDispatcher = _StaticMultiDispatcher<Executor,
                                                         BaseLhs, TypePartialOrder<TypesLhs>,
                                                         BaseRhs, TypePartialOrder<TypesRhs> >;

} // namespace CYTL

#endif // __CYTL_MULTIDISPATCHER