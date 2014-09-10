#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"
#include "config.h"

using namespace carl;

template<typename T>
class IntegerProperties: public testing::Test {};

TYPED_TEST_CASE(IntegerProperties, IntegerTypes);

TYPED_TEST(IntegerProperties, IntegerProperties) {
	for (int i = -10; i < 11; i++) {
		EXPECT_TRUE(carl::isInteger(TypeParam(i)));
	}
}

template<typename T>
class RationalProperties: public testing::Test {};

TYPED_TEST_CASE(RationalProperties, RationalTypes);

TYPED_TEST(RationalProperties, isInteger) {
	for (int i = -10; i < 11; i++) {
		EXPECT_TRUE(carl::isInteger(TypeParam(i)));
		EXPECT_EQ(i % 2 == 0, carl::isInteger(TypeParam(i) / TypeParam(2)));
	}
}
