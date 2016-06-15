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

    template<class T>
    using RemoveConst = typename _RemoveConst<T>::type;

    template<class T> struct _RemoveVolatile { typedef T type; };
    template<class T> struct _RemoveVolatile<volatile T> { typedef T type; };

    template<class T>
    using RemoveVolatile = typename _RemoveVolatile<T>::type;

    template<class T>
    using RemoveConstVolatile = RemoveVolatile< RemoveConst<T> >;

    template<class T> struct _RemoveReference { typedef T type; };
    template<class T> struct _RemoveReference<T&> { typedef T type; };
    template<class T> struct _RemoveReference<T&&> { typedef T type; };

    template<class T>
    using RemoveReference = typename _RemoveReference<T>::type;


    // ---------------------------------------
    // underlying type
    template<class E>
    struct _UnderlyingType
    {
        using type = IfThenElse<E(-1) < E(0),
                                typename std::make_signed<E>::type,
                                typename std::make_unsigned<E>::type >;
    };
    template<class E>
    using UnderlyingType = typename _UnderlyingType<E>::type;

    // Enum Class to UnderType
    template<class E>
    constexpr inline UnderlyingType<E> toUnderlyingType(E e) noexcept
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
    struct IsTypeConvertible
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
    struct IsTypeConvertible<T, T>
    { static const bool value{true}; };

    template<class Parent, class Child>
    struct IsSuperClass
    {
        static const bool value = IsTypeConvertible<const Child*, const Parent*>::value
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


    // --------------------------------------
    // Type2Type: for partial specialization of template function
    template<class T> struct Type2Type {};


    // ---------------------------------------
    // TypeList
    template<class... T> struct TypeList;

    // TypeCar
    template<class L> struct _TypeCar;
    template<class H, class... T> struct _TypeCar< TypeList<H, T...> > { using type = H; };
    template<> struct _TypeCar< TypeList<> > { using type = NullType; };

    template<class L>
    using TypeCar = typename _TypeCar<L>::type;

    // TypeCdr
    template<class L> struct _TypeCdr;
    template<class H, class... T> struct _TypeCdr< TypeList<H, T...> > { using type = TypeList<T...>; };
    template<> struct _TypeCdr< TypeList<> > { using type = NullType; };

    template<class L>
    using TypeCdr = typename _TypeCdr<L>::type;

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

    template<class L, int N>
    using TypeAt = typename _TypeAt<L, N>::type;

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
    template<class... T> struct _TypeAppend<TypeList<T...>, NullType> { using type = TypeList<T...>; };
    template<class... Tail, class T>
    struct _TypeAppend<TypeList<Tail...>, T>
    { using type = TypeList<Tail..., T>; };
    template<class... Tail1, class... Tail2>
    struct _TypeAppend< TypeList<Tail1...>, TypeList<Tail2...> >
    { using type = TypeList<Tail1..., Tail2...>; };

    template<class L, class T>
    using TypeAppend = typename _TypeAppend<L, T>::type;

    // Reverse
    template<class L> struct _TypeReverse;
    template<class H> struct _TypeReverse< TypeList<H> > { using type = TypeList<H>; };
    template<> struct _TypeReverse< TypeList<> > { using type = TypeList<>; };
    template<class H, class... T>
    struct _TypeReverse< TypeList<H, T...> >
    { using type = TypeAppend< typename _TypeReverse< TypeList<T...> >::type, TypeList<H> >; };

    template<class L>
    using TypeReverse = typename _TypeReverse<L>::type;

    // TypeErase
    template<class L, class T> struct _TypeErase;
    template<class T> struct _TypeErase<TypeList<>, T>{ using type = TypeList<>; };
    template<class T, class... Tail>
    struct _TypeErase<TypeList<T, Tail...>, T>
    { using type = TypeList<Tail...>; };
    template<class Head, class... Tail, class T>
    struct _TypeErase<TypeList<Head, Tail...>, T>
    { using type = TypeAppend<TypeList<Head>, typename _TypeErase<TypeList<Tail...>, T>::type>; };

    template<class L, class T>
    using TypeErase = typename _TypeErase<L, T>::type;

    // TypeEraseAll
    template<class L, class T> struct _TypeEraseAll;
    template<class T> struct _TypeEraseAll<TypeList<>, T>{ using type = TypeList<>; };
    template<class T, class... Tail>
    struct _TypeEraseAll<TypeList<T, Tail...>, T>
    { using type = typename _TypeEraseAll<TypeList<Tail...>, T>::type; };
    template<class Head, class... Tail, class T>
    struct _TypeEraseAll<TypeList<Head, Tail...>, T>
    { using type = TypeAppend<TypeList<Head>, typename _TypeEraseAll<TypeList<Tail...>, T>::type>; };

    template<class L, class T>
    using TypeEraseAll = typename _TypeEraseAll<L, T>::type;

    // TypeEraseDuplicates
    template<class L> struct _TypeEraseDuplicates;
    template<> struct _TypeEraseDuplicates< TypeList<> >{ using type = TypeList<>; };
    template<class Head, class... Tail>
    struct _TypeEraseDuplicates< TypeList<Head, Tail...> >
    {
    private:
        using EraseAllHead = TypeEraseAll<TypeList<Tail...>, Head>;
        using TailNoDuplicates = typename _TypeEraseDuplicates<EraseAllHead>::type;
    public:
        using type = TypeAppend<TypeList<Head>, TailNoDuplicates>;
    };

    template<class L>
    using TypeEraseDuplicates = typename _TypeEraseDuplicates<L>::type;

    // TypeReplace
    template<class L, class T, class U> struct _TypeReplace;
    template<class T, class U> struct _TypeReplace<TypeList<>, T, U>{ using type = TypeList<>; };
    template<class T, class... Tail, class U>
    struct _TypeReplace<TypeList<T, Tail...>, T, U>
    { using type = TypeList<U, Tail...>; };
    template<class Head, class... Tail, class T, class U>
    struct _TypeReplace<TypeList<Head, Tail...>, T, U>
    { using type = TypeAppend<TypeList<Head>, typename _TypeReplace<TypeList<Tail...>, T, U>::type>; };

    template<class L, class T, class U>
    using TypeReplace = typename _TypeReplace<L, T, U>::type;

    // TypeReplaceAll
    template<class L, class T, class U> struct _TypeReplaceAll;
    template<class T, class U> struct _TypeReplaceAll<TypeList<>, T, U>{ using type = TypeList<>; };
    template<class T, class... Tail, class U>
    struct _TypeReplaceAll<TypeList<T, Tail...>, T, U>
    { using type = TypeAppend<TypeList<U>, typename _TypeReplaceAll<TypeList<Tail...>, T, U>::type>; };
    template<class Head, class... Tail, class T, class U>
    struct _TypeReplaceAll<TypeList<Head, Tail...>, T, U>
    { using type = TypeAppend<TypeList<Head>, typename _TypeReplaceAll<TypeList<Tail...>, T, U>::type>; };

    template<class L, class T, class U>
    using TypeReplaceAll = typename _TypeReplaceAll<L, T, U>::type;

    // ---------------------------------------
    // TypeOdering
    // MostDerivedType
    template<class L, class T> struct _MostDerivedTypeHelper;
    template<class T> struct _MostDerivedTypeHelper<TypeList<>, T> { using type = T; };
    template<class Head, class... Tail, class T>
    struct _MostDerivedTypeHelper<TypeList<Head, Tail...>, T>
    {
    private:
        using Candidate = typename _MostDerivedTypeHelper<TypeList<Tail...>, T>::type;
    public:
        using type = IfThenElse<IsSuperClass<Candidate, Head>::value, Head, Candidate>;
    };

    template<class L> struct _MostDerivedType;
    template<> struct _MostDerivedType< TypeList<> > { using type = NullType; };
    template<class Head, class... Tail>
    struct _MostDerivedType< TypeList<Head, Tail...> >
    { using type = typename _MostDerivedTypeHelper<TypeList<Head, Tail...>, Head>::type; };

    template<class L>
    using MostDerivedType = typename _MostDerivedType<L>::type;

    // TypePartialOrder
    template<class L> struct _TypePartialOrder;
    template<> struct _TypePartialOrder< TypeList<> > { using type = NullType; };
    template<class Head, class... Tail>
    struct _TypePartialOrder< TypeList<Head, Tail...> >
    {
    private:
        using TheMostDerived = MostDerivedType< TypeList<Head, Tail...> >;
        using RemainType = TypeEraseAll<TypeList<Head, Tail...>, TheMostDerived>;
    public:
        using type = TypeAppend<TypeList<TheMostDerived>, typename _TypePartialOrder<RemainType>::type>;
    };

    template<class L>
    using TypePartialOrder = typename _TypePartialOrder<L>::type;


    // --------------------------------------
    // GenScatterHierarchy
    template<class L, template<class> class Holder, int N=0> struct GenScatterHierarchy;
    template<template<class> class Holder, int N> struct GenScatterHierarchy<TypeList<>, Holder, N> {};
    template<class AtomicType, template<class> class Holder, int N>
    struct GenScatterHierarchy: public Holder<AtomicType>
    { using Base = Holder<AtomicType>; };
    template<class Head, class... Tail, template<class> class Holder, int N>
    struct GenScatterHierarchy<TypeList<Head, Tail...>, Holder, N>
        : public GenScatterHierarchy<Head, Holder, N+1>
        , public GenScatterHierarchy<TypeList<Tail...>, Holder, N+2>
    {
        using LeftBase = GenScatterHierarchy<Head, Holder, N+1>;
        using RightBase = GenScatterHierarchy<TypeList<Tail...>, Holder, N+2>;
    };

    // _FieldHelper
    template<class Hierarchy, int N> struct _FieldHelper;
    template<template<class> class Holder, int M, int N>
    struct _FieldHelper<GenScatterHierarchy<TypeList<>, Holder, M>, N>
    { using type = NullType; };
    template<class Head, class... Tail, template<class> class Holder, int M>
    struct _FieldHelper<GenScatterHierarchy<TypeList<Head, Tail...>, Holder, M>, 0>
    { using type = GenScatterHierarchy<Head, Holder, M+1>; };
    template<class Head, class... Tail, template<class> class Holder, int M, int N>
    struct _FieldHelper<GenScatterHierarchy<TypeList<Head, Tail...>, Holder, M>, N>
    { using type = typename _FieldHelper<GenScatterHierarchy<TypeList<Tail...>, Holder, M+2>, N-1>::type; };

    // Field
    template<int N, class... T, template<class> class Holder>
    constexpr inline typename _FieldHelper<GenScatterHierarchy<TypeList<T...>, Holder, 0>, N>::type::Base&
    Field(GenScatterHierarchy<TypeList<T...>, Holder, 0>& obj) noexcept
    {
        using BT = typename _FieldHelper<GenScatterHierarchy<TypeList<T...>, Holder, 0>, N>::type;
        return static_cast<typename BT::Base&>( static_cast<BT&>(obj) );
    }


    // ---------------------------------------
    // Tuple
    namespace{ template<class T> struct _TupleHolder { using type = T; T _value; }; }

    template<class... T>
    using Tuple = GenScatterHierarchy<TypeList<T...>, _TupleHolder>;

    // Field
    template<int N, class... T>
    constexpr inline typename _FieldHelper<Tuple<T...>, N>::type::Base::type&
    Field(Tuple<T...>& obj) noexcept
    {
        using BT = typename _FieldHelper<Tuple<T...>, N>::type;
        return static_cast<typename BT::Base&>( static_cast<BT&>(obj) )._value;
    }


    // ---------------------------------------
    // GenLinearHierarchy
    template<class L, template<class, class> class Holder, class Root = EmptyType>
    struct GenLinearHierarchy;
    template<template<class, class> class Holder, class Root>
    struct GenLinearHierarchy<TypeList<>, Holder, Root> {};
    template<class H, template<class, class> class Holder, class Root>
    struct GenLinearHierarchy<TypeList<H>, Holder, Root>
        : public Holder<H, Root>
    { using Base = Holder<H, Root>; };
    template<class H, class... T, template<class, class> class Holder, class Root>
    struct GenLinearHierarchy<TypeList<H, T...>, Holder, Root>
        : public Holder< H, GenLinearHierarchy<TypeList<T...>, Holder, Root> >
    { using Base = Holder< H, GenLinearHierarchy<TypeList<T...>, Holder, Root> >; };

    // _FieldHelper
    template<class H, class... T, template<class, class> class Holder, class Root, int N>
    struct _FieldHelper<GenLinearHierarchy<TypeList<H, T...>, Holder, Root>, N>
    { using type = typename _FieldHelper<GenLinearHierarchy<TypeList<T...>, Holder, Root>, N-1>::type; };
    template<class H, class... T, template<class, class> class Holder, class Root>
    struct _FieldHelper<GenLinearHierarchy<TypeList<H, T...>, Holder, Root>, 0>
    { using type = typename GenLinearHierarchy<TypeList<H, T...>, Holder, Root>::Base; };
    template<template<class, class> class Holder, class Root, int N>
    struct _FieldHelper<GenLinearHierarchy<TypeList<>, Holder, Root>, N>
    { using type = NullType; };

    // Field
    template<int N, class... T, template<class, class> class Holder, class Root>
    constexpr inline typename _FieldHelper<GenLinearHierarchy<TypeList<T...>, Holder, Root>, N>::type&
    Field(GenLinearHierarchy<TypeList<T...>, Holder, Root>& obj) noexcept
    {
        using RT = typename _FieldHelper<GenLinearHierarchy<TypeList<T...>, Holder, Root>, N>::type;
        return static_cast<RT&>(obj);
    }

} // namespace CYTL

#endif // __CYTL_UTILS__