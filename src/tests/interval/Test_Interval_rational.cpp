#include <gtest/gtest.h>

#include <carl-arith/interval/Interval.h>
#include <carl-arith/interval/set_theory.h>

#include "../number_types.h"

using namespace carl;

typedef Interval<Rational> clRA_Interval;

template<typename T>
class IntervalRationalTest: public testing::Test {};

#define ZERO TypeParam(0)
#define HALF TypeParam(1)/TypeParam(2)
#define ONE TypeParam(1)

TYPED_TEST_CASE(IntervalRationalTest, RationalTypes);

TYPED_TEST(IntervalRationalTest, Constructor) {
	using Interval = Interval<TypeParam>;

	{ // Interval()
		Interval i;
		EXPECT_EQ(BoundType::STRICT, i.lower_bound_type());
		EXPECT_EQ(BoundType::STRICT, i.upper_bound_type());
		EXPECT_EQ(0, i.content().lower());
		EXPECT_EQ(0, i.content().upper());
	}
	{ // Interval(const Number&)
		Interval i0(TypeParam(0));
		EXPECT_EQ(BoundType::WEAK, i0.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i0.upper_bound_type());
		EXPECT_EQ(0, i0.content().lower());
		EXPECT_EQ(0, i0.content().upper());

		Interval i1(TypeParam(1));
		EXPECT_EQ(BoundType::WEAK, i1.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i1.upper_bound_type());
		EXPECT_EQ(1, i1.content().lower());
		EXPECT_EQ(1, i1.content().upper());
	}
	{ // Interval(const Number&, const Number&)
		Interval i0(TypeParam(1), TypeParam(1));
		EXPECT_EQ(BoundType::WEAK, i0.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i0.upper_bound_type());
		EXPECT_EQ(1, i0.content().lower());
		EXPECT_EQ(1, i0.content().upper());

		Interval i1(TypeParam(1), TypeParam(2));
		EXPECT_EQ(BoundType::WEAK, i1.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i1.upper_bound_type());
		EXPECT_EQ(1, i1.content().lower());
		EXPECT_EQ(2, i1.content().upper());

		Interval i2(TypeParam(2), TypeParam(1));
		EXPECT_EQ(BoundType::STRICT, i2.lower_bound_type());
		EXPECT_EQ(BoundType::STRICT, i2.upper_bound_type());
		EXPECT_EQ(0, i2.content().lower());
		EXPECT_EQ(0, i2.content().upper());
		EXPECT_EQ(Interval::empty_interval(), i2);
	}
}

TYPED_TEST(IntervalRationalTest, StaticConstructor) {
	using Interval = Interval<TypeParam>;
	{ // unbounded_interval()
		Interval i = Interval::unbounded_interval();
		EXPECT_EQ(BoundType::INFTY, i.lower_bound_type());
		EXPECT_EQ(BoundType::INFTY, i.upper_bound_type());
		EXPECT_EQ(0, i.content().lower());
		EXPECT_EQ(0, i.content().upper());
	}
	{ // empty_interval()
		Interval i = Interval::empty_interval();
		EXPECT_EQ(BoundType::STRICT, i.lower_bound_type());
		EXPECT_EQ(BoundType::STRICT, i.upper_bound_type());
		EXPECT_EQ(0, i.content().lower());
		EXPECT_EQ(0, i.content().upper());
	}
	{ // zero_interval()
		Interval i = Interval::zero_interval();
		EXPECT_EQ(BoundType::WEAK, i.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i.upper_bound_type());
		EXPECT_EQ(0, i.content().lower());
		EXPECT_EQ(0, i.content().upper());
	}
}

TYPED_TEST(IntervalRationalTest, integralPart) {
	using Interval = Interval<TypeParam>;
	{
		Interval i = Interval::empty_interval();
		EXPECT_EQ(i, i.integral_part());
	}
	{
		Interval i = Interval(-5, BoundType::STRICT, 5, BoundType::STRICT).integral_part();
		EXPECT_EQ(BoundType::WEAK, i.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i.upper_bound_type());
		EXPECT_EQ(-4, i.lower());
		EXPECT_EQ(4, i.upper());
	}
	{
		Interval i = Interval(-5, BoundType::STRICT, 5, BoundType::WEAK).integral_part();
		EXPECT_EQ(BoundType::WEAK, i.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i.upper_bound_type());
		EXPECT_EQ(-4, i.lower());
		EXPECT_EQ(5, i.upper());
	}
	{
		Interval i = Interval(-5, BoundType::WEAK, 5, BoundType::STRICT).integral_part();
		EXPECT_EQ(BoundType::WEAK, i.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i.upper_bound_type());
		EXPECT_EQ(-5, i.lower());
		EXPECT_EQ(4, i.upper());
	}
	{
		Interval i = Interval(-5, BoundType::WEAK, 5, BoundType::WEAK).integral_part();
		EXPECT_EQ(BoundType::WEAK, i.lower_bound_type());
		EXPECT_EQ(BoundType::WEAK, i.upper_bound_type());
		EXPECT_EQ(-5, i.lower());
		EXPECT_EQ(5, i.upper());
	}
}

TYPED_TEST(IntervalRationalTest, containsInteger) {
	using Interval = Interval<TypeParam>;
	// For every bound type combination: [int/non-int] * [int/non-int] * [yes/no]
	{ // strict / strict
		Interval i1(0, BoundType::STRICT, 2, BoundType::STRICT);
		Interval i2(0, BoundType::STRICT, 1, BoundType::STRICT);
		Interval i3(0, BoundType::STRICT, 1 + HALF, BoundType::STRICT);
		Interval i4(0, BoundType::STRICT, HALF, BoundType::STRICT);
		Interval i5(HALF, BoundType::STRICT, 2, BoundType::STRICT);
		Interval i6(HALF, BoundType::STRICT, 1, BoundType::STRICT);
		Interval i7(HALF, BoundType::STRICT, 1 + HALF, BoundType::STRICT);
		Interval i8(HALF, BoundType::STRICT, HALF, BoundType::STRICT);
		EXPECT_TRUE(i1.contains_integer());
		EXPECT_FALSE(i2.contains_integer());
		EXPECT_TRUE(i3.contains_integer());
		EXPECT_FALSE(i4.contains_integer());
		EXPECT_TRUE(i5.contains_integer());
		EXPECT_FALSE(i6.contains_integer());
		EXPECT_TRUE(i7.contains_integer());
		EXPECT_FALSE(i8.contains_integer());
	}
	{ // strict / weak
		Interval i1(0, BoundType::STRICT, 1, BoundType::WEAK);
		Interval i3(0, BoundType::STRICT, 1 + HALF, BoundType::WEAK);
		Interval i4(0, BoundType::STRICT, HALF, BoundType::WEAK);
		Interval i5(HALF, BoundType::STRICT, 1, BoundType::WEAK);
		Interval i7(HALF, BoundType::STRICT, 1 + HALF, BoundType::WEAK);
		Interval i8(HALF, BoundType::STRICT, HALF, BoundType::WEAK);
		EXPECT_TRUE(i1.contains_integer());
		EXPECT_TRUE(i3.contains_integer());
		EXPECT_FALSE(i4.contains_integer());
		EXPECT_TRUE(i5.contains_integer());
		EXPECT_TRUE(i7.contains_integer());
		EXPECT_FALSE(i8.contains_integer());
	}
	{ // strict / infty
		Interval i1(0, BoundType::STRICT, 0, BoundType::INFTY);
		Interval i2(HALF, BoundType::STRICT, 0, BoundType::INFTY);
		EXPECT_TRUE(i1.contains_integer());
		EXPECT_TRUE(i2.contains_integer());
	}
	{ // weak / strict
		Interval i1(0, BoundType::WEAK, 1, BoundType::STRICT);
		Interval i3(0, BoundType::WEAK, HALF, BoundType::STRICT);
		Interval i5(HALF, BoundType::WEAK, 2, BoundType::STRICT);
		Interval i6(HALF, BoundType::WEAK, 1, BoundType::STRICT);
		Interval i7(HALF, BoundType::WEAK, 1 + HALF, BoundType::STRICT);
		Interval i8(HALF, BoundType::WEAK, HALF, BoundType::STRICT);
		EXPECT_TRUE(i1.contains_integer());
		EXPECT_TRUE(i3.contains_integer());
		EXPECT_TRUE(i5.contains_integer());
		EXPECT_FALSE(i6.contains_integer());
		EXPECT_TRUE(i7.contains_integer());
		EXPECT_FALSE(i8.contains_integer());
	}
	{ // weak / weak
		Interval i1(0, BoundType::WEAK, 1, BoundType::WEAK);
		Interval i3(0, BoundType::WEAK, HALF, BoundType::WEAK);
		Interval i5(HALF, BoundType::WEAK, 1, BoundType::WEAK);
		Interval i7(HALF, BoundType::WEAK, 1 + HALF, BoundType::WEAK);
		Interval i8(HALF, BoundType::WEAK, HALF, BoundType::WEAK);
		EXPECT_TRUE(i1.contains_integer());
		EXPECT_TRUE(i3.contains_integer());
		EXPECT_TRUE(i5.contains_integer());
		EXPECT_TRUE(i7.contains_integer());
		EXPECT_FALSE(i8.contains_integer());
	}
	{ // weak / infty
		Interval i1(0, BoundType::WEAK, 0, BoundType::INFTY);
		Interval i2(HALF, BoundType::WEAK, 0, BoundType::INFTY);
		EXPECT_TRUE(i1.contains_integer());
		EXPECT_TRUE(i2.contains_integer());
	}
	{ // infty / infty
		Interval i1(0, BoundType::INFTY, 0, BoundType::INFTY);
		EXPECT_TRUE(i1.contains_integer());
	}
}

TYPED_TEST(IntervalRationalTest, center) {
	using Interval = Interval<TypeParam>;
	{ // unbounded
		Interval i = Interval::unbounded_interval();
		EXPECT_EQ(0, center(i));
	}
	{ // bounded / unbounded
		Interval i1(-5, BoundType::WEAK, 0, BoundType::INFTY);
		EXPECT_TRUE(center(i1) >= -5);
		Interval i2(-5, BoundType::STRICT, 0, BoundType::INFTY);
		EXPECT_TRUE(center(i2) > -5);
		Interval i3(5, BoundType::WEAK, 0, BoundType::INFTY);
		EXPECT_TRUE(center(i3) >= 5);
		Interval i4(5, BoundType::STRICT, 0, BoundType::INFTY);
		EXPECT_TRUE(center(i4) > 5);
	}
	{ // unbounded / bounded
		Interval i1(0, BoundType::INFTY, -5, BoundType::WEAK);
		EXPECT_TRUE(center(i1) <= -5);
		Interval i2(0, BoundType::INFTY, -5, BoundType::STRICT);
		EXPECT_TRUE(center(i2) < -5);
		Interval i3(0, BoundType::INFTY, 5, BoundType::WEAK);
		EXPECT_TRUE(center(i3) <= 5);
		Interval i4(0, BoundType::INFTY, 5, BoundType::STRICT);
		EXPECT_TRUE(center(i4) < 5);
	}
	{ // weak / weak
		Interval i1(-10, BoundType::WEAK, -5, BoundType::WEAK);
		EXPECT_TRUE(-10 <= center(i1) && center(i1) <= -5);
		Interval i2(-5, BoundType::WEAK, 0, BoundType::WEAK);
		EXPECT_TRUE(-5 <= center(i2) && center(i2) <= 0);
		Interval i3(-5, BoundType::WEAK, 5, BoundType::WEAK);
		EXPECT_TRUE(-5 <= center(i3) && center(i3) <= 5);
		Interval i4(0, BoundType::WEAK, 5, BoundType::WEAK);
		EXPECT_TRUE(0 <= center(i4) && center(i4) <= 5);
		Interval i5(5, BoundType::WEAK, 10, BoundType::WEAK);
		EXPECT_TRUE(5 <= center(i5) && center(i5) <= 10);
	}
	{ // weak / strict
		Interval i1(-10, BoundType::WEAK, -5, BoundType::STRICT);
		EXPECT_TRUE(-10 <= center(i1) && center(i1) < -5);
		Interval i2(-5, BoundType::WEAK, 0, BoundType::STRICT);
		EXPECT_TRUE(-5 <= center(i2) && center(i2) < 0);
		Interval i3(-5, BoundType::WEAK, 5, BoundType::STRICT);
		EXPECT_TRUE(-5 <= center(i3) && center(i3) < 5);
		Interval i4(0, BoundType::WEAK, 5, BoundType::STRICT);
		EXPECT_TRUE(0 <= center(i4) && center(i4) < 5);
		Interval i5(5, BoundType::WEAK, 10, BoundType::STRICT);
		EXPECT_TRUE(5 <= center(i5) && center(i5) < 10);
	}
	{ // strict / weak
		Interval i1(-10, BoundType::STRICT, -5, BoundType::WEAK);
		EXPECT_TRUE(-10 < center(i1) && center(i1) <= -5);
		Interval i2(-5, BoundType::STRICT, 0, BoundType::WEAK);
		EXPECT_TRUE(-5 < center(i2) && center(i2) <= 0);
		Interval i3(-5, BoundType::STRICT, 5, BoundType::WEAK);
		EXPECT_TRUE(-5 < center(i3) && center(i3) <= 5);
		Interval i4(0, BoundType::STRICT, 5, BoundType::WEAK);
		EXPECT_TRUE(0 < center(i4) && center(i4) <= 5);
		Interval i5(5, BoundType::STRICT, 10, BoundType::WEAK);
		EXPECT_TRUE(5 < center(i5) && center(i5) <= 10);
	}
	{ // strict / strict
		Interval i1(-10, BoundType::STRICT, -5, BoundType::STRICT);
		EXPECT_TRUE(-10 < center(i1) && center(i1) < -5);
		Interval i2(-5, BoundType::STRICT, 0, BoundType::STRICT);
		EXPECT_TRUE(-5 < center(i2) && center(i2) < 0);
		Interval i3(-5, BoundType::STRICT, 5, BoundType::STRICT);
		EXPECT_TRUE(-5 < center(i3) && center(i3) < 5);
		Interval i4(0, BoundType::STRICT, 5, BoundType::STRICT);
		EXPECT_TRUE(0 < center(i4) && center(i4) < 5);
		Interval i5(5, BoundType::STRICT, 10, BoundType::STRICT);
		EXPECT_TRUE(5 < center(i5) && center(i5) < 10);
	}
}

TYPED_TEST(IntervalRationalTest, abs) {
	using Interval = Interval<TypeParam>;
	Interval i(-ONE, -HALF);
	EXPECT_EQ(i.abs().lower(), HALF);
	EXPECT_EQ(i.abs().upper(), ONE);

	Interval i2(1,2);
	EXPECT_EQ(i2, i2.abs());

	Interval i3(-1,1);
	EXPECT_EQ(Interval(0,1), i3.abs());

	Interval i4(-ONE, BoundType::STRICT, -HALF, BoundType::WEAK);
	EXPECT_EQ(i4.abs().lower(), HALF);
	EXPECT_EQ(i4.abs().upper(), ONE);
	EXPECT_EQ(i4.abs().lower_bound_type(), BoundType::WEAK);
	EXPECT_EQ(i4.abs().upper_bound_type(), BoundType::STRICT);

	Interval i5(-ONE, BoundType::STRICT, ONE, BoundType::STRICT);
	EXPECT_EQ(i5.abs().lower(), 0);
	EXPECT_EQ(i5.abs().upper(), ONE);
	EXPECT_EQ(i5.abs().lower_bound_type(), BoundType::WEAK);
	EXPECT_EQ(i5.abs().upper_bound_type(), BoundType::STRICT);
}

TYPED_TEST(IntervalRationalTest, mul_assign) {
	Interval<TypeParam> i(TypeParam(-1));
	Interval<TypeParam> j(TypeParam(0), BoundType::INFTY, TypeParam(0), BoundType::STRICT);
	Interval<TypeParam> res(TypeParam(0), BoundType::STRICT, TypeParam(0), BoundType::INFTY);
	i *= j;
	EXPECT_EQ(i, res);

	Interval<TypeParam> k{1, BoundType::STRICT, 2, BoundType::STRICT};
	Interval<TypeParam> res_k = TypeParam(0) * i;
	Interval<TypeParam> l{1, BoundType::WEAK, 2, BoundType::STRICT};
	Interval<TypeParam> res_l = TypeParam(0) * i;
	Interval<TypeParam> m{1, BoundType::STRICT, 2, BoundType::WEAK};
	Interval<TypeParam> res_m = TypeParam(0) * i;
	Interval<TypeParam> n{1, BoundType::WEAK, 2, BoundType::WEAK};
	Interval<TypeParam> res_n = TypeParam(0) * i;
	Interval<TypeParam> o{0, BoundType::INFTY, 2, BoundType::STRICT};
	Interval<TypeParam> res_o = TypeParam(0) * i;
	Interval<TypeParam> p{1, BoundType::STRICT, 1, BoundType::INFTY};
	Interval<TypeParam> res_p = TypeParam(0) * i;
	Interval<TypeParam> q{0, BoundType::INFTY, 0, BoundType::INFTY};
	Interval<TypeParam> res_q = TypeParam(0) * i;

	Interval<TypeParam> zero{0};
	EXPECT_EQ(res_k, zero);
	EXPECT_EQ(res_l, zero);
	EXPECT_EQ(res_m, zero);
	EXPECT_EQ(res_n, zero);
	EXPECT_EQ(res_o, zero);
	EXPECT_EQ(res_p, zero);
	EXPECT_EQ(res_q, zero);
}

TYPED_TEST(IntervalRationalTest, set_is_subset) {
	
	Interval<TypeParam> e = Interval<TypeParam>::empty_interval();
	Interval<TypeParam> i1(TypeParam(-1), TypeParam(1));
	Interval<TypeParam> i2(TypeParam(2), TypeParam(3));
	EXPECT_FALSE(carl::set_is_subset(i1, e));
	EXPECT_FALSE(carl::set_is_subset(i2, e));
}
