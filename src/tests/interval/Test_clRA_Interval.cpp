/**
 * Test cases for the cl_RA instanciation of the generic interval class.
 * @file Test_clRA_Interval.cpp
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @author Benedikt Seidl
 *
 * @since	2014-01-31
 * @version 2014-11-11
 */

#include "carl/util/platform.h"
CLANG_WARNING_DISABLE("-Wsign-compare")
#include "gtest/gtest.h"
CLANG_WARNING_RESET
#include "carl/interval/Interval.h"
#include "carl/interval/set_theory.h"
#include "carl/core/VariablePool.h"

#include "../Common.h"

using namespace carl;

typedef Interval<Rational> clRA_Interval;

TEST(clRA_Interval, Constructor)
{
    clRA_Interval test1 = clRA_Interval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    clRA_Interval test2 = clRA_Interval(-1, BoundType::STRICT, 1, BoundType::STRICT);
	clRA_Interval test3 = clRA_Interval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(clRA_Interval(1, BoundType::WEAK, -1, BoundType::WEAK), clRA_Interval::emptyInterval());
    clRA_Interval test5 = clRA_Interval::unboundedInterval();
    clRA_Interval test6 = clRA_Interval::emptyInterval();

    clRA_Interval test7 = clRA_Interval((mpq_class)-1, BoundType::WEAK, (mpq_class)1, BoundType::WEAK);

	clRA_Interval test8 = clRA_Interval(2, BoundType::STRICT, 0, BoundType::INFTY);
	clRA_Interval test9 = clRA_Interval(1);
	clRA_Interval test10 = clRA_Interval(0);
    SUCCEED();
}

TEST(clRA_Interval, Getters)
{
    clRA_Interval test1 = clRA_Interval(-1.0, BoundType::WEAK, 1.0, BoundType::STRICT);
    clRA_Interval test2 = clRA_Interval(-1, BoundType::WEAK, 1, BoundType::STRICT);
    clRA_Interval test3 = clRA_Interval(1, BoundType::STRICT, 1, BoundType::STRICT);
    clRA_Interval test4 = clRA_Interval(4, BoundType::WEAK, 2, BoundType::WEAK);
    clRA_Interval test5 = clRA_Interval();
    clRA_Interval test6 = clRA_Interval(4);
	clRA_Interval test7 = clRA_Interval(1);
	clRA_Interval test8 = clRA_Interval(0);

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

    test1.set(clRA_Interval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());

    test1 = clRA_Interval();
    test1.set(clRA_Interval::BoostInterval(3, 27));
    EXPECT_EQ(3, test1.lower());
    EXPECT_EQ(27, test1.upper());

	EXPECT_TRUE(test7.isOne());
	EXPECT_TRUE(isOne(test7));

	EXPECT_TRUE(test8.isZero());
	EXPECT_TRUE(isZero(test8));
}

TEST(clRA_Interval, StaticCreators)
{
    clRA_Interval i1 = clRA_Interval::emptyInterval();
    clRA_Interval i2 = clRA_Interval::unboundedInterval();
	clRA_Interval i3 = carl::constant_one<clRA_Interval>().get();
	clRA_Interval i4 = carl::constant_zero<clRA_Interval>().get();

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

TEST(clRA_Interval, Addition)
{
    clRA_Interval a0 = clRA_Interval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    clRA_Interval a1 = clRA_Interval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    clRA_Interval a2 = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    clRA_Interval a3 = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    clRA_Interval b0 = clRA_Interval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    clRA_Interval b1 = clRA_Interval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    clRA_Interval b2 = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    clRA_Interval b3 = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    clRA_Interval result;

    result = a0.add(b0);
    EXPECT_EQ( clRA_Interval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b1);
    EXPECT_EQ( clRA_Interval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a0.add(b2);
	EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a0.add(b3);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);

    result = a1.add(b0);
    EXPECT_EQ( clRA_Interval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a1.add(b1);
	EXPECT_EQ( clRA_Interval(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( clRA_Interval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);

    result = a2.add(b0);
	EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a2.add(b1);
    EXPECT_EQ( clRA_Interval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( clRA_Interval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b3);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);

    result = a3.add(b0);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a3.add(b1);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a3.add(b2);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a3.add(b3);
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
}


TEST(clRA_Interval, Subtraction)
{
    clRA_Interval a0 = clRA_Interval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    clRA_Interval a1 = clRA_Interval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    clRA_Interval a2 = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    clRA_Interval a3 = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    clRA_Interval b0 = clRA_Interval( -1, BoundType::INFTY, 2, BoundType::WEAK);
    clRA_Interval b1 = clRA_Interval( -1, BoundType::WEAK, 2, BoundType::WEAK);
    clRA_Interval b2 = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY);
    clRA_Interval b3 = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY);

    clRA_Interval result;

    result = a0.add(b0.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a0.add(b1.inverse());
    EXPECT_EQ( clRA_Interval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b2.inverse());
    EXPECT_EQ( clRA_Interval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b3.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);

    result = a1.add(b0.inverse());
    EXPECT_EQ( clRA_Interval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a1.add(b1.inverse());
    EXPECT_EQ( clRA_Interval(-3, BoundType::WEAK, 3, BoundType::WEAK), result);
    result = a1.add(b2.inverse());
    EXPECT_EQ( clRA_Interval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a1.add(b3.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);

    result = a2.add(b0.inverse());
    EXPECT_EQ( clRA_Interval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b1.inverse());
    EXPECT_EQ( clRA_Interval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b2.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a2.add(b3.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);

    result = a3.add(b0.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a3.add(b1.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a3.add(b2.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
    result = a3.add(b3.inverse());
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result);
}


TEST(clRA_Interval, Multiplication)
{
    clRA_Interval c0  = clRA_Interval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    clRA_Interval c1  = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    clRA_Interval c2  = clRA_Interval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    clRA_Interval c3  = clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    clRA_Interval c4  = clRA_Interval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    clRA_Interval c5  = clRA_Interval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    clRA_Interval c6  = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval c7  = clRA_Interval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval c8  = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    clRA_Interval d0  = clRA_Interval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    clRA_Interval d1  = clRA_Interval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    clRA_Interval d2  = clRA_Interval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    clRA_Interval d3  = clRA_Interval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    clRA_Interval d4  = clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    clRA_Interval d5  = clRA_Interval( -1, BoundType::WEAK, 0, BoundType::WEAK );
    clRA_Interval d6  = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    clRA_Interval d7  = clRA_Interval( 0, BoundType::WEAK, 1, BoundType::WEAK );
    clRA_Interval d8  = clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK );
    clRA_Interval d9  = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    clRA_Interval d10 = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval d11 = clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval d12 = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    clRA_Interval result;

    result = c0.mul( d0 );
    EXPECT_EQ( clRA_Interval( c0.upper() * d0.upper(), BoundType::WEAK, c0.lower() * d0.lower(), BoundType::WEAK ),
                          result );
    result = c0.mul( d5 );
    EXPECT_EQ( clRA_Interval( c0.upper() * d5.upper(), BoundType::WEAK, c0.lower() * d5.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d0 );
    EXPECT_EQ( clRA_Interval( c1.upper() * d0.lower(), BoundType::WEAK, c1.lower() * d0.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d5 );
    EXPECT_EQ( clRA_Interval( c1.upper() * d5.lower(), BoundType::WEAK, c1.lower() * d5.lower(), BoundType::WEAK ),
                          result );
    result = c2.mul( d0 );
    EXPECT_EQ( clRA_Interval( c2.upper() * d0.lower(), BoundType::WEAK, c2.lower() * d0.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d5 );
    EXPECT_EQ( clRA_Interval( c2.upper() * d5.lower(), BoundType::WEAK, c2.lower() * d5.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d0 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c3.mul( d5 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result );
    result = c4.mul( d0 );
    EXPECT_EQ( clRA_Interval( c4.upper() * d0.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c4.mul( d5 );
    EXPECT_EQ( clRA_Interval( c4.upper() * d5.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d0 );
    EXPECT_EQ( clRA_Interval( c5.upper() * d0.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d5 );
    EXPECT_EQ( clRA_Interval( c5.upper() * d5.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c6.mul( d0 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c6.lower() * d0.lower(), BoundType::WEAK ), result );
    result = c6.mul( d5 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c6.lower() * d5.lower(), BoundType::WEAK ), result );
    result = c7.mul( d0 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c7.lower() * d0.upper(), BoundType::WEAK ), result );
    result = c7.mul( d5 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c7.lower() * d5.upper(), BoundType::WEAK ), result );
    result = c8.mul( d0 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d6 );
    EXPECT_EQ( clRA_Interval( c0.lower() * d6.upper(), BoundType::WEAK, c0.lower() * d6.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d6 );
    EXPECT_EQ( clRA_Interval( ( c1.lower() * d6.upper() < c1.upper() * d6.lower() ? c1.lower() * d6.upper() : c1.upper() * d6.lower()),
                                         BoundType::WEAK,
                                         c1.lower() * d6.lower() > c1.upper() * d6.upper() ? c1.lower() * d6.lower() : c1.upper() * d6.upper(),
                                         BoundType::WEAK ),
                          result );
    result = c2.mul( d6 );
    EXPECT_EQ( clRA_Interval( c2.upper() * d6.lower(), BoundType::WEAK, c2.upper() * d6.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d6 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d6 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d6 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d6 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d6 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d6 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d1 );
    EXPECT_EQ( clRA_Interval( c0.lower() * d1.upper(), BoundType::WEAK, c0.upper() * d1.lower(), BoundType::WEAK ),
                          result );
    result = c0.mul( d7 );
    EXPECT_EQ( clRA_Interval( c0.lower() * d7.upper(), BoundType::WEAK, c0.upper() * d7.lower(), BoundType::WEAK ),
                          result );
    result = c1.mul( d1 );
    EXPECT_EQ( clRA_Interval( c1.lower() * d1.upper(), BoundType::WEAK, c1.upper() * d1.upper(), BoundType::WEAK ),
                          result );
    result = c1.mul( d7 );
    EXPECT_EQ( clRA_Interval( c1.lower() * d7.upper(), BoundType::WEAK, c1.upper() * d7.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d1 );
    EXPECT_EQ( clRA_Interval( c2.lower() * d1.lower(), BoundType::WEAK, c2.upper() * d1.upper(), BoundType::WEAK ),
                          result );
    result = c2.mul( d7 );
    EXPECT_EQ( clRA_Interval( c2.lower() * d7.lower(), BoundType::WEAK, c2.upper() * d7.upper(), BoundType::WEAK ),
                          result );
    result = c3.mul( d1 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c3.mul( d7 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d1 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c4.upper() * d1.lower(), BoundType::WEAK ), result );
    result = c4.mul( d7 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c4.upper() * d7.lower(), BoundType::WEAK ), result );
    result = c5.mul( d1 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c5.upper() * d1.upper(), BoundType::WEAK ), result );
    result = c5.mul( d7 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c5.upper() * d7.upper(), BoundType::WEAK ), result );
    result = c6.mul( d1 );
    EXPECT_EQ( clRA_Interval( c6.lower() * d1.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c6.mul( d7 );
    EXPECT_EQ( clRA_Interval( c6.lower() * d7.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d1 );
    EXPECT_EQ( clRA_Interval( c7.lower() * d1.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d7 );
    EXPECT_EQ( clRA_Interval( c7.lower() * d7.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c8.mul( d1 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d7 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c1.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c2.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c3.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );;
    result = c5.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c6.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c7.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c8.mul( d4 );
    EXPECT_EQ( clRA_Interval( 0 ), result );

    result = c0.mul( d2 );
    EXPECT_EQ( clRA_Interval( c0.upper() * d2.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( clRA_Interval( c0.upper() * d8.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c1.mul( d2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c2.mul( d2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c2.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c2.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c3.mul( d2 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d2 );
    EXPECT_EQ( clRA_Interval( c4.upper() * d2.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c4.mul( d8 );
    EXPECT_EQ( clRA_Interval( c4.upper() * d8.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c7.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c7.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( clRA_Interval( c0.lower() * d9.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( clRA_Interval( c0.lower() * d8.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c2.upper() * d9.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c2.upper() * d8.upper(), BoundType::WEAK ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c4.mul( d8 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d9 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d9 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d9 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d8 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c0.lower() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c0.lower() * d10.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( clRA_Interval( c2.upper() * d11.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c2.mul( d10 );
    EXPECT_EQ( clRA_Interval( c2.upper() * d10.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c3.mul( d10 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
    result = c4.mul( d10 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d10 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d10 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d10 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c0.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c0.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c2.mul( d11 );
    EXPECT_EQ( clRA_Interval( c2.lower() * d11.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c2.mul( d3 );
    EXPECT_EQ( clRA_Interval( c2.lower() * d3.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c3.mul( d11 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c3.mul( d3 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d11 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c4.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c4.mul( d3 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, c4.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c5.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( clRA_Interval( c7.lower() * d11.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( clRA_Interval( c7.lower() * d3.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c8.mul( d11 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d3 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );

    result = c0.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c1.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c2.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c3.mul( d12 );
    EXPECT_EQ( clRA_Interval( 0 ), result );
    result = c4.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c5.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c6.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c7.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
    result = c8.mul( d12 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result );
}


TEST(clRA_Interval, ExtendedDivision)
{
    clRA_Interval a0  = clRA_Interval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    clRA_Interval a1  = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    clRA_Interval a2  = clRA_Interval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    clRA_Interval a3  = clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    clRA_Interval a4  = clRA_Interval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    clRA_Interval a5  = clRA_Interval( -2, BoundType::INFTY, 1, BoundType::WEAK );
    clRA_Interval a6  = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval a7  = clRA_Interval( 1, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval a8  = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY );

    clRA_Interval b0  = clRA_Interval( -2, BoundType::WEAK, -1, BoundType::WEAK );
    clRA_Interval b1  = clRA_Interval( 1, BoundType::WEAK, 2, BoundType::WEAK );
    clRA_Interval b2  = clRA_Interval( -1, BoundType::INFTY, -1, BoundType::WEAK );
    clRA_Interval b3  = clRA_Interval( 1, BoundType::WEAK, 1, BoundType::INFTY );

    clRA_Interval b4  = clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK );
    clRA_Interval b5  = clRA_Interval( -1, BoundType::WEAK, 0, BoundType::WEAK );
    clRA_Interval b6  = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::WEAK );
    clRA_Interval b7  = clRA_Interval( 0, BoundType::WEAK, 1, BoundType::WEAK );
    clRA_Interval b8  = clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK );
    clRA_Interval b9  = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::WEAK );
    clRA_Interval b10 = clRA_Interval( -1, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval b11 = clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY );
    clRA_Interval b12 = clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY );
    clRA_Interval result1, result2;
    // Table 7 Tests: Division without containing 0
    a0.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( a0.upper() / b0.lower(), BoundType::WEAK, a0.lower() / b0.upper(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( a1.upper() / b0.upper(), BoundType::WEAK, a1.lower() / b0.upper(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( a2.upper() / b0.upper(), BoundType::WEAK, a2.lower() / b0.lower(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( a4.upper() / b0.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( a5.upper() / b0.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a6.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a6.lower() / b0.upper(), BoundType::WEAK ), result1 );
    a7.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a7.lower() / b0.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b0, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( a0.lower() / b1.lower(), BoundType::WEAK, a0.upper() / b1.upper(), BoundType::WEAK ),
                          result1 );
    a1.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( a1.lower() / b1.lower(), BoundType::WEAK, a1.upper() / b1.lower(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( a2.lower() / b1.upper(), BoundType::WEAK, a2.upper() / b1.lower(), BoundType::WEAK ),
                          result1 );
    a3.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a4.upper() / b1.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a5.upper() / b1.lower(), BoundType::WEAK ), result1 );
    a6.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( a6.lower() / b1.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a7.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( a7.lower() / b1.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b1, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, a0.lower() / b2.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( a1.upper() / b2.upper(), BoundType::WEAK, a1.lower() / b2.upper(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( a2.upper() / b2.upper(), BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a3.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( a5.upper() / b2.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a6.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a6.lower() / b2.upper(), BoundType::WEAK ), result1 );
    a7.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b2, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    a0.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( a0.lower() / b3.lower(), BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a1.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( a1.lower() / b3.lower(), BoundType::WEAK, a1.upper() / b3.lower(), BoundType::WEAK ),
                          result1 );
    a2.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, a2.upper() / b3.lower(), BoundType::WEAK ), result1 );
    a3.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 0, BoundType::WEAK ), result1 );
    a4.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a5.upper() / b3.lower(), BoundType::WEAK ), result1 );
    a6.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( a6.lower() / b3.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a7.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b3, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 1, BoundType::INFTY ), result1 );

    //Table 8 tests with division containin 0
    a0.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::emptyInterval(), result1 );
    a1.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::emptyInterval(), result1 );

    a4.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::emptyInterval(), result1 );
    a5.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::emptyInterval(), result1 );
    a8.div_ext( b4, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval( a0.upper() / b5.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a2.lower() / b5.lower(), BoundType::WEAK ), result1 );

    a4.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval( a4.upper() / b5.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a7.lower() / b5.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b5, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval( a0.upper() / b6.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a0.upper() / b6.upper(), BoundType::WEAK ), result2 );
    a1.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a2.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( clRA_Interval( a2.lower() / b6.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a4.upper() / b6.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( clRA_Interval( a4.upper() / b6.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a7.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( clRA_Interval( a7.lower() / b6.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( b6, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a0.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval( a2.lower() / b7.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a4.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval( a7.lower() / b7.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b7, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );

    a4.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b8, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a0.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( clRA_Interval( a2.lower() / b9.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a4.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( clRA_Interval( a7.lower() / b9.upper(), BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( b9, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( clRA_Interval( a0.upper() / b10.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a2.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( clRA_Interval( a4.upper() / b10.lower(), BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, a7.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( b10, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a1.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b11, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a0.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a1.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a2.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );

    a4.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a5.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a6.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a7.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
    a8.div_ext( b12, result1, result2 );
    EXPECT_EQ( clRA_Interval::unboundedInterval(), result1 );
}

TEST(clRA_Interval, Intersection)
{
    clRA_Interval a1(-1,BoundType::WEAK,1,BoundType::WEAK);

    clRA_Interval b01(2,BoundType::WEAK,3,BoundType::WEAK);
    clRA_Interval b02(1,BoundType::WEAK,3,BoundType::WEAK);
    clRA_Interval b03(0,BoundType::WEAK,3,BoundType::WEAK);
    clRA_Interval b04(-1,BoundType::WEAK,1,BoundType::WEAK);
    clRA_Interval b05(-2,BoundType::WEAK,0,BoundType::WEAK);

    clRA_Interval b06(-2,BoundType::WEAK,-1,BoundType::WEAK);
    clRA_Interval b07(-3,BoundType::WEAK,-2,BoundType::WEAK);
    clRA_Interval b08(-1,BoundType::WEAK,-1,BoundType::WEAK);
    clRA_Interval b09(1,BoundType::WEAK,1,BoundType::WEAK);
    clRA_Interval b10(0,BoundType::WEAK,1,BoundType::WEAK);

    clRA_Interval b11(-1,BoundType::WEAK,0,BoundType::WEAK);
    clRA_Interval b12(-0.5,BoundType::WEAK,0.5,BoundType::WEAK);
    clRA_Interval b13(-2,BoundType::WEAK,2,BoundType::WEAK);
    clRA_Interval b14(0,BoundType::STRICT,0,BoundType::STRICT);
    clRA_Interval b15(-1,BoundType::INFTY,1,BoundType::INFTY);

    clRA_Interval b16(-1,BoundType::INFTY,-1,BoundType::WEAK);
    clRA_Interval b17(-1,BoundType::INFTY,0,BoundType::WEAK);
    clRA_Interval b18(-1,BoundType::INFTY,1,BoundType::WEAK);
    clRA_Interval b19(-1,BoundType::WEAK,1,BoundType::INFTY);
    clRA_Interval b20(0,BoundType::WEAK,1,BoundType::INFTY);

    clRA_Interval b21(1,BoundType::WEAK,1,BoundType::INFTY);

    EXPECT_EQ(clRA_Interval::emptyInterval(), carl::set_intersection(a1, b01));
    EXPECT_EQ(clRA_Interval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b02));
    EXPECT_EQ(clRA_Interval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b03));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b04));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b05));

    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b06));
    EXPECT_EQ(clRA_Interval::emptyInterval(), carl::set_intersection(a1, b07));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b08));
    EXPECT_EQ(clRA_Interval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b09));
    EXPECT_EQ(clRA_Interval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b10));

    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b11));
    EXPECT_EQ(clRA_Interval(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), carl::set_intersection(a1, b12));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b13));
    EXPECT_EQ(clRA_Interval::emptyInterval(), carl::set_intersection(a1, b14));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b15));

    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b16));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b17));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b18));
    EXPECT_EQ(clRA_Interval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b19));
    EXPECT_EQ(clRA_Interval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b20));

    EXPECT_EQ(clRA_Interval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b21));
}

TEST(clRA_Interval, Union)
{
    clRA_Interval i1(3, BoundType::WEAK, 5, BoundType::WEAK);
    clRA_Interval i2(1, BoundType::WEAK, 4, BoundType::WEAK);
    clRA_Interval i3(-2, BoundType::WEAK, 1, BoundType::WEAK);
    clRA_Interval i4(4, BoundType::STRICT, 9, BoundType::STRICT);
    clRA_Interval i5(1, BoundType::STRICT, 4, BoundType::STRICT);
    clRA_Interval i6(3, BoundType::STRICT, 3, BoundType::INFTY);
    clRA_Interval result1, result2;

    EXPECT_FALSE(i1.unite(i2, result1, result2));
    EXPECT_EQ(clRA_Interval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(i2.unite(i1, result1, result2));
    EXPECT_EQ(clRA_Interval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_TRUE(i1.unite(i3, result1, result2));
    EXPECT_EQ(clRA_Interval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(clRA_Interval(3, BoundType::WEAK, 5, BoundType::WEAK), result2);

    EXPECT_FALSE(i3.unite(i2, result1, result2));
    EXPECT_EQ(clRA_Interval(-2, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_FALSE(i4.unite(i1, result1, result2));
    EXPECT_EQ(clRA_Interval(3, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_TRUE(i3.unite(i4, result1, result2));
    EXPECT_EQ(clRA_Interval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(clRA_Interval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(i2.unite(i4, result1, result2));
    EXPECT_EQ(clRA_Interval(1, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_FALSE(i2.unite(i5, result1, result2));
    EXPECT_EQ(clRA_Interval(1, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_TRUE(i5.unite(i4, result1, result2));
    EXPECT_EQ(clRA_Interval(1, BoundType::STRICT, 4, BoundType::STRICT), result1);
    EXPECT_EQ(clRA_Interval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(i6.unite(i1, result1, result2));
    EXPECT_EQ(clRA_Interval(3, BoundType::WEAK, 3, BoundType::INFTY), result1);

    EXPECT_TRUE(i6.unite(i3, result1, result2));
    EXPECT_EQ(clRA_Interval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(clRA_Interval(3, BoundType::STRICT, 3, BoundType::INFTY), result2);
}

TEST(clRA_Interval, Split)
{
    clRA_Interval i1(-1, BoundType::INFTY, 1, BoundType::INFTY);
    clRA_Interval i2(-1, BoundType::STRICT, 1, BoundType::STRICT);
    clRA_Interval i3(-1, BoundType::WEAK, 1, BoundType::WEAK);
    clRA_Interval i4(0, BoundType::STRICT, 0, BoundType::STRICT);

	std::pair<clRA_Interval, clRA_Interval> res;

	res = i1.split();
    EXPECT_EQ(clRA_Interval(0, BoundType::INFTY, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(clRA_Interval(0, BoundType::WEAK, 0, BoundType::INFTY), res.second);

    res = i2.split();
    EXPECT_EQ(clRA_Interval(-1, BoundType::STRICT, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(clRA_Interval(0, BoundType::WEAK, 1, BoundType::STRICT), res.second);

    res = i3.split();
    EXPECT_EQ(clRA_Interval(-1, BoundType::WEAK, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(clRA_Interval(0, BoundType::WEAK, 1, BoundType::WEAK), res.second);

    res = i4.split();
    EXPECT_EQ(clRA_Interval(0, BoundType::STRICT, 0, BoundType::STRICT), res.first);
    EXPECT_EQ(clRA_Interval(0, BoundType::WEAK, 0, BoundType::STRICT), res.second);

    // uniform multi-split
    clRA_Interval i5(0,BoundType::WEAK, 5, BoundType::STRICT);

    std::list<clRA_Interval> results;
    results = i5.split(5);
    EXPECT_EQ(5, results.size());
    EXPECT_EQ(clRA_Interval(0, BoundType::WEAK, 1, BoundType::STRICT), *results.begin());
	results.pop_front();
    EXPECT_EQ(clRA_Interval(1, BoundType::WEAK, 2, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(clRA_Interval(2, BoundType::WEAK, 3, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(clRA_Interval(3, BoundType::WEAK, 4, BoundType::STRICT), *results.begin());
    results.pop_front();
	EXPECT_EQ(clRA_Interval(4, BoundType::WEAK, 5, BoundType::STRICT), *results.begin());
}

TEST(clRA_Interval, Properties)
{
    clRA_Interval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    clRA_Interval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    clRA_Interval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    clRA_Interval i4(-5, BoundType::WEAK, 3, BoundType::WEAK);

    // Diameter
    EXPECT_EQ(4, i1.diameter());
    EXPECT_EQ(8, i2.diameter());
    EXPECT_EQ(4, i3.diameter());
    EXPECT_EQ(8, i4.diameter());

    // Diameter ratio
    EXPECT_EQ((Rational)1/(Rational)2, i1.diameterRatio(i2));
    EXPECT_EQ(2, i2.diameterRatio(i1));
    EXPECT_EQ((Rational)1/(Rational)2, i3.diameterRatio(i2));
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

TEST(clRA_Interval, Contains)
{
    clRA_Interval i1(3, BoundType::STRICT, 7, BoundType::STRICT);
    clRA_Interval i2(-5, BoundType::STRICT, 3, BoundType::WEAK);
    clRA_Interval i3(3, BoundType::WEAK, 7, BoundType::STRICT);
    clRA_Interval i4(-5, BoundType::WEAK, 3, BoundType::WEAK);
    clRA_Interval i5(4, BoundType::STRICT, 5, BoundType::STRICT);
    clRA_Interval i6(3, BoundType::WEAK, 7, BoundType::WEAK);
    clRA_Interval i7(3, BoundType::STRICT, 4, BoundType::STRICT);
    clRA_Interval i8(3, BoundType::WEAK, 3, BoundType::INFTY);

    // Contains number
    EXPECT_TRUE(i1.contains(Rational(4)));
    EXPECT_FALSE(i1.contains(Rational(2)));
    EXPECT_FALSE(i1.contains(Rational(12)));
    EXPECT_FALSE(i1.contains(Rational(3)));
    EXPECT_FALSE(i1.contains(Rational(7)));

    EXPECT_TRUE(i2.contains(Rational(-1)));
    EXPECT_FALSE(i2.contains(Rational(-13)));
    EXPECT_FALSE(i2.contains(Rational(6)));
    EXPECT_FALSE(i2.contains(Rational(-5)));
    EXPECT_TRUE(i2.contains(Rational(3)));

    EXPECT_TRUE(i3.contains(Rational(4)));
    EXPECT_FALSE(i3.contains(Rational(2)));
    EXPECT_FALSE(i3.contains(Rational(12)));
    EXPECT_TRUE(i3.contains(Rational(3)));
    EXPECT_FALSE(i3.contains(Rational(7)));

    EXPECT_TRUE(i4.contains(Rational(-1)));
    EXPECT_FALSE(i4.contains(Rational(-13)));
    EXPECT_FALSE(i4.contains(Rational(6)));
    EXPECT_TRUE(i4.contains(Rational(-5)));
    EXPECT_TRUE(i4.contains(Rational(3)));

    EXPECT_FALSE(i8.contains(Rational(1)));
    EXPECT_TRUE(i8.contains(Rational(3)));
    EXPECT_TRUE(i8.contains(Rational(4)));
    EXPECT_TRUE(i8.contains(Rational(100)));
    EXPECT_FALSE(i8.contains(Rational(-2)));

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

TEST(clRA_Interval, BloatShrink)
{
    clRA_Interval i1(3, BoundType::WEAK, 7, BoundType::WEAK);
    clRA_Interval i2(-13, BoundType::STRICT, 1, BoundType::STRICT);
    clRA_Interval i3(0, BoundType::STRICT, 1, BoundType::STRICT);
    clRA_Interval i4(5, BoundType::STRICT, 13, BoundType::STRICT);
    clRA_Interval result1(-2, BoundType::WEAK, 12, BoundType::WEAK);
    clRA_Interval result2(-10, BoundType::STRICT, -2, BoundType::STRICT);
    clRA_Interval result4(7, BoundType::STRICT, 11, BoundType::STRICT);

    // Bloat by adding
    i1.bloat_by(5);
    EXPECT_EQ(result1, i1);
    i2.bloat_by(-3);
    EXPECT_EQ(result2, i2);

    // Shrink by subtracting
    i4.shrink_by(2);
    EXPECT_EQ(result4, i4);
}
