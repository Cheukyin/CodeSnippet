#ifndef __CYTL_MULTIDISPATCHER__
#define __CYTL_MULTIDISPATCHER__

#include "Utils.hpp"

namespace CYTL
{
    // ---------------------------------------
    // StaticDoubleDispatcher

    template<class Executor,
             class BaseLhs, class TypesLhs,
             class BaseRhs, class TypesRhs>
    struct _StaticDoubleDispatcher
    {
        using Head = TypeCar<TypesLhs>;
        using Tail = TypeCdr<TypesLhs>;

        static typename Executor::ReturnType dispatch(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        {
            if(Head* p = dynamic_cast<Head*>(&lhs))
                return _StaticDoubleDispatcher<Executor,
                                               BaseLhs, NullType,
                                               BaseRhs, TypesRhs
                                               >::dispatchRhs(*p, rhs, exec);
            else return _StaticDoubleDispatcher<Executor,
                                                BaseLhs, Tail,
                                                BaseRhs, TypesRhs
                                                >::dispatch(lhs, rhs, exec);

        }
    };

    template<class Executor,
             class BaseLhs,
             class BaseRhs, class TypesRhs>
    struct _StaticDoubleDispatcher<Executor,
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
            else return _StaticDoubleDispatcher<Executor,
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
    struct _StaticDoubleDispatcher<Executor,
                                   BaseLhs, TypesLhs,
                                   BaseRhs, NullType>
    {
        static typename Executor::ReturnType dispatchRhs(BaseLhs& lhs, BaseRhs& rhs, Executor exec)
        { return exec.onError(lhs, rhs); }
    };

    template<class Executor,
             class BaseLhs,
             class BaseRhs>
    struct _StaticDoubleDispatcher<Executor,
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
    using StaticDoubleDispatcher = _StaticDoubleDispatcher<Executor,
                                                           BaseLhs, TypePartialOrder<TypesLhs>,
                                                           BaseRhs, TypePartialOrder<TypesRhs> >;

} // namespace CYTL

#endif // __CYTL_MULTIDISPATCHER