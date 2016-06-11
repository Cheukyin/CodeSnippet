#include <iterator>
#include <cstddef>
#include <type_traits>

namespace CYTL
{
    // ---------------------------------------
    // Type Displayer
    // usage: TypeDisplayer<decltype(t)> tType;
    //        then see the compiler's error info
    template<class T> class TypeDisplayer;


    // ----------------------------------------
    // NonCopyable: just inherit it
    struct NonCopyable
    {
        NonCopyable() {}
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };


    // ---------------------------------------
    // check if T1 == T2
    template<class T1, class T2> struct StaticTypeCheckEQ;
    template<class T> struct StaticTypeCheckEQ<T, T> {};


    // ---------------------------------------
    // return size of an array as a constant-time constant
    template<class T, std::size_t N>
    constexpr std::size_t arraySize(T (&)[N]) noexcept { return N; }


    // ---------------------------------------
    // if B, T1, else T2
    template<bool B, class T1, class T2> struct _Select;
    template<class T1, class T2> struct _Select<true, T1, T2> { typedef T1 type; };
    template<class T1, class T2> struct _Select<false, T1, T2> { typedef T2 type; };
    template<bool B, class T1, class T2>
    using IfThenElse = typename _Select<B, T1, T2>::type;


    // ------------------------------------
    // EnableIf
    template<bool B, class T> struct _EnableIf;
    template<class T> struct _EnableIf<true, T> { typedef T type; };
    template<bool B, class T>
    using EnableIf = typename _EnableIf<B, T>::type;


    // ---------------------------------------
    // Remove Qualifiers
    template<class T> struct _RemoveConst { typedef T type; };
    template<class T> struct _RemoveConst<const T> { typedef T type; };
    template<class T> using RemoveConst = typename _RemoveConst<T>::type;

    template<class T> struct _RemoveVolatile { typedef T type; };
    template<class T> struct _RemoveVolatile<volatile T> { typedef T type; };
    template<class T> using RemoveVolatile = typename _RemoveVolatile<T>::type;

    template<class T> using RemoveConstVolatile = RemoveVolatile< RemoveConst<T> >;

    template<class T> struct _RemoveReference { typedef T type; };
    template<class T> struct _RemoveReference<T&> { typedef T type; };
    template<class T> struct _RemoveReference<T&&> { typedef T type; };
    template<class T> using RemoveReference = typename _RemoveReference<T>::type;


    // ---------------------------------------
    // underlying type
    template<class E>
    struct _UnderlyingType
    {
        using type = IfThenElse<E(-1) < E(0),
                                typename std::make_signed<E>::type,
                                typename std::make_unsigned<E>::type >;
    };
    template<class E> using UnderlyingType = typename _UnderlyingType<E>::type;

    // Enum Class to UnderType
    template<class E>
    constexpr UnderlyingType<E> toUnderlyingType(E e) noexcept
    { return static_cast< UnderlyingType<E> >(e); }


    // ---------------------------------------
    // global cbegin, cend
    template<class C>
    auto cbegin(const C& container) -> decltype(std::begin(container))
    { return std::begin(container); }

    template<class C>
    auto cend(const C& container) -> decltype(std::end(container))
    { return std::end(container); }

} // namespace CYTL