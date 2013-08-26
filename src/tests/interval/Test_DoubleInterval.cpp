#include "gtest/gtest.h"
#include "carl/interval/DoubleInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>

using namespace carl;

TEST(DoubleInterval, Constructor)
{
    
}

TEST(DoubleInterval, Getters)
{
    
}

TEST(DoubleInterval, Addition)
{
    
}


TEST(DoubleInterval, Substraction)
{
    
}


TEST(DoubleInterval, Multiplication)
{
    
}


TEST(DoubleInterval, Division)
{
    DoubleInterval i0  = DoubleInterval( 0.0, DoubleInterval::STRICT_BOUND, 0.0, DoubleInterval::STRICT_BOUND );
    DoubleInterval i1  = DoubleInterval();
    DoubleInterval i2  = DoubleInterval( 0.0, DoubleInterval::INFINITY_BOUND, 0.0, DoubleInterval::INFINITY_BOUND );
    DoubleInterval i3  = DoubleInterval( 0.0, DoubleInterval::INFINITY_BOUND, 10.0, DoubleInterval::WEAK_BOUND );

    DoubleInterval a0  = DoubleInterval( -2, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a1  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a2  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND );
    DoubleInterval a3  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval a4  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a5  = DoubleInterval( -2, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a6  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval a7  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval a8  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval b0  = DoubleInterval( -2, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b1  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND );
    DoubleInterval b2  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b3  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval b4  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval b5  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval b6  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b7  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b8  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval b9  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b10 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval b11 = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval b12 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval ia = DoubleInterval( 1, 4 );
    DoubleInterval ib = DoubleInterval( 2, 5 );
    DoubleInterval ic = DoubleInterval( -2, 3 );
    DoubleInterval id = DoubleInterval( 0, 2 );

    VariablePool& varPool = VariablePool::getInstance();
    Variable a = varPool.getFreshVariable();
    varPool.setVariableName(a, "a");
    Variable b = varPool.getFreshVariable();
    varPool.setVariableName(b, "b");
    Variable c = varPool.getFreshVariable();
    varPool.setVariableName(c, "c");
    Variable d = varPool.getFreshVariable();
    varPool.setVariableName(c, "d");
    
    
    DoubleInterval::evaldoubleintervalmap mapt;
    mapt[a] = ia;
    mapt[b] = ib;
    mapt[c] = ic;
    mapt[d] = id;
    
    MultivariatePolynomial<cln::cl_RA> e1;
    e1 += a;
    e1 += b;
    e1 += c;
    e1 += d;
    MultivariatePolynomial<cln::cl_RA> e2;
    e2 += a*b;
    e2 += c*d;
    MultivariatePolynomial<cln::cl_RA> e3;
    e3 += a*b*c;
    e3 += d;
    MultivariatePolynomial<cln::cl_RA> e4;
    e4 += a;
    e4 += b;
    e4 -= c;
    e4 -= d;
    MultivariatePolynomial<cln::cl_RA> e5;
    e5 += a;
    e5 += b;
    e5 += 3;
    e5 += 4;
    MultivariatePolynomial<cln::cl_RA> e6;
    e6 += (cln::cl_RA)12*a;
    e6 += (cln::cl_RA)3*b;
    e6 += Monomial(c, 2);
    e6 -= Monomial(d,3);
    MultivariatePolynomial<cln::cl_RA> e7;
    e7 += a;
//    e7 += b*(c+a)*(c+a)*d;
    e7 += b;
    DoubleInterval result1, result2;
    // Table 7 Tests: Division without containing 0
    a0.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a0.right() / b0.left(), DoubleInterval::WEAK_BOUND, a0.left() / b0.right(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a1.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a1.right() / b0.right(), DoubleInterval::WEAK_BOUND, a1.left() / b0.right(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a2.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a2.right() / b0.right(), DoubleInterval::WEAK_BOUND, a2.left() / b0.left(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a3.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a4.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a4.right() / b0.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a5.right() / b0.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a6.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a6.left() / b0.right(), DoubleInterval::WEAK_BOUND ), result1 );
    a7.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a7.left() / b0.left(), DoubleInterval::WEAK_BOUND ), result1 );
    a8.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );

    a0.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a0.left() / b1.left(), DoubleInterval::WEAK_BOUND, a0.right() / b1.right(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a1.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a1.left() / b1.left(), DoubleInterval::WEAK_BOUND, a1.right() / b1.left(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a2.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a2.left() / b1.right(), DoubleInterval::WEAK_BOUND, a2.right() / b1.left(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a3.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a4.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a4.right() / b1.right(), DoubleInterval::WEAK_BOUND ), result1 );
    a5.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a5.right() / b1.left(), DoubleInterval::WEAK_BOUND ), result1 );
    a6.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a6.left() / b1.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a7.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a7.left() / b1.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a8.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );

    a0.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, a0.left() / b2.right(), DoubleInterval::WEAK_BOUND ), result1 );
    a1.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( a1.right() / b2.right(), DoubleInterval::WEAK_BOUND, a1.left() / b2.right(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a2.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( a2.right() / b2.right(), DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a3.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a4.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( a5.right() / b2.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a6.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a6.left() / b2.right(), DoubleInterval::WEAK_BOUND ), result1 );
    a7.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a8.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );

    a0.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( a0.left() / b3.left(), DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a1.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( a1.left() / b3.left(), DoubleInterval::WEAK_BOUND, a1.right() / b3.left(), DoubleInterval::WEAK_BOUND ),
                          result1 );
    a2.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, a2.right() / b3.left(), DoubleInterval::WEAK_BOUND ), result1 );
    a3.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a4.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a5.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a5.right() / b3.left(), DoubleInterval::WEAK_BOUND ), result1 );
    a6.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( a6.left() / b3.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a7.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a8.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );

    //Table 8 tests with division containin 0
    a0.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );
    a1.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );

    a4.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );
    a5.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );
    a8.div_ext( result1, result2, b4 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( a0.right() / b5.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a1.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a2.left() / b5.left(), DoubleInterval::WEAK_BOUND ), result1 );

    a4.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( a4.right() / b5.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a7.left() / b5.left(), DoubleInterval::WEAK_BOUND ), result1 );
    a8.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( a0.right() / b6.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a0.right() / b6.right(), DoubleInterval::WEAK_BOUND ), result2 );
    a1.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a2.left() / b6.left(), DoubleInterval::WEAK_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( a2.left() / b6.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result2 );

    a4.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a4.right() / b6.right(), DoubleInterval::WEAK_BOUND ), result2 );
    EXPECT_EQ( DoubleInterval( a4.right() / b6.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a7.left() / b6.left(), DoubleInterval::WEAK_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( a7.left() / b6.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result2 );
    a8.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a0.right() / b7.right(), DoubleInterval::WEAK_BOUND ), result1 );
    a1.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( a2.left() / b7.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );

    a4.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a4.right() / b7.right(), DoubleInterval::WEAK_BOUND ), result1 );
    a5.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( a7.left() / b7.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a8.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a1.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );

    a4.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a8.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a0.right() / b9.right(), DoubleInterval::WEAK_BOUND ), result2 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a1.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( a2.left() / b9.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result2 );

    a4.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a4.right() / b9.right(), DoubleInterval::WEAK_BOUND ), result2 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( a7.left() / b9.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result2 );
    a8.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result2 );
    EXPECT_EQ( DoubleInterval( a0.right() / b10.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a1.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a2.left() / b10.left(), DoubleInterval::WEAK_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result2 );

    a4.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result2 );
    EXPECT_EQ( DoubleInterval( a4.right() / b10.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a5.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, a7.left() / b10.left(), DoubleInterval::WEAK_BOUND ), result1 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result2 );
    a8.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a1.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );

    a4.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result1 );
    a5.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result1 );
    a8.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a1.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a4.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a5.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a8.div_ext( result1, result2, b12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
}