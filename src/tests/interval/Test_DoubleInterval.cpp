/**
 * Test cases for the double instanciation of the generic interval class.
 * @file Test_DoubleInterval.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @author Benedikt Seidl
 *
 * @since	2014-01-31
 * @version 2014-11-11
 */


#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include <iostream>
#include "carl/util/platform.h"

#include "../Common.h"

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

    DoubleInterval test7 = DoubleInterval((Rational)-1, BoundType::WEAK, (Rational)1, BoundType::WEAK);

	DoubleInterval test8 = DoubleInterval(2, BoundType::STRICT, 0, BoundType::INFTY);
	DoubleInterval test9 = DoubleInterval(1);
	DoubleInterval test10 = DoubleInterval(0);
    SUCCEED();
}

TEST(DoubleInterval, Hash)
{
    DoubleInterval test1 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    std::hash<DoubleInterval> hashFkt;
    size_t hash1 = hashFkt(test1);
    /*DoubleInterval test2 = DoubleInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval test3 = DoubleInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, -1, BoundType::WEAK), DoubleInterval::emptyInterval());
    DoubleInterval test5 = DoubleInterval::unboundedInterval();
    DoubleInterval test6 = DoubleInterval::emptyInterval();

    DoubleInterval test7 = DoubleInterval((Rational)-1, BoundType::WEAK, (Rational)1, BoundType::WEAK);

	DoubleInterval test8 = DoubleInterval(2, BoundType::STRICT, 0, BoundType::INFTY);
	DoubleInterval test9 = DoubleInterval(1);
	DoubleInterval test10 = DoubleInterval(0);
	*/
    SUCCEED();
}

TEST(DoubleInterval, Getters)
{
    DoubleInterval test1 = DoubleInterval(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    DoubleInterval test2 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    DoubleInterval test3 = DoubleInterval(1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval test4 = DoubleInterval(4, BoundType::WEAK, 2, BoundType::WEAK);
    DoubleInterval test5 = DoubleInterval();
    DoubleInterval test6 = DoubleInterval(4);
	DoubleInterval test7 = DoubleInterval(1);
	DoubleInterval test8 = DoubleInterval(0);

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

    test1.set(DoubleInterval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());

    test1 = DoubleInterval();
    test1.set(DoubleInterval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());

	EXPECT_TRUE(test7.isOne());
	EXPECT_TRUE(isOne(test7));

	EXPECT_TRUE(test8.isZero());
	EXPECT_TRUE(isZero(test8));
}

TEST(DoubleInterval, StaticCreators)
{
    DoubleInterval i1 = DoubleInterval::emptyInterval();
    DoubleInterval i2 = DoubleInterval::unboundedInterval();
	DoubleInterval i3 = carl::constant_one<DoubleInterval>().get();
	DoubleInterval i4 = carl::constant_zero<DoubleInterval>().get();

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

    result  = DoubleInterval( -2, BoundType::WEAK, -1, BoundType::STRICT ).mul(DoubleInterval( -2, BoundType::WEAK, -1, BoundType::STRICT ));
    EXPECT_EQ( DoubleInterval( 1, BoundType::STRICT, 4, BoundType::WEAK ), result );

    DoubleInterval e1 = DoubleInterval( 0.0, BoundType::INFTY, 0.0, BoundType::STRICT );
    DoubleInterval e2 = DoubleInterval( 0.0, BoundType::WEAK, 0.0, BoundType::INFTY );
    DoubleInterval e3 = DoubleInterval( -2.0, BoundType::STRICT, 0.0, BoundType::STRICT );
    DoubleInterval e4 = DoubleInterval( 0.0, BoundType::WEAK, 2.0, BoundType::STRICT );

    result = e1.mul( e2 );
    EXPECT_EQ( DoubleInterval( 0.0, BoundType::INFTY, 0.0, BoundType::WEAK ), result );
    result = e3.mul( e4 );
    EXPECT_EQ( DoubleInterval( -4.0, BoundType::STRICT, 0.0, BoundType::WEAK ), result );
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

TEST(DoubleInterval, Union)
{
    DoubleInterval i1(3, BoundType::WEAK, 5, BoundType::WEAK);
    DoubleInterval i2(1, BoundType::WEAK, 4, BoundType::WEAK);
    DoubleInterval i3(-2, BoundType::WEAK, 1, BoundType::WEAK);
    DoubleInterval i4(4, BoundType::STRICT, 9, BoundType::STRICT);
    DoubleInterval i5(1, BoundType::STRICT, 4, BoundType::STRICT);
    DoubleInterval i6(3, BoundType::STRICT, 3, BoundType::INFTY);
    DoubleInterval i7(0, BoundType::INFTY, 0, BoundType::INFTY);
    DoubleInterval result1, result2;

    EXPECT_FALSE(i1.unite(i2, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i2.unite(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_TRUE(i1.unite(i3, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result2);

    EXPECT_FALSE(i3.unite(i2, result1, result2));
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_FALSE(i4.unite(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_TRUE(i3.unite(i4, result1, result2));
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(i2.unite(i4, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_FALSE(i2.unite(i5, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_TRUE(i5.unite(i4, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::STRICT, 4, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(i6.unite(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 3, BoundType::INFTY), result1);

    EXPECT_TRUE(i6.unite(i3, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 3, BoundType::INFTY), result1);
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result2);

    EXPECT_FALSE(i1.unite(i7, result1, result2));
    EXPECT_EQ(DoubleInterval::unboundedInterval(), result1);

    EXPECT_FALSE(i7.unite(i6, result1, result2));
    EXPECT_EQ(DoubleInterval::unboundedInterval(), result1);
}

TEST(DoubleInterval, Difference)
{
    DoubleInterval i1(3, BoundType::WEAK, 5, BoundType::WEAK);
    DoubleInterval i2(1, BoundType::WEAK, 4, BoundType::WEAK);
    DoubleInterval i3(-1, BoundType::WEAK, 2, BoundType::WEAK);
    DoubleInterval i4(2, BoundType::WEAK, 3, BoundType::WEAK);
    DoubleInterval i5(1, BoundType::STRICT, 3, BoundType::STRICT);
    DoubleInterval i6(2, BoundType::WEAK, 2, BoundType::INFTY);
    DoubleInterval i7(3, BoundType::WEAK, 3, BoundType::WEAK);
    DoubleInterval i8(0, BoundType::STRICT, 0, BoundType::STRICT);
    DoubleInterval result1, result2;

    EXPECT_FALSE(i1.difference(i2, result1, result2));
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i2.difference(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 3, BoundType::STRICT), result1);

    EXPECT_FALSE(i1.difference(i3, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i3.difference(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);

    EXPECT_TRUE(i2.difference(i4, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(i4.difference(i2, result1, result2));
    EXPECT_EQ(DoubleInterval::emptyInterval(), result1);

    EXPECT_FALSE(i5.difference(i2, result1, result2));
    EXPECT_EQ(DoubleInterval::emptyInterval(), result1);

    EXPECT_TRUE(i2.difference(i5, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(i1.difference(i1, result1, result2));
    EXPECT_EQ(DoubleInterval::emptyInterval(), result1);

    EXPECT_FALSE(i6.difference(i2, result1, result2));
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 4, BoundType::INFTY), result1);

    EXPECT_FALSE(i2.difference(i6, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), result1);

    EXPECT_TRUE(i6.difference(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(2, BoundType::WEAK, 3, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(5, BoundType::STRICT, 5, BoundType::INFTY), result2);

    EXPECT_FALSE(i1.difference(i6, result1, result2));
    EXPECT_EQ(DoubleInterval::emptyInterval(), result1);

    EXPECT_FALSE(i3.difference(i6, result1, result2));
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 2, BoundType::STRICT), result1);

    EXPECT_FALSE(i7.difference(i2, result1, result2));
    EXPECT_EQ(DoubleInterval::emptyInterval(), result1);

    EXPECT_TRUE(i2.difference(i7, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 3, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(i1.difference(i7, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i8.difference(i1, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i1.difference(i8, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i8.difference(i3, result1, result2));
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);

    EXPECT_FALSE(i3.difference(i8, result1, result2));
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);
}

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
    EXPECT_EQ((unsigned)5, results.size());
    EXPECT_EQ(DoubleInterval(0, BoundType::WEAK, 1, BoundType::STRICT), *results.begin());
	results.pop_front();
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(2, BoundType::WEAK, 3, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 4, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(4, BoundType::WEAK, 5, BoundType::STRICT), *results.begin());

    DoubleInterval i6(12,BoundType::WEAK, 20, BoundType::WEAK);
    results = i6.split(8);
    EXPECT_EQ((unsigned)8, results.size());
    EXPECT_EQ(DoubleInterval(12, BoundType::WEAK, 13, BoundType::STRICT), *results.begin());
	results.pop_front();
    EXPECT_EQ(DoubleInterval(13, BoundType::WEAK, 14, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(14, BoundType::WEAK, 15, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(15, BoundType::WEAK, 16, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(16, BoundType::WEAK, 17, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(17, BoundType::WEAK, 18, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(18, BoundType::WEAK, 19, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(DoubleInterval(19, BoundType::WEAK, 20, BoundType::WEAK), *results.begin());
}

TEST(DoubleInterval, Properties)
{
    DoubleInterval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    DoubleInterval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    DoubleInterval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    DoubleInterval i4(-5, BoundType::WEAK, 3, BoundType::WEAK);

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
    EXPECT_EQ(5, i1.center());
    EXPECT_EQ(-1, i2.center());
    EXPECT_EQ(5, i3.center());
    EXPECT_EQ(-1, i4.center());
}

TEST(DoubleInterval, Contains)
{
    DoubleInterval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    DoubleInterval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    DoubleInterval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    DoubleInterval i4(-5, BoundType::WEAK, 4, BoundType::WEAK);
    DoubleInterval i5(4, BoundType::STRICT, 5, BoundType::STRICT);
    DoubleInterval i6(3, BoundType::WEAK, 7, BoundType::WEAK);
    DoubleInterval i7(3, BoundType::STRICT, 4, BoundType::STRICT);
    DoubleInterval i8(3, BoundType::WEAK, 3, BoundType::INFTY);

    // Contains number
    EXPECT_TRUE(i1.contains(4));
    EXPECT_FALSE(i1.contains(2));
    EXPECT_FALSE(i1.contains(12));
    EXPECT_FALSE(i1.contains(3));
    EXPECT_FALSE(i1.contains(7));

    EXPECT_TRUE(i2.contains(-1));
    EXPECT_FALSE(i2.contains(-13));
    EXPECT_FALSE(i2.contains(6));
    EXPECT_FALSE(i2.contains(-5));
    EXPECT_TRUE(i2.contains(3));

    EXPECT_TRUE(i3.contains(4));
    EXPECT_FALSE(i3.contains(2));
    EXPECT_FALSE(i3.contains(12));
    EXPECT_TRUE(i3.contains(3));
    EXPECT_FALSE(i3.contains(7));

    EXPECT_TRUE(i4.contains(-1));
    EXPECT_FALSE(i4.contains(-13));
    EXPECT_FALSE(i4.contains(6));
    EXPECT_TRUE(i4.contains(-5));
    EXPECT_TRUE(i4.contains(4));

    EXPECT_FALSE(i8.contains(1));
    EXPECT_TRUE(i8.contains(3));
    EXPECT_TRUE(i8.contains(4));
    EXPECT_TRUE(i8.contains(100));
    EXPECT_FALSE(i8.contains(-2));

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
    EXPECT_FALSE(i3.isSubset(i1));
    EXPECT_TRUE(i1.isSubset(i3));
    EXPECT_TRUE(i5.isSubset(i1));
    EXPECT_FALSE(i1.isSubset(i5));
    EXPECT_FALSE(i6.isSubset(i1));
    EXPECT_TRUE(i1.isSubset(i6));
    EXPECT_TRUE(i1.isSubset(i1));
    EXPECT_TRUE(i6.isSubset(i6));
    EXPECT_TRUE(i7.isSubset(i1));
    EXPECT_FALSE(i1.isSubset(i7));
    EXPECT_TRUE(i7.isSubset(i6));
    EXPECT_FALSE(i6.isSubset(i7));
    EXPECT_TRUE(i5.isSubset(i8));
    EXPECT_FALSE(i8.isSubset(i5));
    EXPECT_TRUE(i6.isSubset(i8));
    EXPECT_FALSE(i8.isSubset(i6));
    EXPECT_FALSE(i4.isSubset(i8));
    EXPECT_FALSE(i8.isSubset(i4));

    EXPECT_FALSE(i2.isProperSubset(i1));
    EXPECT_FALSE(i1.isProperSubset(i2));
    EXPECT_FALSE(i3.isProperSubset(i1));
    EXPECT_TRUE(i1.isProperSubset(i3));
    EXPECT_TRUE(i5.isProperSubset(i1));
    EXPECT_FALSE(i1.isProperSubset(i5));
    EXPECT_FALSE(i6.isProperSubset(i1));
    EXPECT_TRUE(i1.isProperSubset(i6));
    EXPECT_TRUE(i1.isProperSubset(i1));
    EXPECT_TRUE(i6.isProperSubset(i6));
    EXPECT_TRUE(i7.isProperSubset(i1));
    EXPECT_FALSE(i1.isProperSubset(i7));
    EXPECT_TRUE(i7.isProperSubset(i6));
    EXPECT_FALSE(i6.isProperSubset(i7));
    EXPECT_TRUE(i5.isProperSubset(i8));
    EXPECT_FALSE(i8.isProperSubset(i5));
    EXPECT_TRUE(i6.isProperSubset(i8));
    EXPECT_FALSE(i8.isProperSubset(i6));
    EXPECT_FALSE(i4.isProperSubset(i8));
    EXPECT_FALSE(i8.isProperSubset(i4));
}

TEST(DoubleInterval, BloatShrink)
{
    DoubleInterval i1(3, BoundType::WEAK, 7, BoundType::WEAK);
    DoubleInterval i2(-13, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval i3(0, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval i4(5, BoundType::STRICT, 13, BoundType::STRICT);
    DoubleInterval result1(-2, BoundType::WEAK, 12, BoundType::WEAK);
    DoubleInterval result2(-10, BoundType::STRICT, -2, BoundType::STRICT);
    DoubleInterval result4(7, BoundType::STRICT, 11, BoundType::STRICT);

    // Bloat by adding
    i1.bloat_by(5);
    EXPECT_EQ(result1, i1);
    i2.bloat_by(-3);
    EXPECT_EQ(result2, i2);

    // Shrink by subtracting
    i4.shrink_by(2);
    EXPECT_EQ(result4, i4);
}
