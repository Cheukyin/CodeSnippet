#ifndef __CYTL_UTILS__
#define __CYTL_UTILS__

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


    // ---------------------------------------
    // check type convertibility
    template<class T1, class T2>
    struct IsTypeSame{ static const bool value = false; };
    template<class T>
    struct IsTypeSame<T, T>{ static const bool value = true; };

    template<class From, class To>
    struct IsTypeConvertibile
    {
    private:
        using Small = char;
        struct Big { char dummy[2]; };

        static Small test(To);
        static Big test(...);
        static From makeFrom();

    public:
        static const bool value = sizeof( test(makeFrom()) ) == sizeof(Small);
    };
    template<class T>
    struct IsTypeConvertibile<T, T>
    { static const bool value{true}; };

    template<class Parent, class Child>
    struct IsSuperClass
    {
        static const bool value = IsTypeConvertibile<const Child*, const Parent*>::value
                               && !IsTypeSame<const Parent*, const void*>::value;
    };

    template<class Parent, class Child>
    struct IsStrictSuperClass
    {
        static const bool value = IsSuperClass<Parent, Child>::value
                               && !IsTypeSame<const Parent, const Child>::value;
    };


    // ---------------------------------------
    // NullType
    struct NullType{};
    // EmptyType
    struct EmptyType{};


    // ---------------------------------------
    // TypeList
    template<class... T> struct TypeList;

    // TypeLength
    template<class L> struct TypeLength;
    template<> struct TypeLength< TypeList<> >{ static const int value = 0; };
    template<class Head, class... Tail>
    struct TypeLength< TypeList<Head, Tail...> >
    { static const int value = 1 + TypeLength< TypeList<Tail...> >::value; };

    // TypeAt
    template<class L, int N> struct _TypeAt;
    template<int N> struct _TypeAt<TypeList<>, N>{ using type = NullType; };
    template<class Head, class... Tail, int N>
    struct _TypeAt<TypeList<Head, Tail...>, N>
    { using type = IfThenElse<N==0, Head, typename _TypeAt<TypeList<Tail...>, N-1>::type>; };

    template<class L, int N> using TypeAt = typename _TypeAt<L, N>::type;

    // IndexOf
    template<class L, class T> struct IndexOf{ static const int value = -1; };
    template<class... Tail, class T>
    struct IndexOf<TypeList<T, Tail...>, T>
    { static const int value = 0; };
    template<class Head, class... Tail, class T>
    struct IndexOf<TypeList<Head, Tail...>, T>
    {
    private:
        static const int tmp = IndexOf<TypeList<Tail...>, T>::value;
    public:
        static const int value = tmp == -1 ? -1 : 1+tmp;
    };

    // TypeAppend
    template<class L1, class L2> struct _TypeAppend;

    template<> struct _TypeAppend<NullType, NullType> { using type = NullType; };
    template<class... T> struct _TypeAppend< NullType, TypeList<T...> > { using type = TypeList<T...>; };
    template<class T> struct _TypeAppend<NullType, T> { using type = TypeList<T>; };

    template<class... T> struct _TypeAppend<TypeList<T...>, NullType> { using type = TypeList<T...>; };
    template<class... Tail, class T>
    struct _TypeAppend<TypeList<Tail...>, T>
    { using type = TypeList<Tail..., T>; };
    template<class... Tail1, class... Tail2>
    struct _TypeAppend< TypeList<Tail1...>, TypeList<Tail2...> >
    { using type = TypeList<Tail1..., Tail2...>; };

    template<class L, class T> using TypeAppend = typename _TypeAppend<L, T>::type;

} // namespace CYTL

#endif // __CYTL_UTILS__