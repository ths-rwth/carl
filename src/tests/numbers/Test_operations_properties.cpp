#include "../Common.h"

#include <carl/core/polynomialfunctions/GCD.h>
#include <carl/numbers/numbers.h>
#include <gtest/gtest.h>

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

TYPED_TEST(RationalProperties, NumDenom) {
	typedef typename IntegralType<TypeParam>::type IntType;
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			if (b == 0) continue;
			TypeParam r = TypeParam(a) / TypeParam(b);
			// Fraction should be reduced.
			IntType gcd = carl::gcd(IntType(a), IntType(b));
			// Make sure denominator is always positive.
			if (b < 0) gcd *= IntType(-1);
			EXPECT_EQ(carl::div(a, gcd), carl::getNum(r));
			EXPECT_EQ(carl::div(b, gcd), carl::getDenom(r));
		}
	}
}
