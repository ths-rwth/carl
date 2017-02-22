#include "../Common.h"

#include <carl/numbers/numbers.h>
#include <gtest/gtest.h>

template<typename T>
class Integers: public testing::Test {};

TYPED_TEST_CASE(Integers, IntegerTypes);

TYPED_TEST(Integers, parse) {
	EXPECT_EQ(TypeParam(123), carl::parse<TypeParam>("123"));
	EXPECT_EQ(TypeParam(123), carl::parse<TypeParam>("123.0"));
	EXPECT_EQ(TypeParam(123), carl::parse<TypeParam>("123e0"));
	EXPECT_EQ(TypeParam(123), carl::parse<TypeParam>("123E0"));
	EXPECT_EQ(TypeParam(12300), carl::parse<TypeParam>("123e2"));
	EXPECT_EQ(TypeParam(12300), carl::parse<TypeParam>("123E2"));
}

TYPED_TEST(Integers, try_parse) {
	TypeParam res;
	EXPECT_FALSE(carl::try_parse<TypeParam>("12.3", res));
	EXPECT_FALSE(carl::try_parse<TypeParam>("12E-1", res));
}

template<typename T>
class Rationals: public testing::Test {};

TYPED_TEST_CASE(Rationals, RationalTypes);

TYPED_TEST(Rationals, parse) {
	// Integers
	EXPECT_EQ(TypeParam(0), carl::parse<TypeParam>("0"));
	EXPECT_EQ(TypeParam(123), carl::parse<TypeParam>("123"));
    EXPECT_EQ(TypeParam(1234567890), carl::parse<TypeParam>("1234567890"));

	// Easy fractions
	EXPECT_EQ(TypeParam(1)/TypeParam(10), carl::parse<TypeParam>("0.1"));
    EXPECT_EQ(TypeParam(1)/TypeParam(10), carl::parse<TypeParam>(".1"));
    EXPECT_EQ(TypeParam(3)/TypeParam(2), carl::parse<TypeParam>("1.5"));
	
	// Scientific notation
	EXPECT_EQ(TypeParam(1000), carl::parse<TypeParam>("1e3"));
	EXPECT_EQ(TypeParam(1)/TypeParam(1000), carl::parse<TypeParam>("1e-3"));
	EXPECT_EQ(TypeParam(1000)/TypeParam(25), carl::parse<TypeParam>("1e3/2.5E1"));
}

TYPED_TEST(Rationals, try_parse) {
	TypeParam res;
	EXPECT_FALSE(carl::try_parse<TypeParam>("1/2/3", res));
}
