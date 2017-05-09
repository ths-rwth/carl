#include "../Common.h"

#include <carl/interval/Interval.h>

using namespace carl;

typedef Interval<Rational> clRA_Interval;

template<typename T>
class IntervalRationalTest: public testing::Test {};

TYPED_TEST_CASE(IntervalRationalTest, RationalTypes);

TYPED_TEST(IntervalRationalTest, Constructor) {
	using Interval = Interval<TypeParam>;
	
	{ // Interval()
		Interval i;
		EXPECT_EQ(BoundType::STRICT, i.lowerBoundType());
		EXPECT_EQ(BoundType::STRICT, i.upperBoundType());
		EXPECT_EQ(0, i.rContent().lower());
		EXPECT_EQ(0, i.rContent().upper());
	}
	{ // Interval(const Number&)
		Interval i0(TypeParam(0));
		EXPECT_EQ(BoundType::WEAK, i0.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i0.upperBoundType());
		EXPECT_EQ(0, i0.rContent().lower());
		EXPECT_EQ(0, i0.rContent().upper());
		
		Interval i1(TypeParam(1));
		EXPECT_EQ(BoundType::WEAK, i1.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i1.upperBoundType());
		EXPECT_EQ(1, i1.rContent().lower());
		EXPECT_EQ(1, i1.rContent().upper());
	}
	{ // Interval(const Number&, const Number&)
		Interval i0(TypeParam(1), TypeParam(1));
		EXPECT_EQ(BoundType::WEAK, i0.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i0.upperBoundType());
		EXPECT_EQ(1, i0.rContent().lower());
		EXPECT_EQ(1, i0.rContent().upper());
		
		Interval i1(TypeParam(1), TypeParam(2));
		EXPECT_EQ(BoundType::WEAK, i1.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i1.upperBoundType());
		EXPECT_EQ(1, i1.rContent().lower());
		EXPECT_EQ(2, i1.rContent().upper());
		
		Interval i2(TypeParam(2), TypeParam(1));
		EXPECT_EQ(BoundType::STRICT, i2.lowerBoundType());
		EXPECT_EQ(BoundType::STRICT, i2.upperBoundType());
		EXPECT_EQ(0, i2.rContent().lower());
		EXPECT_EQ(0, i2.rContent().upper());
		EXPECT_EQ(Interval::emptyInterval(), i2);
	}
}

TYPED_TEST(IntervalRationalTest, StaticConstructor) {
	using Interval = Interval<TypeParam>;
	{ // unboundedInterval()
		Interval i = Interval::unboundedInterval();
		EXPECT_EQ(BoundType::INFTY, i.lowerBoundType());
		EXPECT_EQ(BoundType::INFTY, i.upperBoundType());
		EXPECT_EQ(0, i.rContent().lower());
		EXPECT_EQ(0, i.rContent().upper());
	}
	{ // emptyInterval()
		Interval i = Interval::emptyInterval();
		EXPECT_EQ(BoundType::STRICT, i.lowerBoundType());
		EXPECT_EQ(BoundType::STRICT, i.upperBoundType());
		EXPECT_EQ(0, i.rContent().lower());
		EXPECT_EQ(0, i.rContent().upper());
	}
	{ // zeroInterval()
		Interval i = Interval::zeroInterval();
		EXPECT_EQ(BoundType::WEAK, i.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i.upperBoundType());
		EXPECT_EQ(0, i.rContent().lower());
		EXPECT_EQ(0, i.rContent().upper());
	}
}

TYPED_TEST(IntervalRationalTest, integralPart) {
	using Interval = Interval<TypeParam>;
	{
		Interval i = Interval::emptyInterval();
		EXPECT_EQ(i, i.integralPart());
	}
	{
		Interval i = Interval(-5, BoundType::STRICT, 5, BoundType::STRICT).integralPart();
		EXPECT_EQ(BoundType::WEAK, i.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i.upperBoundType());
		EXPECT_EQ(-4, i.lower());
		EXPECT_EQ(4, i.upper());
	}
	{
		Interval i = Interval(-5, BoundType::STRICT, 5, BoundType::WEAK).integralPart();
		EXPECT_EQ(BoundType::WEAK, i.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i.upperBoundType());
		EXPECT_EQ(-4, i.lower());
		EXPECT_EQ(5, i.upper());
	}
	{
		Interval i = Interval(-5, BoundType::WEAK, 5, BoundType::STRICT).integralPart();
		EXPECT_EQ(BoundType::WEAK, i.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i.upperBoundType());
		EXPECT_EQ(-5, i.lower());
		EXPECT_EQ(4, i.upper());
	}
	{
		Interval i = Interval(-5, BoundType::WEAK, 5, BoundType::WEAK).integralPart();
		EXPECT_EQ(BoundType::WEAK, i.lowerBoundType());
		EXPECT_EQ(BoundType::WEAK, i.upperBoundType());
		EXPECT_EQ(-5, i.lower());
		EXPECT_EQ(5, i.upper());
	}
}

TYPED_TEST(IntervalRationalTest, center) {
	using Interval = Interval<TypeParam>;
	{ // unbounded
		Interval i = Interval::unboundedInterval();
		EXPECT_EQ(0, i.center());
	}
	{ // bounded / unbounded
		Interval i1(-5, BoundType::WEAK, 0, BoundType::INFTY);
		EXPECT_TRUE(i1.center() >= -5);
		Interval i2(-5, BoundType::STRICT, 0, BoundType::INFTY);
		EXPECT_TRUE(i2.center() > -5);
		Interval i3(5, BoundType::WEAK, 0, BoundType::INFTY);
		EXPECT_TRUE(i3.center() >= 5);
		Interval i4(5, BoundType::STRICT, 0, BoundType::INFTY);
		EXPECT_TRUE(i4.center() > 5);
	}
	{ // unbounded / bounded
		Interval i1(0, BoundType::INFTY, -5, BoundType::WEAK);
		EXPECT_TRUE(i1.center() <= -5);
		Interval i2(0, BoundType::INFTY, -5, BoundType::STRICT);
		EXPECT_TRUE(i2.center() < -5);
		Interval i3(0, BoundType::INFTY, 5, BoundType::WEAK);
		EXPECT_TRUE(i3.center() <= 5);
		Interval i4(0, BoundType::INFTY, 5, BoundType::STRICT);
		EXPECT_TRUE(i4.center() < 5);
	}
	{ // weak / weak
		Interval i1(-10, BoundType::WEAK, -5, BoundType::WEAK);
		EXPECT_TRUE(-10 <= i1.center() && i1.center() <= -5);
		Interval i2(-5, BoundType::WEAK, 0, BoundType::WEAK);
		EXPECT_TRUE(-5 <= i2.center() && i2.center() <= 0);
		Interval i3(-5, BoundType::WEAK, 5, BoundType::WEAK);
		EXPECT_TRUE(-5 <= i3.center() && i3.center() <= 5);
		Interval i4(0, BoundType::WEAK, 5, BoundType::WEAK);
		EXPECT_TRUE(0 <= i4.center() && i4.center() <= 5);
		Interval i5(5, BoundType::WEAK, 10, BoundType::WEAK);
		EXPECT_TRUE(5 <= i5.center() && i5.center() <= 10);
	}
	{ // weak / strict
		Interval i1(-10, BoundType::WEAK, -5, BoundType::STRICT);
		EXPECT_TRUE(-10 <= i1.center() && i1.center() < -5);
		Interval i2(-5, BoundType::WEAK, 0, BoundType::STRICT);
		EXPECT_TRUE(-5 <= i2.center() && i2.center() < 0);
		Interval i3(-5, BoundType::WEAK, 5, BoundType::STRICT);
		EXPECT_TRUE(-5 <= i3.center() && i3.center() < 5);
		Interval i4(0, BoundType::WEAK, 5, BoundType::STRICT);
		EXPECT_TRUE(0 <= i4.center() && i4.center() < 5);
		Interval i5(5, BoundType::WEAK, 10, BoundType::STRICT);
		EXPECT_TRUE(5 <= i5.center() && i5.center() < 10);
	}
	{ // strict / weak
		Interval i1(-10, BoundType::STRICT, -5, BoundType::WEAK);
		EXPECT_TRUE(-10 < i1.center() && i1.center() <= -5);
		Interval i2(-5, BoundType::STRICT, 0, BoundType::WEAK);
		EXPECT_TRUE(-5 < i2.center() && i2.center() <= 0);
		Interval i3(-5, BoundType::STRICT, 5, BoundType::WEAK);
		EXPECT_TRUE(-5 < i3.center() && i3.center() <= 5);
		Interval i4(0, BoundType::STRICT, 5, BoundType::WEAK);
		EXPECT_TRUE(0 < i4.center() && i4.center() <= 5);
		Interval i5(5, BoundType::STRICT, 10, BoundType::WEAK);
		EXPECT_TRUE(5 < i5.center() && i5.center() <= 10);
	}
	{ // strict / strict
		Interval i1(-10, BoundType::STRICT, -5, BoundType::STRICT);
		EXPECT_TRUE(-10 < i1.center() && i1.center() < -5);
		Interval i2(-5, BoundType::STRICT, 0, BoundType::STRICT);
		EXPECT_TRUE(-5 < i2.center() && i2.center() < 0);
		Interval i3(-5, BoundType::STRICT, 5, BoundType::STRICT);
		EXPECT_TRUE(-5 < i3.center() && i3.center() < 5);
		Interval i4(0, BoundType::STRICT, 5, BoundType::STRICT);
		EXPECT_TRUE(0 < i4.center() && i4.center() < 5);
		Interval i5(5, BoundType::STRICT, 10, BoundType::STRICT);
		EXPECT_TRUE(5 < i5.center() && i5.center() < 10);
	}
}
