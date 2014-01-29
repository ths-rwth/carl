/**
 * Test cases for the double instanciation of the generic interval class.
 * @file Test_DoubleInterval.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2014-01-10
 * @version 2014-01-10
 */


#include "gtest/gtest.h"
#include "../../carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>

using namespace carl;

typedef Interval<double> DoubleInterval;

TEST(DoubleInterval, Constructor)
{
    DoubleInterval test1 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    DoubleInterval test2 = DoubleInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
	DoubleInterval test3 = DoubleInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, -1, BoundType::WEAK), DoubleInterval::emptyInterval());
    DoubleInterval test5 = DoubleInterval::unboundedInterval();
    DoubleInterval test6 = DoubleInterval::emptyInterval();
	/*
    DoubleInterval test7 = DoubleInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);
    */
	DoubleInterval test8 = DoubleInterval(2, BoundType::STRICT, 0, BoundType::INFTY);
    SUCCEED();
}

TEST(DoubleInterval, Getters)
{
    DoubleInterval test1 = DoubleInterval(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    EXPECT_EQ(-1, test1.lower());
    EXPECT_EQ(1, test1.upper());
    EXPECT_EQ(BoundType::WEAK, test1.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test1.upperBoundType());
    DoubleInterval test2 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    EXPECT_EQ(-1, test2.lower());
    EXPECT_EQ(1, test2.upper());
    EXPECT_EQ(BoundType::WEAK, test2.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test2.upperBoundType());
	/*
    DoubleInterval test3 = DoubleInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::STRICT);
    EXPECT_EQ(-1, test3.lower());
    EXPECT_EQ(1, test3.upper());
    EXPECT_EQ(BoundType::WEAK, test3.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test3.upperBoundType());
    DoubleInterval test4 = DoubleInterval((cln::cl_RA)-1, BoundType::WEAK, (cln::cl_RA)1, BoundType::STRICT);
    EXPECT_EQ(-1, test4.lower());
    EXPECT_EQ(1, test4.upper());
    EXPECT_EQ(BoundType::WEAK, test4.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test4.upperBoundType());
	 */
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
    EXPECT_EQ( DoubleInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b1);
    EXPECT_EQ( DoubleInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b2);
	EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a0.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a1.add(b0);
    EXPECT_EQ( DoubleInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a1.add(b1);
	EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    
    result = a2.add(b0);
	EXPECT_EQ( DoubleInterval::unboundedInterval(), result);
    result = a2.add(b1);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
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
    EXPECT_EQ( DoubleInterval( c0.upper() * d0.upper(), BoundType::WEAK, c0.lower() * d0.lower(), BoundType::WEAK ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( DoubleInterval( c0.upper() * d5.upper(), BoundType::WEAK, c0.lower() * d5.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( DoubleInterval( c1.upper() * d0.lower(), BoundType::WEAK, c1.lower() * d0.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( DoubleInterval( c1.upper() * d5.lower(), BoundType::WEAK, c1.lower() * d5.lower(), BoundType::WEAK ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( DoubleInterval( c2.upper() * d0.lower(), BoundType::WEAK, c2.lower() * d0.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( DoubleInterval( c2.upper() * d5.lower(), BoundType::WEAK, c2.lower() * d5.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( DoubleInterval( c4.upper() * d0.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( DoubleInterval( c4.upper() * d5.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( DoubleInterval( c5.upper() * d0.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( DoubleInterval( c5.upper() * d5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c6.lower() * d0.lower(), BoundType::WEAK ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c6.lower() * d5.lower(), BoundType::WEAK ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c7.lower() * d0.upper(), BoundType::WEAK ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c7.lower() * d5.upper(), BoundType::WEAK ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d6 );
    EXPECT_EQ( DoubleInterval( c0.lower() * d6.upper(), BoundType::WEAK, c0.lower() * d6.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( DoubleInterval( ( c1.lower() * d6.upper() < c1.upper() * d6.lower() ? c1.lower() * d6.upper() : c1.upper() * d6.lower()),
                                         BoundType::WEAK,
                                         c1.lower() * d6.lower() > c1.upper() * d6.upper() ? c1.lower() * d6.lower() : c1.upper() * d6.upper(),
                                         BoundType::WEAK ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( DoubleInterval( c2.upper() * d6.lower(), BoundType::WEAK, c2.upper() * d6.upper(), BoundType::WEAK ),
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
    EXPECT_EQ( DoubleInterval( c0.lower() * d1.upper(), BoundType::WEAK, c0.upper() * d1.lower(), BoundType::WEAK ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( DoubleInterval( c0.lower() * d7.upper(), BoundType::WEAK, c0.upper() * d7.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( DoubleInterval( c1.lower() * d1.upper(), BoundType::WEAK, c1.upper() * d1.upper(), BoundType::WEAK ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( DoubleInterval( c1.lower() * d7.upper(), BoundType::WEAK, c1.upper() * d7.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( DoubleInterval( c2.lower() * d1.lower(), BoundType::WEAK, c2.upper() * d1.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( DoubleInterval( c2.lower() * d7.lower(), BoundType::WEAK, c2.upper() * d7.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c4.upper() * d1.lower(), BoundType::WEAK ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c4.upper() * d7.lower(), BoundType::WEAK ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c5.upper() * d1.upper(), BoundType::WEAK ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c5.upper() * d7.upper(), BoundType::WEAK ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( DoubleInterval( c6.lower() * d1.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( DoubleInterval( c6.lower() * d7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( DoubleInterval( c7.lower() * d1.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( DoubleInterval( c7.lower() * d7.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
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
    EXPECT_EQ( DoubleInterval( c0.upper() * d2.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.upper() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c2.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c2.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( DoubleInterval( c4.upper() * d2.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( DoubleInterval( c4.upper() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c7.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c7.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( DoubleInterval( c0.lower() * d9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.lower() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c2.upper() * d9.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c2.upper() * d8.upper(), BoundType::WEAK ), result );
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
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.lower() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.lower() * d10.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( DoubleInterval( c2.upper() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( DoubleInterval( c2.upper() * d10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
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
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( DoubleInterval( c2.lower() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( DoubleInterval( c2.lower() * d3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c4.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c4.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( DoubleInterval( c7.lower() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( DoubleInterval( c7.lower() * d3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
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
    a0.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b0.lower(), BoundType::WEAK, a0.lower() / b0.upper(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( a1.upper() / b0.upper(), BoundType::WEAK, a1.lower() / b0.upper(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( a2.upper() / b0.upper(), BoundType::WEAK, a2.lower() / b0.lower(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b0.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( a5.upper() / b0.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a6.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a6.lower() / b0.upper(), BoundType::WEAK ), result1 );
    a7.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b0.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b0, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.lower() / b1.lower(), BoundType::WEAK, a0.upper() / b1.upper(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( a1.lower() / b1.lower(), BoundType::WEAK, a1.upper() / b1.lower(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b1.upper(), BoundType::WEAK, a2.upper() / b1.lower(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b1.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a5.upper() / b1.lower(), BoundType::WEAK ), result1 );
    a6.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( a6.lower() / b1.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a7.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b1.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a8.div_ext( b1, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0.0, BoundType::WEAK, a0.lower() / b2.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( a1.upper() / b2.upper(), BoundType::WEAK, a1.lower() / b2.upper(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( a2.upper() / b2.upper(), BoundType::WEAK, 0.0, BoundType::WEAK ), result1 );
    a3.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( a5.upper() / b2.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a6.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a6.lower() / b2.upper(), BoundType::WEAK ), result1 );
    a7.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b2, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.lower() / b3.lower(), BoundType::WEAK, 0.0, BoundType::WEAK ), result1 );
    a1.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( a1.lower() / b3.lower(), BoundType::WEAK, a1.upper() / b3.lower(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0.0, BoundType::WEAK, a2.upper() / b3.lower(), BoundType::WEAK ), result1 );
    a3.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a5.upper() / b3.lower(), BoundType::WEAK ), result1 );
    a6.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( a6.lower() / b3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a7.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b3, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    //Table 8 tests with division containin 0
    a0.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );
    a1.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );

    a4.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );
    a5.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::emptyInterval(), result1 );
    a8.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a1.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a2.lower() / b5.lower(), BoundType::WEAK ), result1 );

    a4.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b5.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b6.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a0.upper() / b6.upper(), BoundType::WEAK ), result2 );
    a1.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a2.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b6.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );

    a4.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b6.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b6.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b6.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );
    a8.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a0.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );

    a4.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a8.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );

    a4.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a0.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );

    a4.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );
    a8.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a1.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a2.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a1.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a0.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a1.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a2.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );

    a4.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a5.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a6.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a7.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
    a8.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unboundedInterval(), result1 );
}

/*
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
 */
TEST(DoubleInterval, Split)
{
    DoubleInterval i1(-1, BoundType::INFTY, 1, BoundType::INFTY);
    DoubleInterval i2(-1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval i3(-1, BoundType::WEAK, 1, BoundType::WEAK);
    DoubleInterval i4(0, BoundType::STRICT, 0, BoundType::STRICT);
    
	std::pair<DoubleInterval, DoubleInterval> res;
    
	res = i1.split();
    EXPECT_EQ(DoubleInterval(0, BoundType::INFTY, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 0, BoundType::INFTY), res.second);
    
    res = i2.split();
    EXPECT_EQ(DoubleInterval(-1, BoundType::STRICT, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::STRICT), res.second);
    
    res = i3.split();
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::WEAK), res.second);
    
    res = i4.split();
    EXPECT_EQ(DoubleInterval(0, BoundType::STRICT, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 0, BoundType::STRICT), res.second);
    
    // uniform multi-split
    DoubleInterval i5(0,BoundType::WEAK, 5, BoundType::STRICT);
    
    std::list<DoubleInterval> results;
    results = i5.split(5);
    EXPECT_EQ(5, results.size());
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::STRICT), *results.begin());
	results.pop_front();
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(2, BoundType::WEAK, 3, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 4, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(4, BoundType::WEAK, 5, BoundType::STRICT), *results.begin());
}