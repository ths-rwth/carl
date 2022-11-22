#include <carl-arith/numbers/numbers.h>
#include <gtest/gtest.h>

#include "../number_types.h"

using namespace carl;

template<typename T>
class IntegerRounding: public testing::Test {};

TYPED_TEST_SUITE(IntegerRounding, IntegerTypes);

TYPED_TEST(IntegerRounding, IntegerRounding) {
	for (int i = -10; i < 11; i++) {
		EXPECT_EQ(TypeParam(i), carl::floor(TypeParam(i)));
		EXPECT_EQ(TypeParam(i), carl::ceil(TypeParam(i)));
		EXPECT_EQ(TypeParam(std::abs(i)), carl::abs(TypeParam(i)));
	}
}

template<typename T>
class RationalRounding: public testing::Test {};

TYPED_TEST_SUITE(RationalRounding, RationalTypes);

TYPED_TEST(RationalRounding, floor) {
	for (int i = -10; i < 11; i++) {
		EXPECT_EQ(TypeParam(i), carl::floor(TypeParam(i)));
	}
	EXPECT_EQ(TypeParam(-3), carl::floor(TypeParam(-5) / TypeParam(2)));
	EXPECT_EQ(TypeParam(-2), carl::floor(TypeParam(-3) / TypeParam(2)));
	EXPECT_EQ(TypeParam(-1), carl::floor(TypeParam(-1) / TypeParam(2)));
	EXPECT_EQ(TypeParam(0), carl::floor(TypeParam(1) / TypeParam(2)));
	EXPECT_EQ(TypeParam(1), carl::floor(TypeParam(3) / TypeParam(2)));
	EXPECT_EQ(TypeParam(2), carl::floor(TypeParam(5) / TypeParam(2)));
}

TYPED_TEST(RationalRounding, ceil) {
	for (int i = -10; i < 11; i++) {
		EXPECT_EQ(TypeParam(i), carl::ceil(TypeParam(i)));
	}
	EXPECT_EQ(TypeParam(-2), carl::ceil(TypeParam(-5) / TypeParam(2)));
	EXPECT_EQ(TypeParam(-1), carl::ceil(TypeParam(-3) / TypeParam(2)));
	EXPECT_EQ(TypeParam(0), carl::ceil(TypeParam(-1) / TypeParam(2)));
	EXPECT_EQ(TypeParam(1), carl::ceil(TypeParam(1) / TypeParam(2)));
	EXPECT_EQ(TypeParam(2), carl::ceil(TypeParam(3) / TypeParam(2)));
	EXPECT_EQ(TypeParam(3), carl::ceil(TypeParam(5) / TypeParam(2)));
}

TYPED_TEST(RationalRounding, abs) {
	for (int i = -10; i < 11; i++) {
		EXPECT_EQ(TypeParam(std::abs(i)), carl::abs(TypeParam(i)));
	}
	EXPECT_EQ(TypeParam(5) / TypeParam(2), carl::abs(TypeParam(-5) / TypeParam(2)));
	EXPECT_EQ(TypeParam(3) / TypeParam(2), carl::abs(TypeParam(-3) / TypeParam(2)));
	EXPECT_EQ(TypeParam(1) / TypeParam(2), carl::abs(TypeParam(-1) / TypeParam(2)));
	EXPECT_EQ(TypeParam(1) / TypeParam(2), carl::abs(TypeParam(1) / TypeParam(2)));
	EXPECT_EQ(TypeParam(3) / TypeParam(2), carl::abs(TypeParam(3) / TypeParam(2)));
	EXPECT_EQ(TypeParam(5) / TypeParam(2), carl::abs(TypeParam(5) / TypeParam(2)));
}
