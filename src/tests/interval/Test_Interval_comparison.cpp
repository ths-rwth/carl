#include <gtest/gtest.h>

#include <carl/interval/Interval.h>

#include "../number_types.h"

using namespace carl;

template<typename T>
class IntervalOperatorTest: public testing::Test {};

#define ZERO TypeParam(0)
#define ONE TypeParam(1)

TYPED_TEST_CASE(IntervalOperatorTest, RationalTypes);

/* Cases:
 * - WEAK, STRICT, INFTY x WEAK, STRICT, INFTY
 * - Number is <, =, >
 */

TYPED_TEST(IntervalOperatorTest, LBLB_lt) {
	using LB = LowerBound<TypeParam>;

	LB lbW0 { ZERO, BoundType::WEAK };
	LB lbW1 { ONE, BoundType::WEAK };
	LB lbS0 { ZERO, BoundType::STRICT };
	LB lbS1 { ONE, BoundType::STRICT };
	LB lbI0 { ZERO, BoundType::INFTY };
	LB lbI1 { ONE, BoundType::INFTY };
	EXPECT_TRUE(lbW0 < lbW1);
	EXPECT_FALSE(lbW0 < lbW0);
	EXPECT_FALSE(lbW1 < lbW0);
	EXPECT_TRUE(lbW0 < lbS1);
	EXPECT_TRUE(lbW0 < lbS0);
	EXPECT_FALSE(lbW1 < lbS0);
	EXPECT_FALSE(lbW0 < lbI0);
	EXPECT_FALSE(lbW1 < lbI0);
	EXPECT_FALSE(lbW0 < lbI1);
	EXPECT_FALSE(lbS0 < lbW0);
	EXPECT_FALSE(lbS1 < lbW0);
	EXPECT_TRUE(lbS0 < lbW1);
	EXPECT_FALSE(lbS0 < lbS0);
	EXPECT_FALSE(lbS1 < lbS0);
	EXPECT_TRUE(lbS0 < lbS1);
	EXPECT_FALSE(lbS0 < lbI0);
	EXPECT_FALSE(lbS1 < lbI0);
	EXPECT_FALSE(lbS0 < lbI1);
	EXPECT_TRUE(lbI0 < lbW0);
	EXPECT_TRUE(lbI1 < lbW0);
	EXPECT_TRUE(lbI0 < lbW1);
	EXPECT_TRUE(lbI0 < lbS0);
	EXPECT_TRUE(lbI1 < lbS0);
	EXPECT_TRUE(lbI0 < lbS1);
	EXPECT_FALSE(lbI0 < lbI1);
	EXPECT_FALSE(lbI0 < lbI0);
	EXPECT_FALSE(lbI1 < lbI0);
}

TYPED_TEST(IntervalOperatorTest, LBLB_leq) {
	using LB = LowerBound<TypeParam>;

	LB lbW0 { ZERO, BoundType::WEAK };
	LB lbW1 { ONE, BoundType::WEAK };
	LB lbS0 { ZERO, BoundType::STRICT };
	LB lbS1 { ONE, BoundType::STRICT };
	LB lbI0 { ZERO, BoundType::INFTY };
	LB lbI1 { ONE, BoundType::INFTY };
	EXPECT_TRUE(lbW0 <= lbW1);
	EXPECT_TRUE(lbW0 <= lbW0);
	EXPECT_FALSE(lbW1 <= lbW0);
	EXPECT_TRUE(lbW0 <= lbS1);
	EXPECT_TRUE(lbW0 <= lbS0);
	EXPECT_FALSE(lbW1 <= lbS0);
	EXPECT_FALSE(lbW0 <= lbI0);
	EXPECT_FALSE(lbW1 <= lbI0);
	EXPECT_FALSE(lbW0 <= lbI1);
	EXPECT_FALSE(lbS0 <= lbW0);
	EXPECT_FALSE(lbS1 <= lbW0);
	EXPECT_TRUE(lbS0 <= lbW1);
	EXPECT_TRUE(lbS0 <= lbS0);
	EXPECT_FALSE(lbS1 <= lbS0);
	EXPECT_TRUE(lbS0 <= lbS1);
	EXPECT_FALSE(lbS0 <= lbI0);
	EXPECT_FALSE(lbS1 <= lbI0);
	EXPECT_FALSE(lbS0 <= lbI1);
	EXPECT_TRUE(lbI0 <= lbW0);
	EXPECT_TRUE(lbI1 <= lbW0);
	EXPECT_TRUE(lbI0 <= lbW1);
	EXPECT_TRUE(lbI0 <= lbS0);
	EXPECT_TRUE(lbI1 <= lbS0);
	EXPECT_TRUE(lbI0 <= lbS1);
	EXPECT_TRUE(lbI0 <= lbI1);
	EXPECT_TRUE(lbI0 <= lbI0);
	EXPECT_TRUE(lbI1 <= lbI0);
}

TYPED_TEST(IntervalOperatorTest, UBLB_lt) {
	using LB = LowerBound<TypeParam>;
	using UB = UpperBound<TypeParam>;

	LB lbW0 { ZERO, BoundType::WEAK };
	LB lbW1 { ONE, BoundType::WEAK };
	LB lbS0 { ZERO, BoundType::STRICT };
	LB lbS1 { ONE, BoundType::STRICT };
	LB lbI0 { ZERO, BoundType::INFTY };
	LB lbI1 { ONE, BoundType::INFTY };
	UB ubW0 { ZERO, BoundType::WEAK };
	UB ubW1 { ONE, BoundType::WEAK };
	UB ubS0 { ZERO, BoundType::STRICT };
	UB ubS1 { ONE, BoundType::STRICT };
	UB ubI0 { ZERO, BoundType::INFTY };
	UB ubI1 { ONE, BoundType::INFTY };
	EXPECT_TRUE(ubW0 < lbW1);
	EXPECT_FALSE(ubW0 < lbW0);
	EXPECT_FALSE(ubW1 < lbW0);
	EXPECT_TRUE(ubW0 < lbS1);
	EXPECT_TRUE(ubW0 < lbS0);
	EXPECT_FALSE(ubW1 < lbS0);
	EXPECT_FALSE(ubW0 < lbI0);
	EXPECT_FALSE(ubW1 < lbI0);
	EXPECT_FALSE(ubW0 < lbI1);
	EXPECT_TRUE(ubS0 < lbW0);
	EXPECT_FALSE(ubS1 < lbW0);
	EXPECT_TRUE(ubS0 < lbW1);
	EXPECT_TRUE(ubS0 < lbS0);
	EXPECT_FALSE(ubS1 < lbS0);
	EXPECT_TRUE(ubS0 < lbS1);
	EXPECT_FALSE(ubS0 < lbI0);
	EXPECT_FALSE(ubS1 < lbI0);
	EXPECT_FALSE(ubS0 < lbI1);
	EXPECT_FALSE(ubI0 < lbW0);
	EXPECT_FALSE(ubI1 < lbW0);
	EXPECT_FALSE(ubI0 < lbW1);
	EXPECT_FALSE(ubI0 < lbS0);
	EXPECT_FALSE(ubI1 < lbS0);
	EXPECT_FALSE(ubI0 < lbS1);
	EXPECT_FALSE(ubI0 < lbI1);
	EXPECT_FALSE(ubI0 < lbI0);
	EXPECT_FALSE(ubI1 < lbI0);
}

TYPED_TEST(IntervalOperatorTest, LBUB_leq) {
	using LB = LowerBound<TypeParam>;
	using UB = UpperBound<TypeParam>;

	LB lbW0 { ZERO, BoundType::WEAK };
	LB lbW1 { ONE, BoundType::WEAK };
	LB lbS0 { ZERO, BoundType::STRICT };
	LB lbS1 { ONE, BoundType::STRICT };
	LB lbI0 { ZERO, BoundType::INFTY };
	LB lbI1 { ONE, BoundType::INFTY };
	UB ubW0 { ZERO, BoundType::WEAK };
	UB ubW1 { ONE, BoundType::WEAK };
	UB ubS0 { ZERO, BoundType::STRICT };
	UB ubS1 { ONE, BoundType::STRICT };
	UB ubI0 { ZERO, BoundType::INFTY };
	UB ubI1 { ONE, BoundType::INFTY };
	EXPECT_TRUE(lbW0 <= ubW1);
	EXPECT_TRUE(lbW0 <= ubW0);
	EXPECT_FALSE(lbW1 <= ubW0);
	EXPECT_TRUE(lbW0 <= ubS1);
	EXPECT_FALSE(lbW0 <= ubS0);
	EXPECT_FALSE(lbW1 <= ubS0);
	EXPECT_TRUE(lbW0 <= ubI0);
	EXPECT_TRUE(lbW1 <= ubI0);
	EXPECT_TRUE(lbW0 <= ubI1);
	EXPECT_FALSE(lbS0 <= ubW0);
	EXPECT_FALSE(lbS1 <= ubW0);
	EXPECT_TRUE(lbS0 <= ubW1);
	EXPECT_FALSE(lbS0 <= ubS0);
	EXPECT_FALSE(lbS1 <= ubS0);
	EXPECT_TRUE(lbS0 <= ubS1);
	EXPECT_TRUE(lbS0 <= ubI0);
	EXPECT_TRUE(lbS1 <= ubI0);
	EXPECT_TRUE(lbS0 <= ubI1);
	EXPECT_TRUE(lbI0 <= ubW0);
	EXPECT_TRUE(lbI1 <= ubW0);
	EXPECT_TRUE(lbI0 <= ubW1);
	EXPECT_TRUE(lbI0 <= ubS0);
	EXPECT_TRUE(lbI1 <= ubS0);
	EXPECT_TRUE(lbI0 <= ubS1);
	EXPECT_TRUE(lbI0 <= ubI1);
	EXPECT_TRUE(lbI0 <= ubI0);
	EXPECT_TRUE(lbI1 <= ubI0);
}


TYPED_TEST(IntervalOperatorTest, UBUB_lt) {
	using UB = UpperBound<TypeParam>;

	UB ubW0 { ZERO, BoundType::WEAK };
	UB ubW1 { ONE, BoundType::WEAK };
	UB ubS0 { ZERO, BoundType::STRICT };
	UB ubS1 { ONE, BoundType::STRICT };
	UB ubI0 { ZERO, BoundType::INFTY };
	UB ubI1 { ONE, BoundType::INFTY };
	EXPECT_TRUE(ubW0 < ubW1);
	EXPECT_FALSE(ubW0 < ubW0);
	EXPECT_FALSE(ubW1 < ubW0);
	EXPECT_TRUE(ubW0 < ubS1);
	EXPECT_FALSE(ubW0 < ubS0);
	EXPECT_FALSE(ubW1 < ubS0);
	EXPECT_TRUE(ubW0 < ubI0);
	EXPECT_TRUE(ubW1 < ubI0);
	EXPECT_TRUE(ubW0 < ubI1);
	EXPECT_TRUE(ubS0 < ubW0);
	EXPECT_FALSE(ubS1 < ubW0);
	EXPECT_TRUE(ubS0 < ubW1);
	EXPECT_FALSE(ubS0 < ubS0);
	EXPECT_FALSE(ubS1 < ubS0);
	EXPECT_TRUE(ubS0 < ubS1);
	EXPECT_TRUE(ubS0 < ubI0);
	EXPECT_TRUE(ubS1 < ubI0);
	EXPECT_TRUE(ubS0 < ubI1);
	EXPECT_FALSE(ubI0 < ubW0);
	EXPECT_FALSE(ubI1 < ubW0);
	EXPECT_FALSE(ubI0 < ubW1);
	EXPECT_FALSE(ubI0 < ubS0);
	EXPECT_FALSE(ubI1 < ubS0);
	EXPECT_FALSE(ubI0 < ubS1);
	EXPECT_FALSE(ubI0 < ubI1);
	EXPECT_FALSE(ubI0 < ubI0);
	EXPECT_FALSE(ubI1 < ubI0);
}

TYPED_TEST(IntervalOperatorTest, UBUB_leq) {
	using UB = UpperBound<TypeParam>;

	UB ubW0 { ZERO, BoundType::WEAK };
	UB ubW1 { ONE, BoundType::WEAK };
	UB ubS0 { ZERO, BoundType::STRICT };
	UB ubS1 { ONE, BoundType::STRICT };
	UB ubI0 { ZERO, BoundType::INFTY };
	UB ubI1 { ONE, BoundType::INFTY };
	EXPECT_TRUE(ubW0 <= ubW1);
	EXPECT_TRUE(ubW0 <= ubW0);
	EXPECT_FALSE(ubW1 <= ubW0);
	EXPECT_TRUE(ubW0 <= ubS1);
	EXPECT_FALSE(ubW0 <= ubS0);
	EXPECT_FALSE(ubW1 <= ubS0);
	EXPECT_TRUE(ubW0 <= ubI0);
	EXPECT_TRUE(ubW1 <= ubI0);
	EXPECT_TRUE(ubW0 <= ubI1);
	EXPECT_TRUE(ubS0 <= ubW0);
	EXPECT_FALSE(ubS1 <= ubW0);
	EXPECT_TRUE(ubS0 <= ubW1);
	EXPECT_TRUE(ubS0 <= ubS0);
	EXPECT_FALSE(ubS1 <= ubS0);
	EXPECT_TRUE(ubS0 <= ubS1);
	EXPECT_TRUE(ubS0 <= ubI0);
	EXPECT_TRUE(ubS1 <= ubI0);
	EXPECT_TRUE(ubS0 <= ubI1);
	EXPECT_FALSE(ubI0 <= ubW0);
	EXPECT_FALSE(ubI1 <= ubW0);
	EXPECT_FALSE(ubI0 <= ubW1);
	EXPECT_FALSE(ubI0 <= ubS0);
	EXPECT_FALSE(ubI1 <= ubS0);
	EXPECT_FALSE(ubI0 <= ubS1);
	EXPECT_TRUE(ubI0 <= ubI1);
	EXPECT_TRUE(ubI0 <= ubI0);
	EXPECT_TRUE(ubI1 <= ubI0);
}