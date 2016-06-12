#include "Utils.hpp"
#include "UnitTest.hpp"

// TEST_CASE(TypeDisplayer)
// {
//     auto x = {3};
//     CYTL::TypeDisplayer<decltype(x)> xType;
// }

// TEST_CASE(NonCopyable)
// {
//     struct T: public CYTL::NonCopyable {};

//     T a;
//     T b(a); // compiling error

//     T c;
//     c = a; // compiling error
// }

TEST_CASE(arraySize)
{
    int keyVals[] = {1, 2, 3, 4, 5, 6};
    static_assert(CYTL::arraySize(keyVals) == 6, "arraySize error");
}

TEST_CASE(IfThenElse)
{
    struct T; struct F;

    CYTL::StaticTypeCheckEQ<CYTL::IfThenElse<true, T, F>, T>();
    CYTL::StaticTypeCheckEQ<CYTL::IfThenElse<false, T, F>, F>();
}

TEST_CASE(EnableIf)
{
    struct T;
    CYTL::StaticTypeCheckEQ<CYTL::EnableIf<true, T>, T>();
}

TEST_CASE(RemoveQualifiers)
{
    // RemoveConst
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConst<const volatile int>, volatile int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConst<int>, int>();

    // RemoveVolatile
    CYTL::StaticTypeCheckEQ<CYTL::RemoveVolatile<const volatile int>, const int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveVolatile<int&>, int&>();

    // RemoveConstVolatile
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConstVolatile<const volatile int>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveConstVolatile<int>, int>();

    // RemoveReference
    CYTL::StaticTypeCheckEQ<CYTL::RemoveReference<int&>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveReference<int&&>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::RemoveReference<int>, int>();
}

TEST_CASE(UnderlyingType)
{
    enum class E :int{ A=1, B=2, C=3 };
    CYTL::StaticTypeCheckEQ<CYTL::UnderlyingType<E>, int>();

    E e(E::B);
    EXPECT_EQ(CYTL::toUnderlyingType(e), 2);

    enum E1{ A=1, B=2, C=3 };
    CYTL::StaticTypeCheckEQ<CYTL::UnderlyingType<E1>, int>();

    E1 e1(C);
    EXPECT_EQ(CYTL::toUnderlyingType(e1), 3);
}

TEST_CASE(ConstIterator)
{
    int arr[] = {1, 2, 3, 4};

    EXPECT_EQ(CYTL::cbegin(arr), arr);
    EXPECT_EQ(CYTL::cend(arr), arr + CYTL::arraySize(arr));

    CYTL::StaticTypeCheckEQ<decltype( CYTL::cbegin(arr) ), const int *>();
    CYTL::StaticTypeCheckEQ<decltype( CYTL::cend(arr) ), const int *>();
}

TEST_CASE(TypeConvertibility)
{
    // IsTypeSame
    static_assert(CYTL::IsTypeSame<int, int>::value == true, "");
    static_assert(CYTL::IsTypeSame<double, int>::value == false, "");

    // IsTypeConvertible
    static_assert(CYTL::IsTypeConvertible<int, int>::value == true, "");
    static_assert(CYTL::IsTypeConvertible<double, int>::value == true, "");

    struct P{};
    struct C: P{};

    static_assert(CYTL::IsTypeConvertible<P, C>::value == false, "");
    static_assert(CYTL::IsTypeConvertible<C, P>::value == true, "");

    // IsSuperClass
    static_assert(CYTL::IsSuperClass<C, C>::value == true, "");
    static_assert(CYTL::IsSuperClass<P, C>::value == true, "");
    static_assert(CYTL::IsSuperClass<C, P>::value == false, "");

    // IsStrictSuperClass
    static_assert(CYTL::IsStrictSuperClass<C, C>::value == false, "");
    static_assert(CYTL::IsStrictSuperClass<P, C>::value == true, "");
    static_assert(CYTL::IsStrictSuperClass<C, P>::value == false, "");
}

TEST_CASE(TypeList)
{
    using L1 = CYTL::TypeList<>;
    using L2 = CYTL::TypeList<int>;
    using L3 = CYTL::TypeList<int, char, double>;
    using L4 = CYTL::TypeList<int, char, double, int>;
    using L5 = CYTL::TypeList<int, char, int, double, int, char, double>;
    using L6 = CYTL::TypeList<float, char, double, float>;

    // TypeLength
    static_assert(CYTL::TypeLength<L1>::value == 0, "");
    static_assert(CYTL::TypeLength<L2>::value == 1, "");
    static_assert(CYTL::TypeLength<L3>::value == 3, "");

    // TypeAt
    CYTL::StaticTypeCheckEQ<CYTL::TypeAt<L1, 5>, CYTL::NullType>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeAt<L2, 0>, int>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeAt<L3, 2>, double>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeAt<L3, 4>, CYTL::NullType>();

    // IndexOf
    static_assert(CYTL::IndexOf<L1, int>::value == -1, "");
    static_assert(CYTL::IndexOf<L2, int>::value == 0, "");
    static_assert(CYTL::IndexOf<L2, char>::value == -1, "");
    static_assert(CYTL::IndexOf<L3, char>::value == 1, "");
    static_assert(CYTL::IndexOf<L3, double>::value == 2, "");

    // TypeAppend
    CYTL::StaticTypeCheckEQ<CYTL::TypeAppend<L3, int>, L4>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeAppend<L3, CYTL::NullType>, L3>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeAppend<L3, L1>, L3>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeAppend<L3, L2>, L4>();

    // TypeErase
    CYTL::StaticTypeCheckEQ<CYTL::TypeErase<L1, int>, L1>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeErase<L2, int>, L1>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeErase<L2, char>, L2>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeErase<L3, char>,
                            CYTL::TypeList<int, double> >();

    // TypeEraseAll
    CYTL::StaticTypeCheckEQ<CYTL::TypeEraseAll<L4, int>,
                            CYTL::TypeList<char, double> >();

    // TypeEraseDuplicates
    CYTL::StaticTypeCheckEQ<CYTL::TypeEraseDuplicates<L4>, L3>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeEraseDuplicates<L5>, L3>();

    // TypeReplace
    CYTL::StaticTypeCheckEQ<CYTL::TypeReplace<L1, int, char>, L1>();
    CYTL::StaticTypeCheckEQ<CYTL::TypeReplace<L3, char, float>,
                            CYTL::TypeList<int, float, double> >();

    // TypeReplaceAll
    CYTL::StaticTypeCheckEQ<CYTL::TypeReplaceAll<L4, int, float>, L6>();

    struct Widget{};
    struct ScrollBar: Widget{};
    struct Button: Widget{};
    struct GraphicButton: Button{};

    using ButtonL = CYTL::TypeList<Button, Widget, GraphicButton>;
    using SortedButtonL = CYTL::TypeList<GraphicButton, Button, Widget>;

    using GUIL = CYTL::TypeList<Widget, GraphicButton, ScrollBar, Button>;
    using SortedGUIL = CYTL::TypeList<GraphicButton, Button, ScrollBar, Widget>;

    // MostDerivedType
    CYTL::StaticTypeCheckEQ<CYTL::MostDerivedType<ButtonL>, GraphicButton>();

    // TypePartialOrder
    CYTL::StaticTypeCheckEQ<CYTL::TypePartialOrder<ButtonL>, SortedButtonL>();
    CYTL::StaticTypeCheckEQ<CYTL::TypePartialOrder<GUIL>, SortedGUIL>();
}