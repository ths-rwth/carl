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
#include <carl/interval/Interval.h>
#include <carl/interval/set_theory.h>
#include <carl/core/VariablePool.h>
#include <iostream>
#include <carl-common/meta/platform.h>

#include "../number_types.h"

using namespace carl;

typedef Interval<double> DoubleInterval;

TEST(DoubleInterval, Constructor)
{
    DoubleInterval test1 = DoubleInterval(-1, BoundType::WEAK, 1, BoundType::WEAK);
    DoubleInterval test2 = DoubleInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval test3 = DoubleInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, -1, BoundType::WEAK), DoubleInterval::empty_interval());
    DoubleInterval test5 = DoubleInterval::unbounded_interval();
    DoubleInterval test6 = DoubleInterval::empty_interval();

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
    UNUSED(hash1);
    /*DoubleInterval test2 = DoubleInterval(-1, BoundType::STRICT, 1, BoundType::STRICT);
    DoubleInterval test3 = DoubleInterval(-1, BoundType::INFTY, 1, BoundType::INFTY);
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, -1, BoundType::WEAK), DoubleInterval::empty_interval());
    DoubleInterval test5 = DoubleInterval::unbounded_interval();
    DoubleInterval test6 = DoubleInterval::empty_interval();

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
    EXPECT_EQ(BoundType::WEAK, test1.lower_bound_type());
    EXPECT_EQ(BoundType::STRICT, test1.upper_bound_type());
    EXPECT_EQ(-1, test2.lower());
    EXPECT_EQ(1, test2.upper());
    EXPECT_EQ(BoundType::WEAK, test2.lower_bound_type());
    EXPECT_EQ(BoundType::STRICT, test2.upper_bound_type());
    EXPECT_TRUE(test3.is_empty());
    EXPECT_EQ(BoundType::STRICT, test4.lower_bound_type());
    EXPECT_EQ(BoundType::STRICT, test4.upper_bound_type());
    EXPECT_EQ(0, test4.lower());
    EXPECT_EQ(0, test4.upper());
    EXPECT_TRUE(test4.is_empty());
    EXPECT_EQ(0, test5.lower());
    EXPECT_EQ(0, test5.upper());
    EXPECT_TRUE(test5.is_empty());
    EXPECT_EQ(4, test6.lower());
    EXPECT_EQ(4, test6.upper());

    test1.set_lower(-3);
    test1.set_upper(5);
    test1.set_lower_bound_type(BoundType::STRICT);
    test1.set_upper_bound_type(BoundType::WEAK);
    EXPECT_EQ(-3, test1.lower());
    EXPECT_EQ(5, test1.upper());
    EXPECT_EQ(BoundType::STRICT, test1.lower_bound_type());
    EXPECT_EQ(BoundType::WEAK, test1.upper_bound_type());

    test1.set(4, 8);
    EXPECT_EQ(4, test1.lower());
    EXPECT_EQ(8, test1.upper());

    test1.set_lower_bound_type(BoundType::INFTY);
    test1.set_upper_bound_type(BoundType::INFTY);
    EXPECT_TRUE(test1.is_infinite());

    test2.set_upper_bound_type(BoundType::INFTY);
    EXPECT_EQ(BoundType::INFTY, test2.upper_bound_type());
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
    DoubleInterval i1 = DoubleInterval::empty_interval();
    DoubleInterval i2 = DoubleInterval::unbounded_interval();
	DoubleInterval i3 = carl::constant_one<DoubleInterval>().get();
	DoubleInterval i4 = carl::constant_zero<DoubleInterval>().get();

    EXPECT_TRUE(i1.is_empty());
    EXPECT_EQ(0, i1.lower());
    EXPECT_EQ(0, i1.upper());

    EXPECT_TRUE(i2.is_infinite());
    EXPECT_EQ(BoundType::INFTY, i2.lower_bound_type());
    EXPECT_EQ(BoundType::INFTY, i2.upper_bound_type());

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
	EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a0.add(b3);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);

    result = a1.add(b0);
    EXPECT_EQ( DoubleInterval(4, BoundType::INFTY, 4, BoundType::WEAK), result);
    result = a1.add(b1);
	EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result);
    result = a1.add(b2);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a1.add(b3);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);

    result = a2.add(b0);
	EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a2.add(b1);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b2);
    EXPECT_EQ( DoubleInterval(-2, BoundType::WEAK, -2, BoundType::INFTY), result);
    result = a2.add(b3);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);

    result = a3.add(b0);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a3.add(b1);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a3.add(b2);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a3.add(b3);
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a0.add(b1.inverse());
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b2.inverse());
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a0.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);

    result = a1.add(b0.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a1.add(b1.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 3, BoundType::WEAK), result);
    result = a1.add(b2.inverse());
    EXPECT_EQ( DoubleInterval(-1, BoundType::INFTY, 3, BoundType::WEAK), result);
    result = a1.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);

    result = a2.add(b0.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b1.inverse());
    EXPECT_EQ( DoubleInterval(-3, BoundType::WEAK, 1, BoundType::INFTY), result);
    result = a2.add(b2.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a2.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);

    result = a3.add(b0.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a3.add(b1.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a3.add(b2.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
    result = a3.add(b3.inverse());
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result);
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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d5 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d6 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d6 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d6 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d6 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d7 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c7.lower() * d2.upper(), BoundType::WEAK ), result );
    result = c7.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c7.lower() * d8.upper(), BoundType::WEAK ), result );
    result = c8.mul( d2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

    result = c0.mul( d9 );
    EXPECT_EQ( DoubleInterval( c0.lower() * d9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c0.mul( d8 );
    EXPECT_EQ( DoubleInterval( c0.lower() * d8.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c1.mul( d9 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c1.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c2.mul( d9 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c2.upper() * d9.upper(), BoundType::WEAK ), result );
    result = c2.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c2.upper() * d8.upper(), BoundType::WEAK ), result );
    result = c3.mul( d9 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c3.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d9 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c4.mul( d8 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c5.mul( d9 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d9 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d9 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d8 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result );
    result = c8.mul( d9 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d8 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.lower() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d10 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.lower() * d10.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c1.mul( d10 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d10 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d10 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result );
    result = c7.mul( d10 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d10 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

    result = c0.mul( d11 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.upper() * d11.lower(), BoundType::WEAK ), result );
    result = c0.mul( d3 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, c0.upper() * d3.lower(), BoundType::WEAK ), result );
    result = c1.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c1.mul( d3 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
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
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d3 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d3 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d11 );
    EXPECT_EQ( DoubleInterval( c7.lower() * d11.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c7.mul( d3 );
    EXPECT_EQ( DoubleInterval( c7.lower() * d3.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result );
    result = c8.mul( d11 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d3 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

    result = c0.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c1.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c2.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c3.mul( d12 );
    EXPECT_EQ( DoubleInterval( 0 ), result );
    result = c4.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c5.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c6.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c7.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );
    result = c8.mul( d12 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result );

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
    EXPECT_EQ( DoubleInterval::empty_interval(), result1 );
    a1.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::empty_interval(), result1 );

    a4.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::empty_interval(), result1 );
    a5.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::empty_interval(), result1 );
    a8.div_ext( b4, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a1.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a2.lower() / b5.lower(), BoundType::WEAK ), result1 );

    a4.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b5.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b5.lower(), BoundType::WEAK ), result1 );
    a8.div_ext( b5, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b6.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a0.upper() / b6.upper(), BoundType::WEAK ), result2 );
    a1.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a2.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b6.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );

    a4.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b6.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b6.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b6.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b6.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );
    a8.div_ext( b6, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a0.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a1.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );

    a4.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b7.upper(), BoundType::WEAK ), result1 );
    a5.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b7.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a8.div_ext( b7, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );

    a4.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a8.div_ext( b8, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a0.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a1.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a2.lower() / b9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );

    a4.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a4.upper() / b9.upper(), BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a5.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( a7.lower() / b9.upper(), BoundType::WEAK, 1.0, BoundType::INFTY ), result2 );
    a8.div_ext( b9, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a0.upper() / b10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a1.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a2.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );

    a4.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result2 );
    EXPECT_EQ( DoubleInterval( a4.upper() / b10.lower(), BoundType::WEAK, 1.0, BoundType::INFTY ), result1 );
    a5.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1.0, BoundType::INFTY, a7.lower() / b10.lower(), BoundType::WEAK ), result1 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result2 );
    a8.div_ext( b10, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a1.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );

    a4.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( -1, BoundType::INFTY, 0, BoundType::WEAK ), result1 );
    a5.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval( 0, BoundType::WEAK, 1, BoundType::INFTY ), result1 );
    a8.div_ext( b11, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a0.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a1.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a2.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );

    a4.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a5.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a6.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a7.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
    a8.div_ext( b12, result1, result2 );
    EXPECT_EQ( DoubleInterval::unbounded_interval(), result1 );
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

    EXPECT_EQ(DoubleInterval::empty_interval(), carl::set_intersection(a1, b01));
    EXPECT_EQ(DoubleInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b02));
    EXPECT_EQ(DoubleInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b03));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b04));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b05));

    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b06));
    EXPECT_EQ(DoubleInterval::empty_interval(), carl::set_intersection(a1, b07));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b08));
    EXPECT_EQ(DoubleInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b09));
    EXPECT_EQ(DoubleInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b10));

    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b11));
    EXPECT_EQ(DoubleInterval(-0.5,BoundType::WEAK,0.5,BoundType::WEAK), carl::set_intersection(a1, b12));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b13));
    EXPECT_EQ(DoubleInterval::empty_interval(), carl::set_intersection(a1, b14));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b15));

    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,-1,BoundType::WEAK), carl::set_intersection(a1, b16));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,0,BoundType::WEAK), carl::set_intersection(a1, b17));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b18));
    EXPECT_EQ(DoubleInterval(-1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b19));
    EXPECT_EQ(DoubleInterval(0,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b20));

    EXPECT_EQ(DoubleInterval(1,BoundType::WEAK,1,BoundType::WEAK), carl::set_intersection(a1, b21));
}

TEST(DoubleInterval, HaveIntersection)
{
    // have_intersection is symmetric, just test one side.
    DoubleInterval a1(-1,BoundType::WEAK,1,BoundType::WEAK);
    DoubleInterval a2(0,BoundType::WEAK,1,BoundType::WEAK);
    DoubleInterval a3(1,BoundType::WEAK,2,BoundType::WEAK);
    DoubleInterval a4(2,BoundType::WEAK,3,BoundType::WEAK);
    DoubleInterval a5(2,BoundType::INFTY,3,BoundType::WEAK);
    DoubleInterval a6(1,BoundType::STRICT,3,BoundType::WEAK);
    DoubleInterval a7(1,BoundType::INFTY,-1,BoundType::WEAK);
    DoubleInterval a8(-1,BoundType::INFTY,-1,BoundType::STRICT);
    DoubleInterval a9(-1,BoundType::INFTY,1,BoundType::INFTY);
    DoubleInterval a10(-1,BoundType::INFTY,-2,BoundType::WEAK);


    EXPECT_TRUE(carl::set_have_intersection(a1,a1));
    EXPECT_TRUE(carl::set_have_intersection(a1,a2));
    EXPECT_TRUE(carl::set_have_intersection(a1,a3));
    EXPECT_FALSE(carl::set_have_intersection(a1,a4));
    EXPECT_TRUE(carl::set_have_intersection(a1,a5));
    EXPECT_FALSE(carl::set_have_intersection(a1,a6));
    EXPECT_TRUE(carl::set_have_intersection(a1,a7));
    EXPECT_FALSE(carl::set_have_intersection(a1,a8));
    EXPECT_TRUE(carl::set_have_intersection(a1,a9));
    EXPECT_FALSE(carl::set_have_intersection(a1,a10));
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

    EXPECT_FALSE(carl::set_union(i1, i2, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_union(i2, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_TRUE(carl::set_union(i1, i3, result1, result2));
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_union(i3, i2, result1, result2));
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_union(i4, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_TRUE(carl::set_union(i3, i4, result1, result2));
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(carl::set_union(i2, i4, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 9, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_union(i2, i5, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 4, BoundType::WEAK), result1);

    EXPECT_TRUE(carl::set_union(i5, i4, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::STRICT, 4, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 9, BoundType::STRICT), result2);

    EXPECT_FALSE(carl::set_union(i6, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 3, BoundType::INFTY), result1);

    EXPECT_TRUE(carl::set_union(i6, i3, result1, result2));
    EXPECT_EQ(DoubleInterval(-2, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 3, BoundType::INFTY), result2);

    EXPECT_FALSE(carl::set_union(i1, i7, result1, result2));
    EXPECT_EQ(DoubleInterval::unbounded_interval(), result1);

    EXPECT_FALSE(carl::set_union(i7, i6, result1, result2));
    EXPECT_EQ(DoubleInterval::unbounded_interval(), result1);
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

    EXPECT_FALSE(carl::set_difference(i1, i2, result1, result2));
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i2, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 3, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_difference(i1, i3, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i3, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 2, BoundType::WEAK), result1);

    EXPECT_TRUE(carl::set_difference(i2, i4, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_difference(i4, i2, result1, result2));
    EXPECT_EQ(DoubleInterval::empty_interval(), result1);

    EXPECT_FALSE(carl::set_difference(i5, i2, result1, result2));
    EXPECT_EQ(DoubleInterval::empty_interval(), result1);

    EXPECT_TRUE(carl::set_difference(i2, i5, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 1, BoundType::WEAK), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_difference(i1, i1, result1, result2));
    EXPECT_EQ(DoubleInterval::empty_interval(), result1);

    EXPECT_FALSE(carl::set_difference(i6, i2, result1, result2));
    EXPECT_EQ(DoubleInterval(4, BoundType::STRICT, 4, BoundType::INFTY), result1);

    EXPECT_FALSE(carl::set_difference(i2, i6, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 2, BoundType::STRICT), result1);

    EXPECT_TRUE(carl::set_difference(i6, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(2, BoundType::WEAK, 3, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(5, BoundType::STRICT, 5, BoundType::INFTY), result2);

    EXPECT_FALSE(carl::set_difference(i1, i6, result1, result2));
    EXPECT_EQ(DoubleInterval::empty_interval(), result1);

    EXPECT_FALSE(carl::set_difference(i3, i6, result1, result2));
    EXPECT_EQ(DoubleInterval(-1, BoundType::WEAK, 2, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_difference(i7, i2, result1, result2));
    EXPECT_EQ(DoubleInterval::empty_interval(), result1);

    EXPECT_TRUE(carl::set_difference(i2, i7, result1, result2));
    EXPECT_EQ(DoubleInterval(1, BoundType::WEAK, 3, BoundType::STRICT), result1);
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 4, BoundType::WEAK), result2);

    EXPECT_FALSE(carl::set_difference(i1, i7, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::STRICT, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i8, i1, result1, result2));
    EXPECT_EQ(DoubleInterval(0, BoundType::STRICT, 0, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_difference(i1, i8, result1, result2));
    EXPECT_EQ(DoubleInterval(3, BoundType::WEAK, 5, BoundType::WEAK), result1);

    EXPECT_FALSE(carl::set_difference(i8, i3, result1, result2));
    EXPECT_EQ(DoubleInterval(0, BoundType::STRICT, 0, BoundType::STRICT), result1);

    EXPECT_FALSE(carl::set_difference(i3, i8, result1, result2));
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

	DoubleInterval i6(13,BoundType::WEAK, 20, BoundType::WEAK);

    results = i6.split(7);
    EXPECT_EQ((unsigned)7, results.size());
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
    EXPECT_EQ(0.5, i1.diameter_ratio(i2));
    EXPECT_EQ(2, i2.diameter_ratio(i1));
    EXPECT_EQ(0.5, i3.diameter_ratio(i2));
    EXPECT_EQ(2, i4.diameter_ratio(i1));

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
    EXPECT_FALSE(carl::set_is_subset(i1, i2));
    EXPECT_FALSE(carl::set_is_subset(i2, i1));
    EXPECT_TRUE(carl::set_is_subset(i1, i3));
    EXPECT_FALSE(carl::set_is_subset(i3, i1));
    EXPECT_FALSE(carl::set_is_subset(i1, i5));
    EXPECT_TRUE(carl::set_is_subset(i5, i1));
    EXPECT_TRUE(carl::set_is_subset(i1, i6));
    EXPECT_FALSE(carl::set_is_subset(i6, i1));
    EXPECT_TRUE(carl::set_is_subset(i1, i1));
    EXPECT_TRUE(carl::set_is_subset(i6, i6));
    EXPECT_FALSE(carl::set_is_subset(i1, i7));
    EXPECT_TRUE(carl::set_is_subset(i7, i1));
    EXPECT_FALSE(carl::set_is_subset(i6, i7));
    EXPECT_TRUE(carl::set_is_subset(i7, i6));
    EXPECT_FALSE(carl::set_is_subset(i8, i5));
    EXPECT_TRUE(carl::set_is_subset(i5, i8));
    EXPECT_FALSE(carl::set_is_subset(i8, i6));
    EXPECT_TRUE(carl::set_is_subset(i6, i8));
    EXPECT_FALSE(carl::set_is_subset(i8, i4));
    EXPECT_FALSE(carl::set_is_subset(i4, i8));

    EXPECT_FALSE(set_is_proper_subset(i1, i2));
    EXPECT_FALSE(set_is_proper_subset(i2, i1));
    EXPECT_TRUE(set_is_proper_subset(i1, i3));
    EXPECT_FALSE(set_is_proper_subset(i3, i1));
    EXPECT_FALSE(set_is_proper_subset(i1, i5));
    EXPECT_TRUE(set_is_proper_subset(i5, i1));
    EXPECT_TRUE(set_is_proper_subset(i1, i6));
    EXPECT_FALSE(set_is_proper_subset(i6, i1));
    EXPECT_FALSE(set_is_proper_subset(i6, i6));
    EXPECT_FALSE(set_is_proper_subset(i1, i1));
    EXPECT_FALSE(set_is_proper_subset(i1, i7));
    EXPECT_TRUE(set_is_proper_subset(i7, i1));
    EXPECT_FALSE(set_is_proper_subset(i6, i7));
    EXPECT_TRUE(set_is_proper_subset(i7, i6));
    EXPECT_FALSE(set_is_proper_subset(i8, i5));
    EXPECT_TRUE(set_is_proper_subset(i5, i8));
    EXPECT_FALSE(set_is_proper_subset(i8, i6));
    EXPECT_TRUE(set_is_proper_subset(i6, i8));
    EXPECT_FALSE(set_is_proper_subset(i8, i4));
    EXPECT_FALSE(set_is_proper_subset(i4, i8));
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
