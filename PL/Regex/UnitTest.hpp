#ifndef REGEX_UNITTEST_H
#define REGEX_UNITTEST_H

#include <iostream>
#include <math.h>


static const double EPSILON = 0.000001;


#define TEST_CASE(Name) \
    extern void test_case_function_##Name(void); \
    class Test_Case_Class_##Name \
    { \
    public: \
        Test_Case_Class_##Name() \
        { \
            std::cout << #Name << std::endl;   \
            test_case_function_##Name(); \
        } \
    } test_case_instance_##Name; \
    void test_case_function_##Name(void)


#define PRINT_ERR_LOC(cond) \
        std::cerr << "   In File \"" << __FILE__ \
                  << "\" Line " << __LINE__ << ": " \
                  << #cond << " failed!\n"


#define ASSERT(cond) \
    do{ \
        if( !(cond) ) \
        { \
            PRINT_ERR_LOC( cond ); \
            exit(1); \
        } \
    } while(0)
#define ASSERT_1(v, cond) \
    do{ \
        if( !(cond) ) \
        { \
            PRINT_ERR_LOC( cond ); \
            std::cout << "      " << #v << " = " << (v) << std::endl;   \
            exit(1); \
        } \
    } while(0)
#define ASSERT_2(v1, v2, cond) \
    do{ \
        if( !(cond) ) \
        { \
            PRINT_ERR_LOC( cond ); \
            std::cout << "      " << #v1 << " = " << (v1) << std::endl; \
            std::cout << "      " << #v2 << " = " << (v2) << std::endl; \
            exit(1); \
        } \
    } while(0)


#define EXPECT(cond) \
    do{ \
        if( !(cond) ) \
        { \
            PRINT_ERR_LOC( cond ); \
        } \
    } while(0)
#define EXPECT_1(v, cond) \
    do{ \
        if( !(cond) ) \
        { \
            PRINT_ERR_LOC( cond ); \
            std::cout << "      " << #v << " = " << (v) << std::endl;   \
        } \
    } while(0)
#define EXPECT_2(v1, v2, cond)                       \
    do{ \
        if( !(cond) ) \
        { \
            PRINT_ERR_LOC( cond ); \
            std::cout << "      " << #v1 << " = " << (v1) << std::endl; \
            std::cout << "      " << #v2 << " = " << (v2) << std::endl; \
        } \
    } while(0)


#define TEST_PRINT(msg) std::cout << (msg) << std::endl


// Fatal Assertion
#define ASSERT_EQ(v1, v2) ASSERT_2( v1, v2, (v1) == (v2) )
#define ASSERT_NEQ(v1, v2) ASSERT_2( v1, v2, (v1) != (v2) )
#define ASSERT_DOUBLE_EQ(v1, v2) ASSERT_2( v1, v2, fabs( (v1) - (v2) ) < EPSILON )
#define ASSERT_DOUBLE_NEQ(v1, v2) ASSERT_2( v1, v2, fabs( (v1) - (v2) ) >= EPSILON )

#define ASSERT_GT(v1, v2) ASSERT_2( v1, v2, (v1) > (v2) )
#define ASSERT_GET(v1, v2) ASSERT_2( v1, v2, (v1) >= (v2) )
#define ASSERT_LT(v1, v2) ASSERT_2( v1, v2, (v1) < (v2) )
#define ASSERT_LET(v1, v2) ASSERT_2( v1, v2, (v1) <= (v2) )

#define ASSERT_TRUE(v) ASSERT_1( v, (v)== true )
#define ASSERT_FALSE(v) ASSERT_1( v, (v) == false )


// Nonfatal Assertion
#define EXPECT_EQ(v1, v2) EXPECT_2( v1, v2, (v1) == (v2) )
#define EXPECT_NEQ(v1, v2) EXPECT_2( v1, v2, (v1) != (v2) )

#define EXPECT_DOUBLE_EQ(v1, v2) EXPECT_2( v1, v2, fabs( (v1) - (v2) ) < EPSILON )
#define EXPECT_DOUBLE_NEQ(v1, v2) EXPECT_2( v1, v2, fabs( (v1) - (v2) ) >= EPSILON )

#define EXPECT_GT(v1, v2) EXPECT_2( v1, v2, (v1) > (v2) )
#define EXPECT_GET(v1, v2) EXPECT_2( v1, v2, (v1) >= (v2) )
#define EXPECT_LT(v1, v2) EXPECT_2( v1, v2, (v1) < (v2) )
#define EXPECT_LET(v1, v2) EXPECT_2( v1, v2, (v1) <= (v2) )

#define EXPECT_TRUE(v) EXPECT_1( v, (v) == true )
#define EXPECT_FALSE(v) EXPECT_1( v, (v) == false )

#endif // REGEX_UNITTEST_H