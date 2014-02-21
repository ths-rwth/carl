#include "gtest/gtest.h"
#include "carl/interval/DoubleInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>

using namespace carl;

TEST(DoubleInterval, Constructor)
{
    DoubleInterval test1 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    DoubleInterval test2 = DoubleInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval test3 = DoubleInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, -1, BoundType::WEAK), DoubleInterval::emptyInterval());
    DoubleInterval test5 = DoubleInterval::unboundedInterval();
    DoubleInterval test6 = DoubleInterval::emptyInterval();
    DoubleInterval test7 = DoubleInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);
    DoubleInterval test8 = DoubleInterval(2, BoundType::STRICT, 0, BoundType::INFTY);
    SUCCEED();
}

TEST(DoubleInterval, Getters)
{
    DoubleInterval test1 = DoubleInterval(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    EXPECT_EQ(-1, test1.left());
    EXPECT_EQ(1, test1.right());
    EXPECT_EQ(BoundType::WEAK, test1.leftType());
    EXPECT_EQ(BoundType::STRICT, test1.rightType());
    DoubleInterval test2 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    EXPECT_EQ(-1, test2.left());
    EXPECT_EQ(1, test2.right());
    EXPECT_EQ(BoundType::WEAK, test2.leftType());
    EXPECT_EQ(BoundType::STRICT, test2.rightType());
    DoubleInterval test3 = DoubleInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::STRICT);
    EXPECT_EQ(-1, test3.left());
    EXPECT_EQ(1, test3.right());
    EXPECT_EQ(BoundType::WEAK, test3.leftType());
    EXPECT_EQ(BoundType::STRICT, test3.rightType());
    DoubleInterval test4 = DoubleInterval((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
    EXPECT_EQ(-1, test4.left());
    EXPECT_EQ(1, test4.right());
    EXPECT_EQ(BoundType::WEAK, test4.leftType());
    EXPECT_EQ(BoundType::STRICT, test4.rightType());
}

TEST(DoubleInterval, Addition)
{
    DoubleInterval a0 = DoubleInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    DoubleInterval a1 = DoubleInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    DoubleInterval a2 = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    DoubleInterval a3 = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    DoubleInterval b0 = DoubleInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    DoubleInterval b1 = DoubleInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    DoubleInterval b2 = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    DoubleInterval b3 = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    DoubleInterval result;
    
    result = a0.add(b0);
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b1);
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b2);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a0.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a1.add(b0);
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a1.add(b1);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a2.add(b0);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a2.add(b1);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, 1, BoundType::INFTY), result);
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
    DoubleInterval a0 = DoubleInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    DoubleInterval a1 = DoubleInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    DoubleInterval a2 = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    DoubleInterval a3 = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    DoubleInterval b0 = DoubleInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    DoubleInterval b1 = DoubleInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    DoubleInterval b2 = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    DoubleInterval b3 = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    DoubleInterval result;
    
    result = a0.add(b0.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a0.add(b1.inverse());
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b2.inverse());
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a1.add(b0.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a1.add(b1.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 3, BoundType::WEAK), result);
    result = a1.add(b2.inverse());
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a1.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a2.add(b0.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b1.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b2.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a2.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a3.add(b0.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b1.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b2.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a3.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
}


TEST(DoubleInterval, Multiplication)
{
    DoubleInterval c0  = DoubleInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    DoubleInterval c1  = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    DoubleInterval c2  = DoubleInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    DoubleInterval c3  = DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    DoubleInterval c4  = DoubleInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    DoubleInterval c5  = DoubleInterval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    DoubleInterval c6  = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval c7  = DoubleInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval c8  = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    DoubleInterval d0  = DoubleInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    DoubleInterval d1  = DoubleInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    DoubleInterval d2  = DoubleInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    DoubleInterval d3  = DoubleInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    DoubleInterval d4  = DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    DoubleInterval d5  = DoubleInterval( -1, BoundType::WEAK, 0, BoundType::WEAK );
    DoubleInterval d6  = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    DoubleInterval d7  = DoubleInterval( 0, BoundType::WEAK, 1, BoundType::WEAK );
    DoubleInterval d8  = DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK );
    DoubleInterval d9  = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    DoubleInterval d10 = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval d11 = DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval d12 = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    DoubleInterval result;

    result = c0.mul( d0 );
    EXPECT_EQ( DoubleInterval( c0.right() * d0.right(), BoundType::WEAK, c0.left() * d0.left(), BoundType::WEAK ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( DoubleInterval( c0.right() * d5.right(), BoundType::WEAK, c0.left() * d5.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( DoubleInterval( c1.right() * d0.left(), BoundType::WEAK, c1.left() * d0.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( DoubleInterval( c1.right() * d5.left(), BoundType::WEAK, c1.left() * d5.left(), BoundType::WEAK ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( DoubleInterval( c2.right() * d0.left(), BoundType::WEAK, c2.left() * d0.right(), BoundType::WEAK ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( DoubleInterval( c2.right() * d5.left(), BoundType::WEAK, c2.left() * d5.right(), BoundType::WEAK ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( DoubleInterval( c4.right() * d0.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( DoubleInterval( c4.right() * d5.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( DoubleInterval( c5.right() * d0.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( DoubleInterval( c5.right() * d5.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c6.left() * d0.left(), BoundType::WEAK ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c6.left() * d5.left(), BoundType::WEAK ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c7.left() * d0.right(), BoundType::WEAK ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c7.left() * d5.right(), BoundType::WEAK ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d6 );
    EXPECT_EQ( DoubleInterval( c0.left() * d6.right(), BoundType::WEAK, c0.left() * d6.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( DoubleInterval( ( c1.left() * d6.right() < c1.right() * d6.left() ? c1.left() * d6.right() : c1.right() * d6.left()),
                                         BoundType::WEAK,
                                         c1.left() * d6.left() > c1.right() * d6.right() ? c1.left() * d6.left() : c1.right() * d6.right(),
                                         BoundType::WEAK ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( DoubleInterval( c2.right() * d6.left(), BoundType::WEAK, c2.right() * d6.right(), BoundType::WEAK ),
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
    EXPECT_EQ( DoubleInterval( c0.left() * d1.right(), BoundType::WEAK, c0.right() * d1.left(), BoundType::WEAK ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( DoubleInterval( c0.left() * d7.right(), BoundType::WEAK, c0.right() * d7.left(), BoundType::WEAK ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( DoubleInterval( c1.left() * d1.right(), BoundType::WEAK, c1.right() * d1.right(), BoundType::WEAK ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( DoubleInterval( c1.left() * d7.right(), BoundType::WEAK, c1.right() * d7.right(), BoundType::WEAK ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( DoubleInterval( c2.left() * d1.left(), BoundType::WEAK, c2.right() * d1.right(), BoundType::WEAK ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( DoubleInterval( c2.left() * d7.left(), BoundType::WEAK, c2.right() * d7.right(), BoundType::WEAK ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c4.right() * d1.left(), BoundType::WEAK ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c4.right() * d7.left(), BoundType::WEAK ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c5.right() * d1.right(), BoundType::WEAK ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c5.right() * d7.right(), BoundType::WEAK ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( DoubleInterval( c6.left() * d1.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( DoubleInterval( c6.left() * d7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( DoubleInterval( c7.left() * d1.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( DoubleInterval( c7.left() * d7.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
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
    EXPECT_EQ( DoubleInterval( c0.right() * d2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.right() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c2.left() * d2.right(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c2.left() * d8.right(), BoundType::WEAK ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( DoubleInterval( c4.right() * d2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( DoubleInterval( c4.right() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c7.left() * d2.right(), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c7.left() * d8.right(), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( DoubleInterval( c0.left() * d9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.left() * d8.right(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c2.right() * d9.right(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c2.right() * d8.right(), BoundType::WEAK ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c4.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
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
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c0.left() * d11.left(), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c0.left() * d10.left(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( DoubleInterval( c2.right() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( DoubleInterval( c2.right() * d10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d10 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
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
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c0.right() * d11.left(), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c0.right() * d3.left(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( DoubleInterval( c2.left() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( DoubleInterval( c2.left() * d3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c4.right() * d11.left(), BoundType::WEAK ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, c4.right() * d3.left(), BoundType::WEAK ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( DoubleInterval( c7.left() * d11.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( DoubleInterval( c7.left() * d3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result );
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


TEST(DoubleInterval, ExtendedDivision)
{

    DoubleInterval a0  = DoubleInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    DoubleInterval a1  = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    DoubleInterval a2  = DoubleInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    DoubleInterval a3  = DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    DoubleInterval a4  = DoubleInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    DoubleInterval a5  = DoubleInterval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    DoubleInterval a6  = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval a7  = DoubleInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval a8  = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    DoubleInterval b0  = DoubleInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    DoubleInterval b1  = DoubleInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    DoubleInterval b2  = DoubleInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    DoubleInterval b3  = DoubleInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    DoubleInterval b4  = DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    DoubleInterval b5  = DoubleInterval( -1, BoundType::WEAK, 0, BoundType::WEAK );
    DoubleInterval b6  = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    DoubleInterval b7  = DoubleInterval( 0, BoundType::WEAK, 1, BoundType::WEAK );
    DoubleInterval b8  = DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK );
    DoubleInterval b9  = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    DoubleInterval b10 = DoubleInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval b11 = DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY );
    DoubleInterval b12 = DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    DoubleInterval result1, result2;
    // Table 7 Tests: Division without containing 0
    a0.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a0.right() / b0.left(), BoundType::WEAK, a0.left() / b0.right(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a1.right() / b0.right(), BoundType::WEAK, a1.left() / b0.right(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a2.right() / b0.right(), BoundType::WEAK, a2.left() / b0.left(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a4.right() / b0.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( a5.right() / b0.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a6.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a6.left() / b0.right(), BoundType::WEAK ), result1 );
    a7.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a7.left() / b0.left(), BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b0 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a0.left() / b1.left(), BoundType::WEAK, a0.right() / b1.right(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a1.left() / b1.left(), BoundType::WEAK, a1.right() / b1.left(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a2.left() / b1.right(), BoundType::WEAK, a2.right() / b1.left(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a4.right() / b1.right(), BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a5.right() / b1.left(), BoundType::WEAK ), result1 );
    a6.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a6.left() / b1.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a7.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( a7.left() / b1.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b1 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, a0.left() / b2.right(), BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( a1.right() / b2.right(), BoundType::WEAK, a1.left() / b2.right(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( a2.right() / b2.right(), BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a3.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( a5.right() / b2.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a6.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a6.left() / b2.right(), BoundType::WEAK ), result1 );
    a7.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( a0.left() / b3.left(), BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( a1.left() / b3.left(), BoundType::WEAK, a1.right() / b3.left(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, a2.right() / b3.left(), BoundType::WEAK ), result1 );
    a3.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a5.right() / b3.left(), BoundType::WEAK ), result1 );
    a6.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( a6.left() / b3.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a7.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b3 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

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
    EXPECT_EQ( DoubleInterval( a0.right() / b5.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a2.left() / b5.left(), BoundType::WEAK ), result1 );

    a4.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( a4.right() / b5.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a7.left() / b5.left(), BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( a0.right() / b6.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a0.right() / b6.right(), BoundType::WEAK ), result2 );
    a1.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a2.left() / b6.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a2.left() / b6.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a4.right() / b6.right(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a4.right() / b6.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a7.left() / b6.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a7.left() / b6.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( result1, result2, b6 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a0.right() / b7.right(), BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( a2.left() / b7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a4.right() / b7.right(), BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval( a7.left() / b7.right(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( result1, result2, b7 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );

    a4.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( result1, result2, b8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a0.right() / b9.right(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a2.left() / b9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a4.right() / b9.right(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a7.left() / b9.right(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( result1, result2, b9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a0.right() / b10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a2.left() / b10.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a4.right() / b10.left(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, a7.left() / b10.left(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( result1, result2, b10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a1.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( result1, result2, b11 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
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


TEST(DoubleInterval, Intersection)
{
    DoubleInterval a1(-1,BoundType::WEAK,1,BoundType::WEAK);
    
    DoubleInterval b01(2,BoundType::WEAK,3,BoundType::WEAK);
    DoubleInterval b02(1,BoundType::WEAK,3,BoundType::WEAK);
    DoubleInterval b03(0,BoundType::WEAK,3,BoundType::WEAK);
    DoubleInterval b04(-1,BoundType::WEAK,1,BoundType::WEAK);
    DoubleInterval b05(-2,BoundType::WEAK,0,BoundType::WEAK);
    
    DoubleInterval b06(-2,BoundType::WEAK,-1,BoundType::WEAK);
    DoubleInterval b07(-3,BoundType::WEAK,-2,BoundType::WEAK);
    DoubleInterval b08(-1,BoundType::WEAK,-1,BoundType::WEAK);
    DoubleInterval b09(1,BoundType::WEAK,1,BoundType::WEAK);
    DoubleInterval b10(0,BoundType::WEAK,1,BoundType::WEAK);
    
    DoubleInterval b11(-1,BoundType::WEAK,0,BoundType::WEAK);
    DoubleInterval b12(-0.5,BoundType::WEAK,0.5,BoundType::WEAK);
    DoubleInterval b13(-2,BoundType::WEAK,2,BoundType::WEAK);
    DoubleInterval b14(0,BoundType::STRICT,0,BoundType::STRICT);
    DoubleInterval b15(-1,BoundType::INFTY,1,BoundType::INFTY);
    
    DoubleInterval b16(-1,BoundType::INFTY,-1,BoundType::WEAK);
    DoubleInterval b17(-1,BoundType::INFTY,0,BoundType::WEAK);
    DoubleInterval b18(-1,BoundType::INFTY,1,BoundType::WEAK);
    DoubleInterval b19(-1,BoundType::WEAK,1,BoundType::INFTY);
    DoubleInterval b20(0,BoundType::WEAK,1,BoundType::INFTY);
    
    DoubleInterval b21(1,BoundType::WEAK,1,BoundType::INFTY);
    
    EXPECT_EQ(DoubleInterval::emptyInterval(), a1.intersect(b01));
    EXPECT_EQ(DoubleInterval(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b02));
    EXPECT_EQ(DoubleInterval(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b03));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b04));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b05));
    
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), a1.intersect(b06));
    EXPECT_EQ(DoubleInterval::emptyInterval(), a1.intersect(b07));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), a1.intersect(b08));
    EXPECT_EQ(DoubleInterval(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b09));
    EXPECT_EQ(DoubleInterval(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b10));
    
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b11));
    EXPECT_EQ(DoubleInterval(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), a1.intersect(b12));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b13));
    EXPECT_EQ(DoubleInterval::emptyInterval(), a1.intersect(b14));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b15));
    
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), a1.intersect(b16));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b17));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b18));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b19));
    EXPECT_EQ(DoubleInterval(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b20));
    
    EXPECT_EQ(DoubleInterval(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b21));
}

TEST(DoubleInterval, Split)
{
    DoubleInterval i1(-1, BoundType::INFTY, 1, BoundType::INFTY);
    DoubleInterval i2(-1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval i3(-1, BoundType::WEAK, 1, BoundType::WEAK);
    DoubleInterval i4(0, BoundType::STRICT, 0, BoundType::STRICT);
    
    DoubleInterval resA;
    DoubleInterval resB;
    
    i1.split(resA, resB);
    EXPECT_EQ(DoubleInterval(-1, BoundType::INFTY, 0, BoundType::STRICT), resA);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::INFTY), resB);
    
    i2.split(resA, resB);
    EXPECT_EQ(DoubleInterval(-1, BoundType::STRICT, 0, BoundType::STRICT), resA);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::STRICT), resB);
    
    i3.split(resA, resB);
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 0, BoundType::STRICT), resA);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::WEAK), resB);
    
    i4.split(resA, resB);
    EXPECT_EQ(DoubleInterval(0, BoundType::STRICT, 0, BoundType::STRICT), resA);
    EXPECT_EQ(DoubleInterval(0, BoundType::STRICT, 0, BoundType::STRICT), resB);
    
    // uniform multi-split
    DoubleInterval i5(0,BoundType::WEAK, 5, BoundType::STRICT);
    
    std::vector<DoubleInterval> results;
    i5.split(results,5);
    EXPECT_EQ((unsigned)5, results.size());
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::STRICT), results.at(0));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), results.at(1));
    EXPECT_EQ(DoubleInterval(2, BoundType::WEAK, 3, BoundType::STRICT), results.at(2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 4, BoundType::STRICT), results.at(3));
    EXPECT_EQ(DoubleInterval(4, BoundType::WEAK, 5, BoundType::STRICT), results.at(4));
}