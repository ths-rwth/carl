/**
 * Test cases for the mpfr instanciation of the generic interval class.
 * @file Test_MpfrInterval.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since	2014-11-11
 * @version 2014-11-11
 */


#include "gtest/gtest.h"
#include "../../carl/numbers/numbers.h"
#include "../../carl/interval/Interval.h"
#include "carl/interval/set_theory.h"
#include "carl/core/VariablePool.h"
#include <iostream>
#include <carl-common/meta/platform.h>

#include "../Common.h"

#ifdef USE_MPFR_FLOAT

using namespace carl;

typedef Interval<FLOAT_T<mpfr_t>> mpfrInterval;

TEST(mpfrInterval, Constructor)
{
    mpfrInterval test1 = mpfrInterval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    mpfrInterval test2 = mpfrInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
    mpfrInterval test3 = mpfrInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, -1, BoundType::WEAK), mpfrInterval::emptyInterval());
    mpfrInterval test5 = mpfrInterval::unboundedInterval();
    mpfrInterval test6 = mpfrInterval::emptyInterval();
	/*
    mpfrInterval test7 = mpfrInterval((Rational)-1, BoundType::WEAK, (Rational)1, BoundType::WEAK);
    */
	mpfrInterval test8 = mpfrInterval(2, BoundType::STRICT, 0, BoundType::INFTY);
	mpfrInterval test9 = mpfrInterval(1);
	mpfrInterval test10 = mpfrInterval(0);
    SUCCEED();
}

TEST(mpfrInterval, Getters)
{
    mpfrInterval test1 = mpfrInterval(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    mpfrInterval test2 = mpfrInterval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    mpfrInterval test3 = mpfrInterval(1, BoundType::STRICT, 1, BoundType::STRICT);
    mpfrInterval test4 = mpfrInterval(4, BoundType::WEAK, 2, BoundType::WEAK);
    mpfrInterval test5 = mpfrInterval();
    mpfrInterval test6 = mpfrInterval(4);
	mpfrInterval test7 = mpfrInterval(1);
	mpfrInterval test8 = mpfrInterval(0);

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

    test1.set(mpfrInterval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());

    test1 = mpfrInterval();
    test1.set(mpfrInterval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());

    test2 = mpfrInterval();
    mpfrInterval::BoostInterval bi(0, 1);
    bi = boost::numeric::widen(bi, FLOAT_T<mpfr_t>(-3)); // create an invalid interval by this hack
    test2.set(bi);
    EXPECT_EQ(0, test2.lower());
    EXPECT_EQ(0, test2.upper());
    EXPECT_TRUE(test2.isEmpty());

    test2 = mpfrInterval(bi, BoundType::WEAK, BoundType::WEAK);
    EXPECT_EQ(0, test2.lower());
    EXPECT_EQ(0, test2.upper());
    EXPECT_TRUE(test2.isEmpty());

	EXPECT_TRUE(test7.isOne());
	EXPECT_TRUE(isOne(test7));

	EXPECT_TRUE(test8.isZero());
	EXPECT_TRUE(isZero(test8));
}

TEST(mpfrInterval, StaticCreators)
{
    mpfrInterval i1 = mpfrInterval::emptyInterval();
    mpfrInterval i2 = mpfrInterval::unboundedInterval();
	mpfrInterval i3 = carl::constant_one<mpfrInterval>().get();
	mpfrInterval i4 = carl::constant_zero<mpfrInterval>().get();

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

TEST(mpfrInterval, Addition)
{
    mpfrInterval a0 = mpfrInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    mpfrInterval a1 = mpfrInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    mpfrInterval a2 = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    mpfrInterval a3 = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    mpfrInterval b0 = mpfrInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    mpfrInterval b1 = mpfrInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    mpfrInterval b2 = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    mpfrInterval b3 = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    mpfrInterval result;

    result = a0.add(b0);
    EXPECT_EQ( mpfrInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b1);
    EXPECT_EQ( mpfrInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b2);
	EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a0.add(b3);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);

    result = a1.add(b0);
    EXPECT_EQ( mpfrInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a1.add(b1);
	EXPECT_EQ( mpfrInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( mpfrInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);

    result = a2.add(b0);
	EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a2.add(b1);
    EXPECT_EQ( mpfrInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( mpfrInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b3);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);

    result = a3.add(b0);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a3.add(b1);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a3.add(b2);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a3.add(b3);
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
}


TEST(mpfrInterval, Subtraction)
{
    mpfrInterval a0 = mpfrInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    mpfrInterval a1 = mpfrInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    mpfrInterval a2 = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    mpfrInterval a3 = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    mpfrInterval b0 = mpfrInterval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    mpfrInterval b1 = mpfrInterval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    mpfrInterval b2 = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    mpfrInterval b3 = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    mpfrInterval result;

    result = a0.add(b0.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a0.add(b1.inverse());
    EXPECT_EQ( mpfrInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b2.inverse());
    EXPECT_EQ( mpfrInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b3.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);

    result = a1.add(b0.inverse());
    EXPECT_EQ( mpfrInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a1.add(b1.inverse());
    EXPECT_EQ( mpfrInterval(-3, BoundType::WEAK, 3, BoundType::WEAK), result);
    result = a1.add(b2.inverse());
    EXPECT_EQ( mpfrInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a1.add(b3.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);

    result = a2.add(b0.inverse());
    EXPECT_EQ( mpfrInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b1.inverse());
    EXPECT_EQ( mpfrInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b2.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a2.add(b3.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);

    result = a3.add(b0.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a3.add(b1.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a3.add(b2.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
    result = a3.add(b3.inverse());
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result);
}


TEST(mpfrInterval, Multiplication)
{
    mpfrInterval c0  = mpfrInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    mpfrInterval c1  = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    mpfrInterval c2  = mpfrInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    mpfrInterval c3  = mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    mpfrInterval c4  = mpfrInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    mpfrInterval c5  = mpfrInterval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    mpfrInterval c6  = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval c7  = mpfrInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval c8  = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    mpfrInterval d0  = mpfrInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    mpfrInterval d1  = mpfrInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    mpfrInterval d2  = mpfrInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    mpfrInterval d3  = mpfrInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    mpfrInterval d4  = mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    mpfrInterval d5  = mpfrInterval( -1, BoundType::WEAK, 0, BoundType::WEAK );
    mpfrInterval d6  = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    mpfrInterval d7  = mpfrInterval( 0, BoundType::WEAK, 1, BoundType::WEAK );
    mpfrInterval d8  = mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK );
    mpfrInterval d9  = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    mpfrInterval d10 = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval d11 = mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval d12 = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    mpfrInterval result;

    result = c0.mul( d0 );
    EXPECT_EQ( mpfrInterval( c0.upper() * d0.upper(), BoundType::WEAK, c0.lower() * d0.lower(), BoundType::WEAK ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( mpfrInterval( c0.upper() * d5.upper(), BoundType::WEAK, c0.lower() * d5.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( mpfrInterval( c1.upper() * d0.lower(), BoundType::WEAK, c1.lower() * d0.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( mpfrInterval( c1.upper() * d5.lower(), BoundType::WEAK, c1.lower() * d5.lower(), BoundType::WEAK ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( mpfrInterval( c2.upper() * d0.lower(), BoundType::WEAK, c2.lower() * d0.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( mpfrInterval( c2.upper() * d5.lower(), BoundType::WEAK, c2.lower() * d5.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( mpfrInterval( c4.upper() * d0.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( mpfrInterval( c4.upper() * d5.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( mpfrInterval( c5.upper() * d0.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( mpfrInterval( c5.upper() * d5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c6.lower() * d0.lower(), BoundType::WEAK ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c6.lower() * d5.lower(), BoundType::WEAK ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c7.lower() * d0.upper(), BoundType::WEAK ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c7.lower() * d5.upper(), BoundType::WEAK ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d6 );
    EXPECT_EQ( mpfrInterval( c0.lower() * d6.upper(), BoundType::WEAK, c0.lower() * d6.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( mpfrInterval( ( c1.lower() * d6.upper() < c1.upper() * d6.lower() ? c1.lower() * d6.upper() : c1.upper() * d6.lower()),
                                         BoundType::WEAK,
                                         c1.lower() * d6.lower() > c1.upper() * d6.upper() ? c1.lower() * d6.lower() : c1.upper() * d6.upper(),
                                         BoundType::WEAK ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( mpfrInterval( c2.upper() * d6.lower(), BoundType::WEAK, c2.upper() * d6.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d6 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d6 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d6 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d6 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d6 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d6 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d1 );
    EXPECT_EQ( mpfrInterval( c0.lower() * d1.upper(), BoundType::WEAK, c0.upper() * d1.lower(), BoundType::WEAK ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( mpfrInterval( c0.lower() * d7.upper(), BoundType::WEAK, c0.upper() * d7.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( mpfrInterval( c1.lower() * d1.upper(), BoundType::WEAK, c1.upper() * d1.upper(), BoundType::WEAK ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( mpfrInterval( c1.lower() * d7.upper(), BoundType::WEAK, c1.upper() * d7.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( mpfrInterval( c2.lower() * d1.lower(), BoundType::WEAK, c2.upper() * d1.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( mpfrInterval( c2.lower() * d7.lower(), BoundType::WEAK, c2.upper() * d7.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c4.upper() * d1.lower(), BoundType::WEAK ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c4.upper() * d7.lower(), BoundType::WEAK ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c5.upper() * d1.upper(), BoundType::WEAK ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c5.upper() * d7.upper(), BoundType::WEAK ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( mpfrInterval( c6.lower() * d1.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( mpfrInterval( c6.lower() * d7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( mpfrInterval( c7.lower() * d1.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( mpfrInterval( c7.lower() * d7.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c8.mul( d1 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d7 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c1.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c2.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c3.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );;
    result = c5.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c6.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c7.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c8.mul( d4 );
    EXPECT_EQ( mpfrInterval( 0 ), result );

    result = c0.mul( d2 );
    EXPECT_EQ( mpfrInterval( c0.upper() * d2.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( mpfrInterval( c0.upper() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c2.mul( d2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c2.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c2.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( mpfrInterval( c4.upper() * d2.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( mpfrInterval( c4.upper() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c7.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c7.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( mpfrInterval( c0.lower() * d9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( mpfrInterval( c0.lower() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c2.upper() * d9.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c2.upper() * d8.upper(), BoundType::WEAK ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c4.mul( d8 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d9 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d9 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d9 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d8 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c0.lower() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c0.lower() * d10.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( mpfrInterval( c2.upper() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( mpfrInterval( c2.upper() * d10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c3.mul( d10 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
    result = c4.mul( d10 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d10 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d10 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d10 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c0.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c0.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( mpfrInterval( c2.lower() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( mpfrInterval( c2.lower() * d3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c4.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, c4.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( mpfrInterval( c7.lower() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( mpfrInterval( c7.lower() * d3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c8.mul( d11 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d3 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );

    result = c0.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c1.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c2.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c3.mul( d12 );
    EXPECT_EQ( mpfrInterval( 0 ), result );
    result = c4.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c5.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c6.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c7.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
    result = c8.mul( d12 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result );
}


TEST(mpfrInterval, ExtendedDivision)
{
    mpfrInterval a0  = mpfrInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    mpfrInterval a1  = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    mpfrInterval a2  = mpfrInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    mpfrInterval a3  = mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    mpfrInterval a4  = mpfrInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    mpfrInterval a5  = mpfrInterval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    mpfrInterval a6  = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval a7  = mpfrInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval a8  = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    mpfrInterval b0  = mpfrInterval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    mpfrInterval b1  = mpfrInterval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    mpfrInterval b2  = mpfrInterval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    mpfrInterval b3  = mpfrInterval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    mpfrInterval b4  = mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    mpfrInterval b5  = mpfrInterval( -1, BoundType::WEAK, 0, BoundType::WEAK );
    mpfrInterval b6  = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    mpfrInterval b7  = mpfrInterval( 0, BoundType::WEAK, 1, BoundType::WEAK );
    mpfrInterval b8  = mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK );
    mpfrInterval b9  = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    mpfrInterval b10 = mpfrInterval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval b11 = mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY );
    mpfrInterval b12 = mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    mpfrInterval result1, result2;
    // Table 7 Tests: Division without containing 0
    a0.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( a0.upper() / b0.lower(), BoundType::WEAK, a0.lower() / b0.upper(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( a1.upper() / b0.upper(), BoundType::WEAK, a1.lower() / b0.upper(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( a2.upper() / b0.upper(), BoundType::WEAK, a2.lower() / b0.lower(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( a4.upper() / b0.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( a5.upper() / b0.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a6.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a6.lower() / b0.upper(), BoundType::WEAK ), result1 );
    a7.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a7.lower() / b0.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b0, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( a0.lower() / b1.lower(), BoundType::WEAK, a0.upper() / b1.upper(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( a1.lower() / b1.lower(), BoundType::WEAK, a1.upper() / b1.lower(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( a2.lower() / b1.upper(), BoundType::WEAK, a2.upper() / b1.lower(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a4.upper() / b1.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a5.upper() / b1.lower(), BoundType::WEAK ), result1 );
    a6.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( a6.lower() / b1.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a7.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( a7.lower() / b1.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a8.div_ext( b1, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0.0, BoundType::WEAK, a0.lower() / b2.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( a1.upper() / b2.upper(), BoundType::WEAK, a1.lower() / b2.upper(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( a2.upper() / b2.upper(), BoundType::WEAK, 0.0, BoundType::WEAK ), result1 );
    a3.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( a5.upper() / b2.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a6.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a6.lower() / b2.upper(), BoundType::WEAK ), result1 );
    a7.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b2, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( a0.lower() / b3.lower(), BoundType::WEAK, 0.0, BoundType::WEAK ), result1 );
    a1.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( a1.lower() / b3.lower(), BoundType::WEAK, a1.upper() / b3.lower(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0.0, BoundType::WEAK, a2.upper() / b3.lower(), BoundType::WEAK ), result1 );
    a3.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a5.upper() / b3.lower(), BoundType::WEAK ), result1 );
    a6.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( a6.lower() / b3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a7.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b3, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    //Table 8 tests with division containin 0
    a0.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::emptyInterval(), result1 );
    a1.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::emptyInterval(), result1 );

    a4.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::emptyInterval(), result1 );
    a5.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::emptyInterval(), result1 );
    a8.div_ext( b4, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval( a0.upper() / b5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a1.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a2.lower() / b5.lower(), BoundType::WEAK ), result1 );

    a4.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval( a4.upper() / b5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a7.lower() / b5.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b5, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval( a0.upper() / b6.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a0.upper() / b6.upper(), BoundType::WEAK ), result2 );
    a1.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a2.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( mpfrInterval( a2.lower() / b6.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );

    a4.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a4.upper() / b6.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( mpfrInterval( a4.upper() / b6.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a7.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( mpfrInterval( a7.lower() / b6.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );
    a8.div_ext( b6, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a0.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval( a2.lower() / b7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );

    a4.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a4.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval( a7.lower() / b7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a8.div_ext( b7, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );

    a4.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b8, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a0.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( mpfrInterval( a2.lower() / b9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );

    a4.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a4.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( mpfrInterval( a7.lower() / b9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );
    a8.div_ext( b9, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( mpfrInterval( a0.upper() / b10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a1.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a2.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( mpfrInterval( a4.upper() / b10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1.0, BoundType::INFTY, a7.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( b10, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a1.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b11, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a0.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a1.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a2.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );

    a4.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a5.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a6.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a7.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
    a8.div_ext( b12, result1, result2 );
    EXPECT_EQ( mpfrInterval::unboundedInterval(), result1 );
}

TEST(mpfrInterval, Intersection)
{
    mpfrInterval a1(-1,BoundType::WEAK,1,BoundType::WEAK);

    mpfrInterval b01(2,BoundType::WEAK,3,BoundType::WEAK);
    mpfrInterval b02(1,BoundType::WEAK,3,BoundType::WEAK);
    mpfrInterval b03(0,BoundType::WEAK,3,BoundType::WEAK);
    mpfrInterval b04(-1,BoundType::WEAK,1,BoundType::WEAK);
    mpfrInterval b05(-2,BoundType::WEAK,0,BoundType::WEAK);

    mpfrInterval b06(-2,BoundType::WEAK,-1,BoundType::WEAK);
    mpfrInterval b07(-3,BoundType::WEAK,-2,BoundType::WEAK);
    mpfrInterval b08(-1,BoundType::WEAK,-1,BoundType::WEAK);
    mpfrInterval b09(1,BoundType::WEAK,1,BoundType::WEAK);
    mpfrInterval b10(0,BoundType::WEAK,1,BoundType::WEAK);

    mpfrInterval b11(-1,BoundType::WEAK,0,BoundType::WEAK);
    mpfrInterval b12(-0.5,BoundType::WEAK,0.5,BoundType::WEAK);
    mpfrInterval b13(-2,BoundType::WEAK,2,BoundType::WEAK);
    mpfrInterval b14(0,BoundType::STRICT,0,BoundType::STRICT);
    mpfrInterval b15(-1,BoundType::INFTY,1,BoundType::INFTY);

    mpfrInterval b16(-1,BoundType::INFTY,-1,BoundType::WEAK);
    mpfrInterval b17(-1,BoundType::INFTY,0,BoundType::WEAK);
    mpfrInterval b18(-1,BoundType::INFTY,1,BoundType::WEAK);
    mpfrInterval b19(-1,BoundType::WEAK,1,BoundType::INFTY);
    mpfrInterval b20(0,BoundType::WEAK,1,BoundType::INFTY);

    mpfrInterval b21(1,BoundType::WEAK,1,BoundType::INFTY);

    EXPECT_EQ(mpfrInterval::emptyInterval(), carl::set_intersection(a1, b01));
    EXPECT_EQ(mpfrInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b02));
    EXPECT_EQ(mpfrInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b03));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b04));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b05));

    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b06));
    EXPECT_EQ(mpfrInterval::emptyInterval(), carl::set_intersection(a1, b07));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b08));
    EXPECT_EQ(mpfrInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b09));
    EXPECT_EQ(mpfrInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b10));

    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b11));
    EXPECT_EQ(mpfrInterval(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), carl::set_intersection(a1, b12));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b13));
    EXPECT_EQ(mpfrInterval::emptyInterval(), carl::set_intersection(a1, b14));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b15));

    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b16));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b17));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b18));
    EXPECT_EQ(mpfrInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b19));
    EXPECT_EQ(mpfrInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b20));

    EXPECT_EQ(mpfrInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b21));
}

TEST(mpfrInterval, Union)
{
    mpfrInterval i1(3, BoundType::WEAK, 5, BoundType::WEAK);
    mpfrInterval i2(1, BoundType::WEAK, 4, BoundType::WEAK);
    mpfrInterval i3(-2, BoundType::WEAK, 1, BoundType::WEAK);
    mpfrInterval i4(4, BoundType::STRICT, 9, BoundType::STRICT);
    mpfrInterval i5(1, BoundType::STRICT, 4, BoundType::STRICT);
    mpfrInterval i6(3, BoundType::STRICT, 3, BoundType::INFTY);
    mpfrInterval i7(0, BoundType::INFTY, 0, BoundType::INFTY);
    mpfrInterval result1, result2;

    EXPECT_FALSE(carl::set_union(i1, i2, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_union(i2, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_TRUE(carl::set_union(i1, i3, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);
    EXPECT_EQ(mpfrInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_union(i3, i2, result1, result2));
    EXPECT_EQ(mpfrInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_union(i4, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_TRUE(carl::set_union(i3, i4, result1, result2));
    EXPECT_EQ(mpfrInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(mpfrInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(carl::set_union(i2, i4, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_union(i2, i5, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_TRUE(carl::set_union(i5, i4, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::STRICT, 4, BoundType::STRICT), result1);
    EXPECT_EQ(mpfrInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(carl::set_union(i6, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 3, BoundType::INFTY), result1);

    EXPECT_TRUE(carl::set_union(i6, i3, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::STRICT, 3, BoundType::INFTY), result1);
    EXPECT_EQ(mpfrInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_union(i1, i7, result1, result2));
    EXPECT_EQ(mpfrInterval::unboundedInterval(), result1);

    EXPECT_FALSE(carl::set_union(i7, i6, result1, result2));
    EXPECT_EQ(mpfrInterval::unboundedInterval(), result1);
}

TEST(mpfrInterval, Difference)
{
    mpfrInterval i1(3, BoundType::WEAK, 5, BoundType::WEAK);
    mpfrInterval i2(1, BoundType::WEAK, 4, BoundType::WEAK);
    mpfrInterval i3(-1, BoundType::WEAK, 2, BoundType::WEAK);
    mpfrInterval i4(2, BoundType::WEAK, 3, BoundType::WEAK);
    mpfrInterval i5(1, BoundType::STRICT, 3, BoundType::STRICT);
    mpfrInterval i6(2, BoundType::WEAK, 2, BoundType::INFTY);
    mpfrInterval i7(3, BoundType::WEAK, 3, BoundType::WEAK);
    mpfrInterval i8(0, BoundType::STRICT, 0, BoundType::STRICT);
    mpfrInterval result1, result2;

    EXPECT_FALSE(carl::set_difference(i1, i2, result1, result2));
    EXPECT_EQ(mpfrInterval(4, BoundType::STRICT, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i2, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 3, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_difference(i1, i3, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i3, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);

    EXPECT_TRUE(carl::set_difference(i2, i4, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 2, BoundType::STRICT), result1);
    EXPECT_EQ(mpfrInterval(3, BoundType::STRICT, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_difference(i4, i2, result1, result2));
    EXPECT_EQ(mpfrInterval::emptyInterval(), result1);

    EXPECT_FALSE(carl::set_difference(i5, i2, result1, result2));
    EXPECT_EQ(mpfrInterval::emptyInterval(), result1);

    EXPECT_TRUE(carl::set_difference(i2, i5, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_difference(i1, i1, result1, result2));
    EXPECT_EQ(mpfrInterval::emptyInterval(), result1);

    EXPECT_FALSE(carl::set_difference(i6, i2, result1, result2));
    EXPECT_EQ(mpfrInterval(4, BoundType::STRICT, 4, BoundType::INFTY), result1);

    EXPECT_FALSE(carl::set_difference(i2, i6, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 2, BoundType::STRICT), result1);

    EXPECT_TRUE(carl::set_difference(i6, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(2, BoundType::WEAK, 3, BoundType::STRICT), result1);
    EXPECT_EQ(mpfrInterval(5, BoundType::STRICT, 5, BoundType::INFTY), result2);

    EXPECT_FALSE(carl::set_difference(i1, i6, result1, result2));
    EXPECT_EQ(mpfrInterval::emptyInterval(), result1);

    EXPECT_FALSE(carl::set_difference(i3, i6, result1, result2));
    EXPECT_EQ(mpfrInterval(-1, BoundType::WEAK, 2, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_difference(i7, i2, result1, result2));
    EXPECT_EQ(mpfrInterval::emptyInterval(), result1);

    EXPECT_TRUE(carl::set_difference(i2, i7, result1, result2));
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 3, BoundType::STRICT), result1);
    EXPECT_EQ(mpfrInterval(3, BoundType::STRICT, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_difference(i1, i7, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::STRICT, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i8, i1, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i1, i8, result1, result2));
    EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i8, i3, result1, result2));
    EXPECT_EQ(mpfrInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i3, i8, result1, result2));
    EXPECT_EQ(mpfrInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);
}

TEST(mpfrInterval, Split)
{
    mpfrInterval i1(-1, BoundType::INFTY, 1, BoundType::INFTY);
    mpfrInterval i2(-1, BoundType::STRICT, 1, BoundType::STRICT);
    mpfrInterval i3(-1, BoundType::WEAK, 1, BoundType::WEAK);
    mpfrInterval i4(0, BoundType::STRICT, 0, BoundType::STRICT);

	std::pair<mpfrInterval, mpfrInterval> res;

	res = i1.split();
    EXPECT_EQ(mpfrInterval(0, BoundType::INFTY, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(mpfrInterval(0, BoundType::WEAK, 0, BoundType::INFTY), res.second);

    res = i2.split();
    EXPECT_EQ(mpfrInterval(-1, BoundType::STRICT, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(mpfrInterval(0, BoundType::WEAK, 1, BoundType::STRICT), res.second);

    res = i3.split();
    EXPECT_EQ(mpfrInterval(-1, BoundType::WEAK, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(mpfrInterval(0, BoundType::WEAK, 1, BoundType::WEAK), res.second);

    res = i4.split();
    EXPECT_EQ(mpfrInterval(0, BoundType::STRICT, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(mpfrInterval(0, BoundType::WEAK, 0, BoundType::STRICT), res.second);

    // uniform multi-split
    mpfrInterval i5(0,BoundType::WEAK, 5, BoundType::STRICT);

    std::list<mpfrInterval> results;
    results = i5.split(5);
    EXPECT_EQ((unsigned)5, results.size());
    EXPECT_EQ(mpfrInterval(0, BoundType::WEAK, 1, BoundType::STRICT), *results.begin());
	results.pop_front();
    EXPECT_EQ(mpfrInterval(1, BoundType::WEAK, 2, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(mpfrInterval(2, BoundType::WEAK, 3, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(mpfrInterval(3, BoundType::WEAK, 4, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(mpfrInterval(4, BoundType::WEAK, 5, BoundType::STRICT), *results.begin());
}

TEST(mpfrInterval, Properties)
{
    mpfrInterval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    mpfrInterval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    mpfrInterval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    mpfrInterval i4(-5, BoundType::WEAK, 3, BoundType::WEAK);

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

TEST(mpfrInterval, Contains)
{
    mpfrInterval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    mpfrInterval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    mpfrInterval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    mpfrInterval i4(-5, BoundType::WEAK, 4, BoundType::WEAK);
    mpfrInterval i5(4, BoundType::STRICT, 5, BoundType::STRICT);
    mpfrInterval i6(3, BoundType::WEAK, 7, BoundType::WEAK);
    mpfrInterval i7(3, BoundType::STRICT, 4, BoundType::STRICT);
    mpfrInterval i8(3, BoundType::WEAK, 3, BoundType::INFTY);

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
    EXPECT_FALSE(carl::set_is_subset(i1, i2));
    EXPECT_FALSE(carl::set_is_subset(i2, i1));
    EXPECT_FALSE(carl::set_is_subset(i1, i3));
    EXPECT_TRUE(carl::set_is_subset(i3, i1));
    EXPECT_TRUE(carl::set_is_subset(i1, i5));
    EXPECT_FALSE(carl::set_is_subset(i5, i1));
    EXPECT_FALSE(carl::set_is_subset(i1, i6));
    EXPECT_TRUE(carl::set_is_subset(i6, i1));
    EXPECT_TRUE(carl::set_is_subset(i1, i1));
    EXPECT_TRUE(carl::set_is_subset(i6, i6));
    EXPECT_TRUE(carl::set_is_subset(i1, i7));
    EXPECT_FALSE(carl::set_is_subset(i7, i1));
    EXPECT_TRUE(carl::set_is_subset(i6, i7));
    EXPECT_FALSE(carl::set_is_subset(i7, i6));
    EXPECT_TRUE(carl::set_is_subset(i8, i5));
    EXPECT_FALSE(carl::set_is_subset(i5, i8));
    EXPECT_TRUE(carl::set_is_subset(i8, i6));
    EXPECT_FALSE(carl::set_is_subset(i6, i8));
    EXPECT_FALSE(carl::set_is_subset(i8, i4));
    EXPECT_FALSE(carl::set_is_subset(i4, i8));

    EXPECT_FALSE(carl::set_is_proper_subset(i1, i2));
    EXPECT_FALSE(carl::set_is_proper_subset(i2, i1));
    EXPECT_FALSE(carl::set_is_proper_subset(i1, i3));
    EXPECT_TRUE(carl::set_is_proper_subset(i3, i1));
    EXPECT_TRUE(carl::set_is_proper_subset(i1, i5));
    EXPECT_FALSE(carl::set_is_proper_subset(i5, i1));
    EXPECT_FALSE(carl::set_is_proper_subset(i1, i6));
    EXPECT_TRUE(carl::set_is_proper_subset(i6, i1));
    EXPECT_TRUE(carl::set_is_proper_subset(i1, i1));
    EXPECT_TRUE(carl::set_is_proper_subset(i6, i6));
    EXPECT_TRUE(carl::set_is_proper_subset(i1, i7));
    EXPECT_FALSE(carl::set_is_proper_subset(i7, i1));
    EXPECT_TRUE(carl::set_is_proper_subset(i6, i7));
    EXPECT_FALSE(carl::set_is_proper_subset(i7, i6));
    EXPECT_TRUE(carl::set_is_proper_subset(i8, i5));
    EXPECT_FALSE(carl::set_is_proper_subset(i5, i8));
    EXPECT_TRUE(carl::set_is_proper_subset(i8, i6));
    EXPECT_FALSE(carl::set_is_proper_subset(i6, i8));
    EXPECT_FALSE(carl::set_is_proper_subset(i8, i4));
    EXPECT_FALSE(carl::set_is_proper_subset(i4, i8));
}

TEST(mpfrInterval, BloatShrink)
{
    mpfrInterval i1(3, BoundType::WEAK, 7, BoundType::WEAK);
    mpfrInterval i2(-13, BoundType::STRICT, 1, BoundType::STRICT);
    mpfrInterval i3(0, BoundType::STRICT, 1, BoundType::STRICT);
    mpfrInterval i4(5, BoundType::STRICT, 13, BoundType::STRICT);
    mpfrInterval result1(-2, BoundType::WEAK, 12, BoundType::WEAK);
    mpfrInterval result2(-10, BoundType::STRICT, -2, BoundType::STRICT);
    mpfrInterval result3(2, BoundType::STRICT, -1, BoundType::STRICT);
    mpfrInterval result4(7, BoundType::STRICT, 11, BoundType::STRICT);

    // Bloat by adding
    i1.bloat_by(5);
    EXPECT_EQ(result1, i1);
    i2.bloat_by(-3);
    EXPECT_EQ(result2, i2);
    // Note that here exist inconsistencies
    // as we can create in valid intervals using this method
    i3.bloat_by(-2);
    EXPECT_EQ(result3, i3);

    // Shrink by subtracting
    i4.shrink_by(2);
    EXPECT_EQ(result4, i4);
}

#endif
