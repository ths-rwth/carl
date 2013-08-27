#include "gtest/gtest.h"
#include "carl/interval/DoubleInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>

using namespace carl;

TEST(DoubleInterval, Constructor)
{
    DoubleInterval test1 = DoubleInterval(-1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND);
    DoubleInterval test2 = DoubleInterval(-1, DoubleInterval::STRICT_BOUND, 1, DoubleInterval::STRICT_BOUND);
    DoubleInterval test3 = DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    EXPECT_EQ(DoubleInterval(1, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND), DoubleInterval::emptyInterval());
    DoubleInterval test5 = DoubleInterval::unboundedInterval();
    DoubleInterval test6 = DoubleInterval::emptyInterval();
    DoubleInterval test7 = DoubleInterval((mpz_class)-1, DoubleInterval::WEAK_BOUND, (mpz_class)1, DoubleInterval::WEAK_BOUND);
    SUCCEED();
}

TEST(DoubleInterval, Getters)
{
    DoubleInterval test1 = DoubleInterval(-1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::STRICT_BOUND);
    EXPECT_EQ(test1.left(), -1);
    EXPECT_EQ(test1.right(), 1);
    EXPECT_EQ(test1.leftType(), DoubleInterval::WEAK_BOUND);
    EXPECT_EQ(test1.rightType(), DoubleInterval::STRICT_BOUND);
}

TEST(DoubleInterval, Addition)
{
    DoubleInterval a0 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval a1 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval a2 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    DoubleInterval a3 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    
    DoubleInterval b0 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval b1 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval b2 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    DoubleInterval b3 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    
    DoubleInterval result;
    
    result = a0.add(b0);
    EXPECT_EQ( DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 4, DoubleInterval::WEAK_BOUND), result);
    result = a0.add(b1);
    EXPECT_EQ( DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 4, DoubleInterval::WEAK_BOUND), result);
    result = a0.add(b2);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a0.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a1.add(b0);
    EXPECT_EQ( DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 4, DoubleInterval::WEAK_BOUND), result);
    result = a1.add(b1);
    EXPECT_EQ( DoubleInterval(-2, DoubleInterval::WEAK_BOUND, 4, DoubleInterval::WEAK_BOUND), result);
    result = a1.add(b2);
    EXPECT_EQ( DoubleInterval(-2, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND), result);
    result = a1.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a2.add(b0);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a2.add(b1);
    EXPECT_EQ( DoubleInterval(-2, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND), result);
    result = a2.add(b2);
    EXPECT_EQ( DoubleInterval(-2, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND), result);
    result = a2.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a3.add(b0);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b1);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b2);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
}


TEST(DoubleInterval, Subtraction)
{
    DoubleInterval a0 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval a1 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval a2 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    DoubleInterval a3 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    
    DoubleInterval b0 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval b1 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND);
    DoubleInterval b2 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    DoubleInterval b3 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND);
    
    DoubleInterval result;
    
    result = a0.add(b0.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a0.add(b1.minus());
    EXPECT_EQ( DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 3, DoubleInterval::WEAK_BOUND), result);
    result = a0.add(b2.minus());
    EXPECT_EQ( DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 3, DoubleInterval::WEAK_BOUND), result);
    result = a0.add(b3.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a1.add(b0.minus());
    EXPECT_EQ( DoubleInterval(-3, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND), result);
    result = a1.add(b1.minus());
    EXPECT_EQ( DoubleInterval(-3, DoubleInterval::WEAK_BOUND, 3, DoubleInterval::WEAK_BOUND), result);
    result = a1.add(b2.minus());
    EXPECT_EQ( DoubleInterval(-1, DoubleInterval::INFINITY_BOUND, 3, DoubleInterval::WEAK_BOUND), result);
    result = a1.add(b3.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a2.add(b0.minus());
    EXPECT_EQ( DoubleInterval(-3, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND), result);
    result = a2.add(b1.minus());
    EXPECT_EQ( DoubleInterval(-3, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND), result);
    result = a2.add(b2.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a2.add(b3.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a3.add(b0.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b1.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b2.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b3.minus());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
}


TEST(DoubleInterval, Multiplication)
{
    DoubleInterval c0  = DoubleInterval( -2, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval c1  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval c2  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND );
    DoubleInterval c3  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval c4  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval c5  = DoubleInterval( -2, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval c6  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval c7  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval c8  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval d0  = DoubleInterval( -2, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval d1  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND );
    DoubleInterval d2  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval d3  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval d4  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval d5  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval d6  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval d7  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval d8  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval d9  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval d10 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval d11 = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval d12 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval result;

    result = c0.mul( d0 );
    EXPECT_EQ( DoubleInterval( c0.right() * d0.right(), DoubleInterval::WEAK_BOUND, c0.left() * d0.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( DoubleInterval( c0.right() * d5.right(), DoubleInterval::WEAK_BOUND, c0.left() * d5.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( DoubleInterval( c1.right() * d0.left(), DoubleInterval::WEAK_BOUND, c1.left() * d0.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( DoubleInterval( c1.right() * d5.left(), DoubleInterval::WEAK_BOUND, c1.left() * d5.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( DoubleInterval( c2.right() * d0.left(), DoubleInterval::WEAK_BOUND, c2.left() * d0.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( DoubleInterval( c2.right() * d5.left(), DoubleInterval::WEAK_BOUND, c2.left() * d5.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( DoubleInterval( c4.right() * d0.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( DoubleInterval( c4.right() * d5.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( DoubleInterval( c5.right() * d0.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( DoubleInterval( c5.right() * d5.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c6.left() * d0.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c6.left() * d5.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c7.left() * d0.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c7.left() * d5.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d6 );
    EXPECT_EQ( DoubleInterval( c0.left() * d6.right(), DoubleInterval::WEAK_BOUND, c0.left() * d6.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( DoubleInterval( ( c1.left() * d6.right() < c1.right() * d6.left() ? c1.left() * d6.right() : c1.right() * d6.left()),
                                         DoubleInterval::WEAK_BOUND,
                                         c1.left() * d6.left() > c1.right() * d6.right() ? c1.left() * d6.left() : c1.right() * d6.right(),
                                         DoubleInterval::WEAK_BOUND ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( DoubleInterval( c2.right() * d6.left(), DoubleInterval::WEAK_BOUND, c2.right() * d6.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c3.mul( d6 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d1 );
    EXPECT_EQ( DoubleInterval( c0.left() * d1.right(), DoubleInterval::WEAK_BOUND, c0.right() * d1.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( DoubleInterval( c0.left() * d7.right(), DoubleInterval::WEAK_BOUND, c0.right() * d7.left(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( DoubleInterval( c1.left() * d1.right(), DoubleInterval::WEAK_BOUND, c1.right() * d1.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( DoubleInterval( c1.left() * d7.right(), DoubleInterval::WEAK_BOUND, c1.right() * d7.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( DoubleInterval( c2.left() * d1.left(), DoubleInterval::WEAK_BOUND, c2.right() * d1.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( DoubleInterval( c2.left() * d7.left(), DoubleInterval::WEAK_BOUND, c2.right() * d7.right(), DoubleInterval::WEAK_BOUND ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c4.right() * d1.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c4.right() * d7.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c5.right() * d1.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c5.right() * d7.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( DoubleInterval( c6.left() * d1.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( DoubleInterval( c6.left() * d7.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( DoubleInterval( c7.left() * d1.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( DoubleInterval( c7.left() * d7.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c8.mul( d1 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c1.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c2.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );;
    result = c5.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c6.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c7.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c8.mul( d4 );
    EXPECT_EQ( DoubleInterval( 0 ), result );

    result = c0.mul( d2 );
    EXPECT_EQ( DoubleInterval( c0.right() * d2.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.right() * d8.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c2.left() * d2.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c2.left() * d8.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( DoubleInterval( c4.right() * d2.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( DoubleInterval( c4.right() * d8.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c7.left() * d2.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c7.left() * d8.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( DoubleInterval( c0.left() * d9.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.left() * d8.right(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c2.right() * d9.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c2.right() * d8.right(), DoubleInterval::WEAK_BOUND ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c4.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c5.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c0.left() * d11.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c0.left() * d10.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( DoubleInterval( c2.right() * d11.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( DoubleInterval( c2.right() * d10.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d10 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND ), result );
    result = c4.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c0.right() * d11.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c0.right() * d3.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( DoubleInterval( c2.left() * d11.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( DoubleInterval( c2.left() * d3.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c4.right() * d11.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, c4.right() * d3.left(), DoubleInterval::WEAK_BOUND ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( DoubleInterval( c7.left() * d11.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( DoubleInterval( c7.left() * d3.left(), DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND ), result );
    result = c8.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c3.mul( d12 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d12 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
}


TEST(DoubleInterval, Division)
{

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