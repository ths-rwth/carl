#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"

using namespace carl;

TEST(NumbersNative, pow)
{
	ASSERT_EQ(1, carl::pow(0, 0));
	ASSERT_EQ(0, carl::pow(0, 1));
	ASSERT_EQ(0, carl::pow(0, 2));
	ASSERT_EQ(1, carl::pow(1, 0));
	ASSERT_EQ(1, carl::pow(1, 1));
	ASSERT_EQ(1, carl::pow(1, 2));
	ASSERT_EQ(1, carl::pow(2, 0));
	ASSERT_EQ(2, carl::pow(2, 1));
	ASSERT_EQ(4, carl::pow(2, 2));
	ASSERT_EQ(27, carl::pow(3, 3));
	ASSERT_EQ(81, carl::pow(3, 4));
	ASSERT_EQ(243, carl::pow(3, 5));
}
	
TEST(NumbersNative, highestPower)
{
    ASSERT_EQ(highestPower(64), 64);
	ASSERT_EQ(highestPower(65), 64);
	ASSERT_EQ(highestPower(66), 64);
	ASSERT_EQ(highestPower(0), 0);
}