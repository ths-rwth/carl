/**
 * Test cases for the double instanciation of the generic interval class.
 * @file Test_MpqInterval.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2014-01-10
 * @version 2014-01-10
 */


#include "gtest/gtest.h"
#include "../../carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <gmpxx.h>

using namespace carl;

typedef Interval<mpq_class> MpqInterval;

TEST(MpqInterval, Constructor)
{
    MpqInterval test1 = MpqInterval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    MpqInterval test2 = MpqInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
	MpqInterval test3 = MpqInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(MpqInterval(1, BoundType::WEAK, -1, BoundType::WEAK), MpqInterval::emptyInterval());
    MpqInterval test5 = MpqInterval::unboundedInterval();
    MpqInterval test6 = MpqInterval::emptyInterval();
	/*
    MpqInterval test7 = MpqInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);
    */
	MpqInterval test8 = MpqInterval(2, BoundType::STRICT, mpq_class(0), BoundType::INFTY);
    SUCCEED();
}

TEST(MpqInterval, Getters)
{
    MpqInterval test1 = MpqInterval(mpq_class(-1.0), BoundType::WEAK, mpq_class(1.0), BoundType::STRICT);
    EXPECT_EQ(-1, test1.lower());
    EXPECT_EQ(1, test1.upper());
    EXPECT_EQ(BoundType::WEAK, test1.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test1.upperBoundType());
    MpqInterval test2 = MpqInterval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    EXPECT_EQ(-1, test2.lower());
    EXPECT_EQ(1, test2.upper());
    EXPECT_EQ(BoundType::WEAK, test2.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test2.upperBoundType());
	/*
    MpqInterval test3 = MpqInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::STRICT);
    EXPECT_EQ(-1, test3.lower());
    EXPECT_EQ(1, test3.upper());
    EXPECT_EQ(BoundType::WEAK, test3.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test3.upperBoundType());
    MpqInterval test4 = MpqInterval((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
    EXPECT_EQ(-1, test4.lower());
    EXPECT_EQ(1, test4.upper());
    EXPECT_EQ(BoundType::WEAK, test4.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test4.upperBoundType());
	 */
}

TEST(MpqInterval, Addition)
{
    MpqInterval a0 = MpqInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    MpqInterval a1 = MpqInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    MpqInterval a2 = MpqInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    MpqInterval a3 = MpqInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    MpqInterval b0 = MpqInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    MpqInterval b1 = MpqInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    MpqInterval b2 = MpqInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    MpqInterval b3 = MpqInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    MpqInterval result;
    
    result = a0.add(b0);
    EXPECT_EQ( MpqInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b1);
    EXPECT_EQ( MpqInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b2);
	EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a0.add(b3);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    
    result = a1.add(b0);
    EXPECT_EQ( MpqInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a1.add(b1);
	EXPECT_EQ( MpqInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( MpqInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    
    result = a2.add(b0);
	EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a2.add(b1);
    EXPECT_EQ( MpqInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( MpqInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b3);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    
    result = a3.add(b0);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a3.add(b1);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a3.add(b2);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a3.add(b3);
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
}


TEST(MpqInterval, Subtraction)
{
    MpqInterval a0 = MpqInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    MpqInterval a1 = MpqInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    MpqInterval a2 = MpqInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    MpqInterval a3 = MpqInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    MpqInterval b0 = MpqInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    MpqInterval b1 = MpqInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    MpqInterval b2 = MpqInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    MpqInterval b3 = MpqInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);
    
    MpqInterval result;
	result = a0.add(b0.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a0.add(b1.inverse());
    EXPECT_EQ( MpqInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b2.inverse());
    EXPECT_EQ( MpqInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b3.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    
    result = a1.add(b0.inverse());
    EXPECT_EQ( MpqInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a1.add(b1.inverse());
    EXPECT_EQ( MpqInterval(-3, BoundType::WEAK, 3, BoundType::WEAK), result);
    result = a1.add(b2.inverse());
    EXPECT_EQ( MpqInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a1.add(b3.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    
    result = a2.add(b0.inverse());
    EXPECT_EQ( MpqInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b1.inverse());
    EXPECT_EQ( MpqInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b2.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a2.add(b3.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    
    result = a3.add(b0.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a3.add(b1.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a3.add(b2.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
    result = a3.add(b3.inverse());
    EXPECT_EQ( MpqInterval::unboundedInterval(), result);
}


TEST(MpqInterval, Multiplication)
{
    MpqInterval c0  = MpqInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    MpqInterval c1  = MpqInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    MpqInterval c2  = MpqInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    MpqInterval c3  = MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK );
    MpqInterval c4  = MpqInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    MpqInterval c5  = MpqInterval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    MpqInterval c6  = MpqInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    MpqInterval c7  = MpqInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    MpqInterval c8  = MpqInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    MpqInterval d0  = MpqInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    MpqInterval d1  = MpqInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    MpqInterval d2  = MpqInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    MpqInterval d3  = MpqInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    MpqInterval d4  = MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK );
    MpqInterval d5  = MpqInterval( -1, BoundType::WEAK, mpq_class(0), BoundType::WEAK );
    MpqInterval d6  = MpqInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    MpqInterval d7  = MpqInterval( mpq_class(0), BoundType::WEAK, 1, BoundType::WEAK );
    MpqInterval d8  = MpqInterval( -1, BoundType::INFTY, mpq_class(0), BoundType::WEAK );
    MpqInterval d9  = MpqInterval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    MpqInterval d10 = MpqInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    MpqInterval d11 = MpqInterval( mpq_class(0), BoundType::WEAK, 1, BoundType::INFTY );
    MpqInterval d12 = MpqInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    MpqInterval result;

    result = c0.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.upper() * d0.upper()), BoundType::WEAK, mpq_class(c0.lower() * d0.lower()), BoundType::WEAK ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.upper() * d5.upper()), BoundType::WEAK, mpq_class(c0.lower() * d5.lower()), BoundType::WEAK ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(c1.upper() * d0.lower()), BoundType::WEAK, mpq_class(c1.lower() * d0.lower()), BoundType::WEAK ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(c1.upper() * d5.lower()), BoundType::WEAK, mpq_class(c1.lower() * d5.lower()), BoundType::WEAK ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.upper() * d0.lower()), BoundType::WEAK, mpq_class(c2.lower() * d0.upper()), BoundType::WEAK ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.upper() * d5.lower()), BoundType::WEAK, mpq_class(c2.lower() * d5.upper()), BoundType::WEAK ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(c4.upper() * d0.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(c4.upper() * d5.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(c5.upper() * d0.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(c5.upper() * d5.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c6.lower() * d0.lower()), BoundType::WEAK ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c6.lower() * d5.lower()), BoundType::WEAK ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c7.lower() * d0.upper()), BoundType::WEAK ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c7.lower() * d5.upper()), BoundType::WEAK ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d6 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.lower() * d6.upper()), BoundType::WEAK, mpq_class(c0.lower() * d6.lower()), BoundType::WEAK ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( MpqInterval( ( mpq_class(c1.lower() * d6.upper()) < mpq_class(c1.upper() * d6.lower()) ? mpq_class(c1.lower() * d6.upper()) : mpq_class(c1.upper() * d6.lower())),
                                         BoundType::WEAK,
                                         mpq_class(c1.lower() * d6.lower()) > mpq_class(c1.upper() * d6.upper()) ? mpq_class(c1.lower() * d6.lower()) : mpq_class(c1.upper() * d6.upper()),
                                         BoundType::WEAK ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.upper() * d6.lower()), BoundType::WEAK, mpq_class(c2.upper() * d6.upper()), BoundType::WEAK ),
                          result );
    result = c3.mul( d6 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d6 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d6 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d6 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d6 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d6 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.lower() * d1.upper()), BoundType::WEAK, mpq_class(c0.upper() * d1.lower()), BoundType::WEAK ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.lower() * d7.upper()), BoundType::WEAK, mpq_class(c0.upper() * d7.lower()), BoundType::WEAK ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(c1.lower() * d1.upper()), BoundType::WEAK, mpq_class(c1.upper() * d1.upper()), BoundType::WEAK ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(c1.lower() * d7.upper()), BoundType::WEAK, mpq_class(c1.upper() * d7.upper()), BoundType::WEAK ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.lower() * d1.lower()), BoundType::WEAK, mpq_class(c2.upper() * d1.upper()), BoundType::WEAK ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.lower() * d7.lower()), BoundType::WEAK, mpq_class(c2.upper() * d7.upper()), BoundType::WEAK ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c4.upper() * d1.lower()), BoundType::WEAK ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c4.upper() * d7.lower()), BoundType::WEAK ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c5.upper() * d1.upper()), BoundType::WEAK ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c5.upper() * d7.upper()), BoundType::WEAK ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(c6.lower() * d1.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(c6.lower() * d7.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( MpqInterval( mpq_class(c7.lower() * d1.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( MpqInterval( mpq_class(c7.lower() * d7.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c8.mul( d1 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d7 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c1.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c2.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c3.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );;
    result = c5.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c6.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c7.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c8.mul( d4 );
    EXPECT_EQ( MpqInterval( 0 ), result );

    result = c0.mul( d2 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.upper() * d2.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.upper() * d8.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c2.mul( d2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c2.lower() * d2.upper()), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c2.lower() * d8.upper()), BoundType::WEAK ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( MpqInterval( mpq_class(c4.upper() * d2.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(c4.upper() * d8.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c7.lower() * d2.upper()), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c7.lower() * d8.upper()), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.lower() * d9.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(c0.lower() * d8.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c2.upper() * d9.upper()), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c2.upper() * d8.upper()), BoundType::WEAK ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c4.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d9 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d9 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d9 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d8 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c0.lower() * d11.lower()), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c0.lower() * d10.lower()), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.upper() * d11.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.upper() * d10.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c3.mul( d10 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result );
    result = c4.mul( d10 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d10 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d10 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d10 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c0.upper() * d11.lower()), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c0.upper() * d3.lower()), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.lower() * d11.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( MpqInterval( mpq_class(c2.lower() * d3.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c4.upper() * d11.lower()), BoundType::WEAK ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(c4.upper() * d3.lower()), BoundType::WEAK ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( MpqInterval( mpq_class(c7.lower() * d11.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( MpqInterval( mpq_class(c7.lower() * d3.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result );
    result = c8.mul( d11 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d3 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );

    result = c0.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c1.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c2.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c3.mul( d12 );
    EXPECT_EQ( MpqInterval( 0 ), result );
    result = c4.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c5.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c6.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c7.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
    result = c8.mul( d12 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result );
}

TEST(MpqInterval, ExtendedDivision)
{
    MpqInterval a0  = MpqInterval( mpq_class(-2), BoundType::WEAK, mpq_class(-1), BoundType::WEAK );
    MpqInterval a1  = MpqInterval( mpq_class(-1), BoundType::WEAK, mpq_class(1), BoundType::WEAK );
    MpqInterval a2  = MpqInterval( mpq_class(1), BoundType::WEAK, mpq_class(2), BoundType::WEAK );
    MpqInterval a3  = MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK );
    MpqInterval a4  = MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(-1), BoundType::WEAK );
    MpqInterval a5  = MpqInterval( mpq_class(-2), BoundType::INFTY, mpq_class(1), BoundType::WEAK );
    MpqInterval a6  = MpqInterval( mpq_class(-1), BoundType::WEAK, mpq_class(1), BoundType::INFTY );
    MpqInterval a7  = MpqInterval( mpq_class(1), BoundType::WEAK, mpq_class(1), BoundType::INFTY );
    MpqInterval a8  = MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY );

    MpqInterval b0  = MpqInterval( mpq_class(-2), BoundType::WEAK, mpq_class(-1), BoundType::WEAK );
    MpqInterval b1  = MpqInterval( mpq_class(1), BoundType::WEAK, mpq_class(2), BoundType::WEAK );
    MpqInterval b2  = MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(-1), BoundType::WEAK );
    MpqInterval b3  = MpqInterval( mpq_class(1), BoundType::WEAK, mpq_class(1), BoundType::INFTY );

    MpqInterval b4  = MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK );
    MpqInterval b5  = MpqInterval( mpq_class(-1), BoundType::WEAK, mpq_class(0), BoundType::WEAK );
    MpqInterval b6  = MpqInterval( mpq_class(-1), BoundType::WEAK, mpq_class(1), BoundType::WEAK );
    MpqInterval b7  = MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::WEAK );
    MpqInterval b8  = MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK );
    MpqInterval b9  = MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::WEAK );
    MpqInterval b10 = MpqInterval( mpq_class(-1), BoundType::WEAK, mpq_class(1), BoundType::INFTY );
    MpqInterval b11 = MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY );
    MpqInterval b12 = MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY );
    MpqInterval result1, result2;
    // Table 7 Tests: Division without containing 0
    a0.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a0.upper() / b0.lower()), BoundType::WEAK, mpq_class(a0.lower() / b0.upper()), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a1.upper() / b0.upper()), BoundType::WEAK, mpq_class(a1.lower() / b0.upper()), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a2.upper() / b0.upper()), BoundType::WEAK, mpq_class(a2.lower() / b0.lower()), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result1 );
    a4.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a4.upper() / b0.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a5.upper() / b0.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a6.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a6.lower() / b0.upper()), BoundType::WEAK ), result1 );
    a7.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a7.lower() / b0.lower()), BoundType::WEAK ), result1 );
    a8.div_ext( b0, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY ), result1 );

    a0.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a0.lower() / b1.lower()), BoundType::WEAK, mpq_class(a0.upper() / b1.upper()), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a1.lower() / b1.lower()), BoundType::WEAK, mpq_class(a1.upper() / b1.lower()), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a2.lower() / b1.upper()), BoundType::WEAK, mpq_class(a2.upper() / b1.lower()), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result1 );
    a4.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a4.upper() / b1.upper()), BoundType::WEAK ), result1 );
    a5.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a5.upper() / b1.lower()), BoundType::WEAK ), result1 );
    a6.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a6.lower() / b1.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a7.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a7.lower() / b1.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a8.div_ext( b1, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY ), result1 );

    a0.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(a0.lower() / b2.upper()), BoundType::WEAK ), result1 );
    a1.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a1.upper() / b2.upper()), BoundType::WEAK, mpq_class(a1.lower() / b2.upper()), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a2.upper() / b2.upper()), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result1 );
    a3.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result1 );
    a4.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a5.upper() / b2.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a6.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a6.lower() / b2.upper()), BoundType::WEAK ), result1 );
    a7.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    a8.div_ext( b2, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY ), result1 );

    a0.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a0.lower() / b3.lower()), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result1 );
    a1.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a1.lower() / b3.lower()), BoundType::WEAK, mpq_class(a1.upper() / b3.lower()), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(a2.upper() / b3.lower()), BoundType::WEAK ), result1 );
    a3.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(0), BoundType::WEAK ), result1 );
    a4.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    a5.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a5.upper() / b3.lower()), BoundType::WEAK ), result1 );
    a6.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a6.lower() / b3.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a7.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a8.div_ext( b3, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY ), result1 );

    //Table 8 tests with division containin 0
    a0.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::emptyInterval(), result1 );
    a1.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::emptyInterval(), result1 );

    a4.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::emptyInterval(), result1 );
    a5.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::emptyInterval(), result1 );
    a8.div_ext( b4, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a0.upper() / b5.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a1.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a2.lower() / b5.lower()), BoundType::WEAK ), result1 );

    a4.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a4.upper() / b5.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a7.lower() / b5.lower()), BoundType::WEAK ), result1 );
    a8.div_ext( b5, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a0.upper() / b6.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a0.upper() / b6.upper()), BoundType::WEAK ), result2 );
    a1.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a2.lower() / b6.lower()), BoundType::WEAK ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(a2.lower() / b6.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result2 );

    a4.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a4.upper() / b6.upper()), BoundType::WEAK ), result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a4.upper() / b6.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a7.lower() / b6.lower()), BoundType::WEAK ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(a7.lower() / b6.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result2 );
    a8.div_ext( b6, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a0.upper() / b7.upper()), BoundType::WEAK ), result1 );
    a1.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a2.lower() / b7.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );

    a4.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a4.upper() / b7.upper()), BoundType::WEAK ), result1 );
    a5.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a7.lower() / b7.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a8.div_ext( b7, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a1.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );

    a4.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    a8.div_ext( b8, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a0.upper() / b9.upper()), BoundType::WEAK ), result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a1.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(a2.lower() / b9.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result2 );

    a4.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a4.upper() / b9.upper()), BoundType::WEAK ), result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(a7.lower() / b9.upper()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result2 );
    a8.div_ext( b9, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a0.upper() / b10.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a1.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(a2.lower() / b10.lower()), BoundType::WEAK ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result2 );

    a4.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result2 );
    EXPECT_EQ( MpqInterval( mpq_class(a4.upper() / b10.lower()), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a5.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, a7.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result2 );
    a8.div_ext( b10, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    a1.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );

    a4.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(-1), BoundType::INFTY, mpq_class(0), BoundType::WEAK ), result1 );
    a5.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval( mpq_class(0), BoundType::WEAK, mpq_class(1), BoundType::INFTY ), result1 );
    a8.div_ext( b11, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a0.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a1.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a2.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );

    a4.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a5.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a6.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a7.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
    a8.div_ext( b12, result1, result2 );
    EXPECT_EQ( MpqInterval::unboundedInterval(), result1 );
}

/*
TEST(MpqInterval, Intersection)
{
    MpqInterval a1(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK);
    
    MpqInterval b01(2,BoundType::WEAK,3,BoundType::WEAK);
    MpqInterval b02(1,BoundType::WEAK,3,BoundType::WEAK);
    MpqInterval b03(0,BoundType::WEAK,3,BoundType::WEAK);
    MpqInterval b04(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK);
    MpqInterval b05(mpq_class(-2),BoundType::WEAK,0,BoundType::WEAK);
    
    MpqInterval b06(mpq_class(-2),BoundType::WEAK,mpq_class(-1),BoundType::WEAK);
    MpqInterval b07(-3,BoundType::WEAK,mpq_class(-2),BoundType::WEAK);
    MpqInterval b08(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK);
    MpqInterval b09(1,BoundType::WEAK,1,BoundType::WEAK);
    MpqInterval b10(0,BoundType::WEAK,1,BoundType::WEAK);
    
    MpqInterval b11(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK);
    MpqInterval b12(-0.5,BoundType::WEAK,0.5,BoundType::WEAK);
    MpqInterval b13(mpq_class(-2),BoundType::WEAK,2,BoundType::WEAK);
    MpqInterval b14(0,BoundType::STRICT,0,BoundType::STRICT);
    MpqInterval b15(mpq_class(-1),BoundType::INFTY,1,BoundType::INFTY);
    
    MpqInterval b16(mpq_class(-1),BoundType::INFTY,mpq_class(-1),BoundType::WEAK);
    MpqInterval b17(mpq_class(-1),BoundType::INFTY,0,BoundType::WEAK);
    MpqInterval b18(mpq_class(-1),BoundType::INFTY,1,BoundType::WEAK);
    MpqInterval b19(mpq_class(-1),BoundType::WEAK,1,BoundType::INFTY);
    MpqInterval b20(0,BoundType::WEAK,1,BoundType::INFTY);
    
    MpqInterval b21(1,BoundType::WEAK,1,BoundType::INFTY);
    
    EXPECT_EQ(MpqInterval::emptyInterval(), a1.intersect(b01));
    EXPECT_EQ(MpqInterval(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b02));
    EXPECT_EQ(MpqInterval(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b03));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b04));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b05));
    
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK), a1.intersect(b06));
    EXPECT_EQ(MpqInterval::emptyInterval(), a1.intersect(b07));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK), a1.intersect(b08));
    EXPECT_EQ(MpqInterval(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b09));
    EXPECT_EQ(MpqInterval(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b10));
    
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b11));
    EXPECT_EQ(MpqInterval(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), a1.intersect(b12));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b13));
    EXPECT_EQ(MpqInterval::emptyInterval(), a1.intersect(b14));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b15));
    
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK), a1.intersect(b16));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK), a1.intersect(b17));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b18));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b19));
    EXPECT_EQ(MpqInterval(0,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b20));
    
    EXPECT_EQ(MpqInterval(1,BoundType::WEAK,1,BoundType::WEAK), a1.intersect(b21));
}
 */
TEST(MpqInterval, Split)
{
    MpqInterval i1(mpq_class(-1), BoundType::INFTY, mpq_class(1), BoundType::INFTY);
    MpqInterval i2(mpq_class(-1), BoundType::STRICT, mpq_class(1), BoundType::STRICT);
    MpqInterval i3(mpq_class(-1), BoundType::WEAK, mpq_class(1), BoundType::WEAK);
    MpqInterval i4(0, BoundType::STRICT, mpq_class(0), BoundType::STRICT);
    
	std::pair<MpqInterval, MpqInterval> res;
    
	res = i1.split();
    EXPECT_EQ(MpqInterval(0, BoundType::INFTY, mpq_class(0), BoundType::STRICT), res.first);
    EXPECT_EQ(MpqInterval(0, BoundType::WEAK, mpq_class(0), BoundType::INFTY), res.second);
    
    res = i2.split();
    EXPECT_EQ(MpqInterval(mpq_class(-1), BoundType::STRICT, mpq_class(0), BoundType::STRICT), res.first);
    EXPECT_EQ(MpqInterval(0, BoundType::WEAK, mpq_class(1), BoundType::STRICT), res.second);
    
    res = i3.split();
    EXPECT_EQ(MpqInterval(mpq_class(-1), BoundType::WEAK, mpq_class(0), BoundType::STRICT), res.first);
    EXPECT_EQ(MpqInterval(0, BoundType::WEAK, mpq_class(1), BoundType::WEAK), res.second);
    
    res = i4.split();
    EXPECT_EQ(MpqInterval(0, BoundType::STRICT, mpq_class(0), BoundType::STRICT), res.first);
    EXPECT_EQ(MpqInterval(0, BoundType::WEAK, mpq_class(0), BoundType::STRICT), res.second);
    
    // uniform multi-split
    MpqInterval i5(0,BoundType::WEAK, 5, BoundType::STRICT);
    
    std::list<MpqInterval> results;
    results = i5.split(5);
    EXPECT_EQ(5, results.size());
    EXPECT_EQ(MpqInterval(0, BoundType::WEAK, mpq_class(1), BoundType::STRICT), *results.begin());
	results.pop_front();
    EXPECT_EQ(MpqInterval(1, BoundType::WEAK, 2, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(MpqInterval(2, BoundType::WEAK, 3, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(MpqInterval(3, BoundType::WEAK, 4, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(MpqInterval(4, BoundType::WEAK, 5, BoundType::STRICT), *results.begin());
}