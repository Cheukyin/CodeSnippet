/* 
 * CSE 351 HW1 (Data Lab )
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to this homework by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   Use the dlc compiler to check that your solutions conform
 *   to the coding rules.
 */


#endif
// Rating: 1
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
    // x&y == ~( ~(x&y) ) == ~( ~x | ~y )
    return ~( ~x | ~y );
}
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    // x^y == (~xy) | (~yx) == ~( ~(~xy) & ~(~yx) )
    return ~( (~ (~x & y)) & (~ (x & ~y)) );
}
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 * and the rest set to 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int thirdBits(void) {
    int word = 0x1; //0b001
    word |= word<<3; //0b001 001
    word |= word<<6; //0b001001 001001
    word |= word<<12; //0b 001001001001 001001001001
    word |= word<<24; //0b 001001001001001001001001 001001001001001001001001
    return word; /* return 0x49249249; */
}
// Rating: 2
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    int isPositive = !(x>>31); //check the MSB to see if x is positive
    int shift = n + ~0;  //minus is not allowed, so use ~0 instead of -1( (0 == ~0 + 1) ==> (~0 == -1) )

    //if x>0, then x should fit in the least n-1(nth bit is sign bit), so x>>shift should be 0

    //elseif x<0, then x is in the form of 2's complement, it is closely related to the sign bit
    //so we can't just shift it as before
    //we should firstly convert it to a positive number,

    //we notice when -2^(n-1)<x<-1(say, it can fit in the n bit), ~x = -x-1
    //so 0< ~x <2^(n-1)-1, say, ~x>>shift should be 0
    return ( isPositive & !(x>>shift) ) | ( !isPositive & !( (~x)>>shift ) );
}
/* 
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x) {
    // x>>31 to check if x<0
    //if x<0 , x>>31 == 0b111...111 == -1( arithmetical right shift )
    //if x>=0, then
    // !!x to check if x==0, or if x>0, return 1;
    return (x>>31) | !!x;
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    //right shift x by n bytes, say, n*8 = n*2^3 = n<<3 bits
    //then the desired byte is at the lowest position
    //mask it out by 0xFF
    return ( x >> (n<<3) ) & 0xFF;
}
// Rating: 3
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
    //x>>n is arithmetically shift,
    //the most n-1 bits maybe all 1(if x<0)
    //we need a mask that has all 0 in its most n-1 bits and all 1 in the remaining bits
    //so 1<<31 to make it 0b1000...,
    //then arithmetically right shift n bits to make it all '1' in the most n bits
    //then left shift one bit to make it all '1' in the most n-1 bits
    //flip each bit to make it all 0 in the most n-1 bits and all 1 in the remaining bits
    int mask = ~( (1<<31)>>n<<1 );
    return mask & (x>>n);
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
    int is_x_Positive = !(x>>31);
    int is_y_Positive = !(y>>31);

    int is_xy_diff_sign = is_x_Positive ^ is_y_Positive;

    int result = x+y;
    int is_r_Positive = !(result>>31);

    int is_xr_same_sign = !(is_x_Positive ^ is_r_Positive);

    //if sign of x and y is different, it won't overflow, return 1 directly
    //else if x and y same sign,
        //check the result of x+y is same sign as x or y
        //if sign of r and x is different, OverFlow! return 0
    return is_xy_diff_sign | ((!is_xy_diff_sign) & is_xr_same_sign);
}
// Rating: 4
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
    //divide into 2 16-bit-groups, check if either has '1'
    //store the result in the low 16 bits
    x |= x>>16;
    //divide into 2 8-bit-groups, check if either has '1'
    //store the result in the low 8 bits
    x |= x>>8;
    //divide into 2 4-bit-groups, check if either has '1'
    //store the result in the low 4 bits
    x |= x>>4;
    //divide into 2 2-bit-groups, check if either has '1'
    //store the result in the low 2 bits
    x |= x>>2;
    //divide into 2 1-bit-groups, check if either has '1'
    //store the result in the lowest bits
    x |= x>>1;

    //extract the lowest bit
    //if 0x01, return 0, else return 1
    return (x & 0x01) ^ 0x01;
}
// Extra Credit: Rating: 3
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    //if x==0, !!x == 0, mask == 0b000....000, return (0 & y) | (~0 & z)
    //else if x!=0, !!x == 1, mask == 0b111....111, return (0b11...11 & y) | (~0b11...11 & z)
    int mask = (!!x)<<31>>31;
    return ( mask & y ) | ( ~mask & z );
}
// Extra Credit: Rating: 4
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
    //if x==0, notZero=0;
    //elseif x!=0, notZero=0x01
    int notZero = !!x;
    //if notZero=0, return 0
    //else if notZero=0x01
        //if x<0, say, !(x>>31)=!(0b11...11)=0, return 0;
        //if x>0,
            //when x is a power of 2, say, x=0b0...010...0
                //then x-1=0b00...01111
                //x&(x-1)=0
            //when x isn't a power of 2,
                //then x&(x-1) != 0
    return notZero & (!(x>>31) & !(x & (x+~0)));
}

/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1<<31;
}

/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    //if x!=0, notZero = 0x01,
    //then notZero<<31>>31 = 0b111...111
    int notZero = !!x;
    return (notZero<<31>>31) & (!(x>>31));
}

/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}

/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    //change sign bit to 0
    unsigned tmp = uf & ~(1<<31);
    //IEEE754 Standard: 1 bit for sign, 8 bits for exp, 23 bits for mantissa
    //when 8-bit-exp = 1111 1111, but 23-bit-mantissa != 0000...000
    //tmp = NaN
    if( tmp > (0xFF<<23) )
        return uf;
    //if not NaN, change the sign bit to its opposite and return
    else
        return uf ^ (1<<31);
}

/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    //transform x into the form of sign*2^(exp)*mantissa
    //sign 1 bit, exp 8 bits, mantissa 23 bits
    //notice that, exp - (2^(8-1)-1) = true_exp

    unsigned int sign, round=0;
    unsigned int leftshift=1;
    unsigned int abs_x = x;
    unsigned int true_exp;
    unsigned int bias = ( 1<<(8-1) ) - 1;

    if(x==0)
        return 0;

    //if x<0, sign = 0x1000..00
    sign = x & (1<<31);
    //abs_x stores the absolute value of x
    if(x<0)
        abs_x = -x;

    //Normalized mantissa
    //the following 6 lines rightshift abs_x until the leftmost '1' is wiped out
    while( !(abs_x & (1<<31)) )
    {
        abs_x <<= 1;
        leftshift++;
    }
    //such as ,after the following line:
    //0b01100...000 will become 0b100...000
    abs_x <<=1;

    //since the mantissa part of float can only contain 23 bit
    //so the low (32-23)=9 bits will be truncated
    //if the low 9 bit > (half=0x0100), abs_x will be rounded up
    if( (abs_x & 0x000001FF) > 0x0100 )
        round = 1;
    //if the low 9 bits == (half=0x0100), abs_x will be rounded to the nearest even number
    //so we need to check the 23th bit(from left to right),
        //if it's 0, no need to round, round is 0
        //if it's 1, then abs_x is odd, we should add abs_x by 1 to make it even, say, round = 1
    //In conclusion,
    //if the 23th bit(from left to right, equivalent to 10th bit)==1 && low 9 bits==0x0100
    //which is equivalent to abs_x & 0x03ff == 0x0300
    //then round =1
    else if( (abs_x & 0x000003FF) == 0x0300 )
        round = 1;

    //the bits number of abs_x excluding the leftmost '1'
    true_exp = 32 - leftshift;

    //combime the sign, exp, matissa up
    //In IEEE Standard, exp shoulde be coded as biased value
    return (sign | ( (bias + true_exp) << 23 ) | (abs_x>>9)) + round ;
}

/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    //sign 1 bit, exp 8 bits, mantissa 23 bits

    unsigned int sign = uf & (1<<31);
    unsigned int exp_mask = ((1<<31) >> 8) & ~(1<<31); //0b0 1111 1111 000...00
    //extract the 8-bit exp, equivalent to exp = uf & 0b0 1111 1111 000...00
    unsigned int exp = uf & ( exp_mask );

    //if exp == 0
    if( !exp )
        //because exp is 0, then we should multiply the mantissa by 2
        //left shift may lose the sign bit, so need to Or sign
        uf = (uf<<1) | sign;
    //elseif exp != 0
    //if uf != NaN ( if exp = 0b1111 1111, then uf == NaN )
    else if( exp != exp_mask )
        //increase exp by 1, equivalent to multiplying uf by 2
        uf += (1 << (32-1-8));

    return uf;
}

/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    //if x<0, is_x_Negative = 0x01
    int is_x_Negative = !!(x>>31);
    //if y<0, is_y_Negative = 0x01
    int is_y_Negative = !!(y>>31);

    //if xy different sign, is_xy_diff_sign = 0x01
    int is_xy_diff_sign = is_x_Negative ^ is_y_Negative;

    //if xy different sign, just return is_x_Negative
    //if xy same sign, check the sign of x-y = x+~y+1
    //because when y=0x1000...00, ~y+1 overflows
    //notice that, if x<=y return 1
    //so we can take the case '=' as '<'
    //calculate x-(y+1) = x-y-1 = x+~y+1-1 = x+~y
    return ( is_xy_diff_sign & is_x_Negative ) \
        |  ( (!is_xy_diff_sign) & (x + ~y)>>31 );
}

/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    //if x<0, isNegative=0b11...111
    //elseif x>=0, isNegative=0
    int isNegative = x>>31;

    //if x>0, then x/(2^n) = x>>n
    //if x<0
        //when x/(2^n) isn't an integer,
            //we should (x>>n)+1 to make it round to zero
        //but when x/(2^n) is an iterger,
            //we should just return x>>n
            //so we do a trick
            //increase x by (2^n)-1, then right shift i by n bits

    //if x<0, bias=(2^n)-1
    //elseif x>=0, bias=0
    int bias = isNegative & ( (1<<n) + ~0 );

    return (x+bias)>>n;
}

/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    //equivalent to finding the position of the leftmost '1' bit
    //the following algorithm is similar to binary search
    //shift1-5 records the righ-shift steps of each search
    //finally just add them all and return the result

    ////////////////////////////////////////////////////////////////////
    int shift1;
    int shift2;
    int shift3;
    int shift4;
    int shift5;

    /////////////////////////////////////////////////////////////////
    //if the leftmost 16 bits isn't all '0' bits, not_all_bit_0 = 1
    int not_all_bit_0 = !!(x>>16);
    //if leftmost 16 bits isn't all '0', shift1 = 16, else shift1 = 0
    shift1 = not_all_bit_0<<4;
    //if shift1 = 16, move the leftmost 16 bits down to the rightmost
    x = x>>shift1;


    /////////////////////////////////////////////////////////////////
    //if the leftmost 8 bits isn't all '0' bits, not_all_bit_0 = 1
    not_all_bit_0 = !!(x>>8);
    //if leftmost 8 bits isn't all '0', shift2 = 8, else shift2 = 0
    shift2 = not_all_bit_0<<3;
    //if shift2 = 8, move the leftmost 8 bits down to the rightmost
    x = x>>shift2;


    /////////////////////////////////////////////////////////////////
    //if the leftmost 4 bits isn't all '0' bits, not_all_bit_0 = 1
    not_all_bit_0 = !!(x>>4);
    //if leftmost 4 bits isn't all '0', shift3 = 4, else shift3 = 0
    shift3 = not_all_bit_0<<2;
    //if shift3 = 8, move the leftmost 4 bits down to the rightmost
    x = x>>shift3;


    /////////////////////////////////////////////////////////////////
    //if the leftmost 2 bits isn't all '0' bits, not_all_bit_0 = 1
    not_all_bit_0 = !!(x>>2);
    //if leftmost 2 bits isn't all '0', shift4 = 2, else shift4 = 0
    shift4 = not_all_bit_0<<1;
    //if shift4 = 2, move the leftmost 2 bits down to the rightmost
    x = x>>shift4;

    /////////////////////////////////////////////////////////////////
    //if the leftmost 1 bits isn't all '0' bits, not_all_bit_0 = 1
    not_all_bit_0 = !!(x>>1);
    //if leftmost 2 bits isn't all '0', shift5 = 1, else shift5 = 0
    shift5 = not_all_bit_0;


    ///////////////////////////////////////////////////////////////////
    //add all of shifts up, then it's the position of the leftmost '1'
    return shift1+shift2+shift3+shift4+shift5;
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int result = 0;
    int result1 = 0;
    int result2 = 0;
    int result3 = 0;

    ///////////////////////////////////////////////////////////////////////
    int mask = 0x11;
    mask |= mask<<8;
    mask |= mask<<16; //mask = 0x1111 1111

    //the following four lines count the number of 1's of every 4-bit
    result += x&mask;
    result += (x>>1)&mask;
    result += (x>>2)&mask;
    result += (x>>3)&mask;


    ///////////////////////////////////////////////////////////////////////
    mask = 0x0F;
    mask |= mask<<8;
    mask |= mask<<16; // mask = 0x0F0F 0F0F

    //the following 2 lines count the number of 1's of every 8-bit
    result1 += result & mask;
    result1 += (result>>4)&mask;


    ///////////////////////////////////////////////////////////////////////
    mask = 0xFF;
    mask |= mask<<16;// mask = 0x0000 00FF

    //the following 2 lines count the number of 1's of every 16-bit
    result2 += result1 & mask;
    result2 += (result1>>8)&mask;


    ///////////////////////////////////////////////////////////////////////
    mask = 0xFF;

    //the following 2 lines count the number of 1's of the whole word
    result3 += result2 & mask;
    result3 += (result2>>16)&mask;


    return result3;
}