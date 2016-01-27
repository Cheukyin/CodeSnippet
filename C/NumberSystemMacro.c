#include <stdio.h>

#define CY_STR(s)   CY_STR_I(s)
#define CY_STR_I(s) #s

//if c!=0 ,return x;
//if c=0,  return y
#define CY_IF_C_X_Y(c, x, y) CY_IF_C(CY_BOOL(c), x, y)
#define CY_IF_C(c, x, y)     CY_IF_CC(c, x, y)
#define CY_IF_CC(c, x, y)    CY_IF##c(x, y)
#define CY_IF0(x, y)         y
#define CY_IF1(x, y)         x


#define CY_ADD(m, n)     CY_ADD_C(CY_INC(m), CY_DEC(n))
#define CY_ADD_C(m, n)   CY_ADD_CC(m, n)
#define CY_ADD_CC(m, n)  CY_ADD##n(m, n)
#define CY_ADD0(m, n)    CY_ADD0_C(m, 0)
#define CY_ADD0_C(m, n)  m
#define CY_ADD1(m, n)    CY_ADD1_C(CY_INC(m), 0)
#define CY_ADD1_C(m, n)  CY_ADD##n(m, n)
#define CY_ADD2(m, n)    CY_ADD2_C(CY_INC(m), 1)
#define CY_ADD2_C(m, n)  CY_ADD##n(m, n)
#define CY_ADD3(m, n)    CY_ADD3_C(CY_INC(m), 2)
#define CY_ADD3_C(m, n)  CY_ADD##n(m, n)
#define CY_ADD4(m, n)    CY_ADD4_C(CY_INC(m), 3)
#define CY_ADD4_C(m, n)  CY_ADD##n(m, n)

 //if c!=0, return 1, else 0
 #define CY_BOOL(c) CY_BOOL##c
 #define CY_BOOL0   0
 #define CY_BOOL1   1
 #define CY_BOOL2   1
 #define CY_BOOL3   1
 #define CY_BOOL4   1
 #define CY_BOOL5   1
 #define CY_BOOL6   1
 #define CY_BOOL7   1

#define CY_INC(n) CY_INC##n
#define CY_INC0   1
#define CY_INC1   2
#define CY_INC2   3
#define CY_INC3   4
#define CY_INC4   5
#define CY_INC5   6
#define CY_INC6   7

#define CY_DEC(n) CY_DEC##n
#define CY_DEC0   0
#define CY_DEC1   0
#define CY_DEC2   1
#define CY_DEC3   2
#define CY_DEC4   3
#define CY_DEC5   4
#define CY_DEC6   5

 int main()
 {
     printf("%d\n", CY_ADD(3,2));
     return 0;
}
