/**
 * Test cases for the mpq_class instanciation of the generic interval class.
 * @file Test_MpqInterval.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @author Benedikt Seidl
 *
 * @since	2014-01-31
 * @version 2014-11-11
 */

#include <gtest/gtest.h>
#include "carl/interval/Interval.h"
#include "carl/interval/set_theory.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <carl/numbers/numbers.h>

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
	
    MpqInterval test7 = MpqInterval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);
    
	MpqInterval test8 = MpqInterval(2, BoundType::STRICT, mpq_class(0), BoundType::INFTY);
	MpqInterval test9 = MpqInterval(1);
	MpqInterval test10 = MpqInterval(0);
    SUCCEED();
}

TEST(MpqInterval, Getters)
{
    MpqInterval test1 = MpqInterval(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    MpqInterval test2 = MpqInterval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    MpqInterval test3 = MpqInterval(1, BoundType::STRICT, 1, BoundType::STRICT);
    MpqInterval test4 = MpqInterval(4, BoundType::WEAK, 2, BoundType::WEAK);
    MpqInterval test5 = MpqInterval();
    MpqInterval test6 = MpqInterval(4);
	MpqInterval test7 = MpqInterval(1);
	MpqInterval test8 = MpqInterval(0);
    
    EXPECT_EQ(-1, test1.lower());
    EXPECT_EQ(1, test1.upper());
    EXPECT_EQ(BoundType::WEAK, test1.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test1.upperBoundType());
    EXPECT_EQ(-1, test2.lower());
    EXPECT_EQ(1, test2.upper());
    EXPECT_EQ(BoundType::WEAK, test2.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test2.upperBoundType());
    EXPECT_TRUE(test3.isEmpty());
    EXPECT_EQ(BoundType::STRICT, test4.lowerBoundType());
    EXPECT_EQ(BoundType::STRICT, test4.upperBoundType());
    EXPECT_EQ(0, test4.lower());
    EXPECT_EQ(0, test4.upper());
    EXPECT_TRUE(test4.isEmpty());
    EXPECT_EQ(0, test5.lower());
    EXPECT_EQ(0, test5.upper());
    EXPECT_TRUE(test5.isEmpty());
    EXPECT_EQ(4, test6.lower());
    EXPECT_EQ(4, test6.upper());
    
    test1.setLower(-3);
    test1.setUpper(5);
    test1.setLowerBoundType(BoundType::STRICT);
    test1.setUpperBoundType(BoundType::WEAK);
    EXPECT_EQ(-3, test1.lower());
    EXPECT_EQ(5, test1.upper());
    EXPECT_EQ(BoundType::STRICT, test1.lowerBoundType());
    EXPECT_EQ(BoundType::WEAK, test1.upperBoundType());
    
    test1.set(4, 8);
    EXPECT_EQ(4, test1.lower());
    EXPECT_EQ(8, test1.upper());
    
    test1.setLowerBoundType(BoundType::INFTY);
    test1.setUpperBoundType(BoundType::INFTY);
    EXPECT_TRUE(test1.isInfinite());

    test2.setUpperBoundType(BoundType::INFTY);
    EXPECT_EQ(BoundType::INFTY, test2.upperBoundType());
    EXPECT_EQ(test2.lower(), test2.upper());
    
    test1.set(MpqInterval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());
    
    test1 = MpqInterval();
    test1.set(MpqInterval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());
    
	EXPECT_TRUE(test7.isOne());
	EXPECT_TRUE(isOne(test7));
	
	EXPECT_TRUE(test8.isZero());
	EXPECT_TRUE(isZero(test8));
}

TEST(MpqInterval, StaticCreators)
{
    MpqInterval i1 = MpqInterval::emptyInterval();
    MpqInterval i2 = MpqInterval::unboundedInterval();
	MpqInterval i3 = carl::constant_one<MpqInterval>().get();
	MpqInterval i4 = carl::constant_zero<MpqInterval>().get();
    
    EXPECT_TRUE(i1.isEmpty());
    EXPECT_EQ(0, i1.lower());
    EXPECT_EQ(0, i1.upper());
    
    EXPECT_TRUE(i2.isInfinite());
    EXPECT_EQ(BoundType::INFTY, i2.lowerBoundType());
    EXPECT_EQ(BoundType::INFTY, i2.upperBoundType());
	
	EXPECT_TRUE(i3.isOne());
	EXPECT_TRUE(isOne(i3));
	
	EXPECT_TRUE(i4.isZero());
	EXPECT_TRUE(isZero(i4));
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
    
    EXPECT_EQ(MpqInterval::emptyInterval(), carl::set_intersection(a1, b01));
    EXPECT_EQ(MpqInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b02));
    EXPECT_EQ(MpqInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b03));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b04));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b05));
    
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK), carl::set_intersection(a1, b06));
    EXPECT_EQ(MpqInterval::emptyInterval(), carl::set_intersection(a1, b07));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK), carl::set_intersection(a1, b08));
    EXPECT_EQ(MpqInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b09));
    EXPECT_EQ(MpqInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b10));
    
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b11));
    EXPECT_EQ(MpqInterval(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), carl::set_intersection(a1, b12));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b13));
    EXPECT_EQ(MpqInterval::emptyInterval(), carl::set_intersection(a1, b14));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b15));
    
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,mpq_class(-1),BoundType::WEAK), carl::set_intersection(a1, b16));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b17));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b18));
    EXPECT_EQ(MpqInterval(mpq_class(-1),BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b19));
    EXPECT_EQ(MpqInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b20));
    
    EXPECT_EQ(MpqInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b21));
}

TEST(MpqInterval, Union)
{
    MpqInterval i1(3, BoundType::WEAK, 5, BoundType::WEAK);
    MpqInterval i2(1, BoundType::WEAK, 4, BoundType::WEAK);
    MpqInterval i3(-2, BoundType::WEAK, 1, BoundType::WEAK);
    MpqInterval i4(4, BoundType::STRICT, 9, BoundType::STRICT);
    MpqInterval i5(1, BoundType::STRICT, 4, BoundType::STRICT);
    MpqInterval i6(3, BoundType::STRICT, 3, BoundType::INFTY);
    MpqInterval result1, result2;
    
    EXPECT_FALSE(carl::set_union(i1, i2, result1, result2));
    EXPECT_EQ(MpqInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);
    
    EXPECT_FALSE(carl::set_union(i2, i1, result1, result2));
    EXPECT_EQ(MpqInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);
    
    EXPECT_TRUE(carl::set_union(i1, i3, result1, result2));
    EXPECT_EQ(MpqInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(MpqInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result2);
    
    EXPECT_FALSE(carl::set_union(i3, i2, result1, result2));
    EXPECT_EQ(MpqInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result1);
    
    EXPECT_FALSE(carl::set_union(i4, i1, result1, result2));
    EXPECT_EQ(MpqInterval(3, BoundType::WEAK, 9, BoundType::STRICT), result1);
    
    EXPECT_TRUE(carl::set_union(i3, i4, result1, result2));
    EXPECT_EQ(MpqInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(MpqInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);
    
    EXPECT_FALSE(carl::set_union(i2, i4, result1, result2));
    EXPECT_EQ(MpqInterval(1, BoundType::WEAK, 9, BoundType::STRICT), result1);
    
    EXPECT_FALSE(carl::set_union(i2, i5, result1, result2));
    EXPECT_EQ(MpqInterval(1, BoundType::WEAK, 4, BoundType::WEAK), result1);
    
    EXPECT_TRUE(carl::set_union(i5, i4, result1, result2));
    EXPECT_EQ(MpqInterval(1, BoundType::STRICT, 4, BoundType::STRICT), result1);
    EXPECT_EQ(MpqInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);
    
    EXPECT_FALSE(carl::set_union(i6, i1, result1, result2));
    EXPECT_EQ(MpqInterval(3, BoundType::WEAK, 3, BoundType::INFTY), result1);
    
    EXPECT_TRUE(carl::set_union(i6, i3, result1, result2));
    EXPECT_EQ(MpqInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(MpqInterval(3, BoundType::STRICT, 3, BoundType::INFTY), result2);
}

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

TEST(MpqInterval, Properties)
{
    MpqInterval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    MpqInterval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    MpqInterval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    MpqInterval i4(-5, BoundType::WEAK, 3, BoundType::WEAK);
    
    // Diameter
    EXPECT_EQ(4, i1.diameter());
    EXPECT_EQ(8, i2.diameter());
    EXPECT_EQ(4, i3.diameter());
    EXPECT_EQ(8, i4.diameter());
    
    // Diameter ratio
    EXPECT_EQ(0.5, i1.diameterRatio(i2));
    EXPECT_EQ(2, i2.diameterRatio(i1));
    EXPECT_EQ(0.5, i3.diameterRatio(i2));
    EXPECT_EQ(2, i4.diameterRatio(i1));
    
    // Magnitude
    EXPECT_EQ(7, i1.magnitude());
    EXPECT_EQ(5, i2.magnitude());
    EXPECT_EQ(7, i3.magnitude());
    EXPECT_EQ(5, i4.magnitude());
    
    // Center
    EXPECT_EQ(5, center(i1));
    EXPECT_EQ(-1, center(i2));
    EXPECT_EQ(5, center(i3));
    EXPECT_EQ(-1, center(i4));
}

TEST(MpqInterval, Contains)
{
    MpqInterval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    MpqInterval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    MpqInterval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    MpqInterval i4(-5, BoundType::WEAK, 3, BoundType::WEAK);
    MpqInterval i5(4, BoundType::STRICT, 5, BoundType::STRICT);
    MpqInterval i6(3, BoundType::WEAK, 7, BoundType::WEAK);
    MpqInterval i7(3, BoundType::STRICT, 4, BoundType::STRICT);
    MpqInterval i8(3, BoundType::WEAK, 3, BoundType::INFTY);
    
    // Contains number
    EXPECT_TRUE(i1.contains(mpq_class(4)));
    EXPECT_FALSE(i1.contains(mpq_class(2)));
    EXPECT_FALSE(i1.contains(mpq_class(12)));
    EXPECT_FALSE(i1.contains(mpq_class(3)));
    EXPECT_FALSE(i1.contains(mpq_class(7)));
    
    EXPECT_TRUE(i2.contains(mpq_class(-1)));
    EXPECT_FALSE(i2.contains(mpq_class(-13)));
    EXPECT_FALSE(i2.contains(mpq_class(6)));
    EXPECT_FALSE(i2.contains(mpq_class(-5)));
    EXPECT_TRUE(i2.contains(mpq_class(3)));
    
    EXPECT_TRUE(i3.contains(mpq_class(4)));
    EXPECT_FALSE(i3.contains(mpq_class(2)));
    EXPECT_FALSE(i3.contains(mpq_class(12)));
    EXPECT_TRUE(i3.contains(mpq_class(3)));
    EXPECT_FALSE(i3.contains(mpq_class(7)));
    
    EXPECT_TRUE(i4.contains(mpq_class(-1)));
    EXPECT_FALSE(i4.contains(mpq_class(-13)));
    EXPECT_FALSE(i4.contains(mpq_class(6)));
    EXPECT_TRUE(i4.contains(mpq_class(-5)));
    EXPECT_TRUE(i4.contains(mpq_class(3)));
    
    EXPECT_FALSE(i8.contains(mpq_class(1)));
    EXPECT_TRUE(i8.contains(mpq_class(3)));
    EXPECT_TRUE(i8.contains(mpq_class(4)));
    EXPECT_TRUE(i8.contains(mpq_class(100)));
    EXPECT_FALSE(i8.contains(mpq_class(-2)));
    
    // Contains interval
    EXPECT_FALSE(i1.contains(i2));
    EXPECT_FALSE(i2.contains(i1));
    EXPECT_FALSE(i1.contains(i3));
    EXPECT_TRUE(i3.contains(i1));
    EXPECT_TRUE(i1.contains(i5));
    EXPECT_FALSE(i5.contains(i1));
    EXPECT_FALSE(i1.contains(i6));
    EXPECT_TRUE(i6.contains(i1));
    EXPECT_TRUE(i1.contains(i1));
    EXPECT_TRUE(i6.contains(i6));
    EXPECT_TRUE(i1.contains(i7));
    EXPECT_FALSE(i7.contains(i1));
    EXPECT_TRUE(i6.contains(i7));
    EXPECT_FALSE(i7.contains(i6));
    EXPECT_TRUE(i8.contains(i5));
    EXPECT_FALSE(i5.contains(i8));
    EXPECT_TRUE(i8.contains(i6));
    EXPECT_FALSE(i6.contains(i8));
    EXPECT_FALSE(i8.contains(i4));
    EXPECT_FALSE(i4.contains(i8));
    
    // Subset is the opposite
    EXPECT_FALSE(i2.isSubset(i1));
    EXPECT_FALSE(i1.isSubset(i2));
    EXPECT_TRUE(i3.isSubset(i1));
    EXPECT_FALSE(i1.isSubset(i3));
    EXPECT_FALSE(i5.isSubset(i1));
    EXPECT_TRUE(i1.isSubset(i5));
    EXPECT_TRUE(i6.isSubset(i1));
    EXPECT_FALSE(i1.isSubset(i6));
    EXPECT_TRUE(i1.isSubset(i1));
    EXPECT_TRUE(i6.isSubset(i6));
    EXPECT_FALSE(i7.isSubset(i1));
    EXPECT_TRUE(i1.isSubset(i7));
    EXPECT_FALSE(i7.isSubset(i6));
    EXPECT_TRUE(i6.isSubset(i7));
    EXPECT_FALSE(i5.isSubset(i8));
    EXPECT_TRUE(i8.isSubset(i5));
    EXPECT_FALSE(i6.isSubset(i8));
    EXPECT_TRUE(i8.isSubset(i6));
    EXPECT_FALSE(i4.isSubset(i8));
    EXPECT_FALSE(i8.isSubset(i4));
    
    EXPECT_FALSE(i2.isProperSubset(i1));
    EXPECT_FALSE(i1.isProperSubset(i2));
    EXPECT_TRUE(i3.isProperSubset(i1));
    EXPECT_FALSE(i1.isProperSubset(i3));
    EXPECT_FALSE(i5.isProperSubset(i1));
    EXPECT_TRUE(i1.isProperSubset(i5));
    EXPECT_TRUE(i6.isProperSubset(i1));
    EXPECT_FALSE(i1.isProperSubset(i6));
    EXPECT_FALSE(i1.isProperSubset(i1));
    EXPECT_FALSE(i6.isProperSubset(i6));
    EXPECT_FALSE(i7.isProperSubset(i1));
    EXPECT_TRUE(i1.isProperSubset(i7));
    EXPECT_FALSE(i7.isProperSubset(i6));
    EXPECT_TRUE(i6.isProperSubset(i7));
    EXPECT_FALSE(i5.isProperSubset(i8));
    EXPECT_TRUE(i8.isProperSubset(i5));
    EXPECT_FALSE(i6.isProperSubset(i8));
    EXPECT_TRUE(i8.isProperSubset(i6));
    EXPECT_FALSE(i4.isProperSubset(i8));
    EXPECT_FALSE(i8.isProperSubset(i4));
}

TEST(MpqInterval, BloatShrink)
{
    MpqInterval i1(3, BoundType::WEAK, 7, BoundType::WEAK);
    MpqInterval i2(-13, BoundType::STRICT, 1, BoundType::STRICT);
    MpqInterval i3(0, BoundType::STRICT, 1, BoundType::STRICT);
    MpqInterval i4(5, BoundType::STRICT, 13, BoundType::STRICT);
    MpqInterval result1(-2, BoundType::WEAK, 12, BoundType::WEAK);
    MpqInterval result2(-10, BoundType::STRICT, -2, BoundType::STRICT);
    MpqInterval result4(7, BoundType::STRICT, 11, BoundType::STRICT);
    
    // Bloat by adding
    i1.bloat_by(5);
    EXPECT_EQ(result1, i1);
    i2.bloat_by(-3);
    EXPECT_EQ(result2, i2);
    
    // Shrink by subtracting
    i4.shrink_by(2);
    EXPECT_EQ(result4, i4);
}
