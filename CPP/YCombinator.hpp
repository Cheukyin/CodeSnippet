#ifndef __CYTL_YCOMBINATOR__
#define __CYTL_YCOMBINATOR__

#include <functional>

namespace CYTL
{

    // λf.( λx.(f λy. ((x x) y)) λx.(f λy.((x x) y)) )
    // ( (U->R) -> (U->R) ) -> (U->R)

    // ==> λy. ((x x) y): U->R
    // ==> y: U
    // ==> ((x x) y): R
    // ==> (x x): U->R

    template<class R, class U>
    using U2R = R(U);

    template<class R, class U>
    using U2RFunc = std::function< U2R<R, U> >;

    template<class R, class U>
    struct RecursiveFunc
    { std::function<U2RFunc<R, U>(RecursiveFunc)> func; };

    template<class R, class U>
    inline U2RFunc<R, U>
    yCombinator(U2RFunc< U2RFunc<R, U>, U2RFunc<R, U> > f)
    {
        RecursiveFunc<R, U> r;

        // λx.(f λy. ((x x) y))
        r.func = [f](RecursiveFunc<R, U> x) -> U2RFunc<R, U> {
                       // λy. ((x x) y)
            return f( [x](U y) -> R { return x.func(x)(y); } );
        };

        return r.func(r);
    }


} // namespace CYTL

#endif // __CYTL_YCOMBINATOR__