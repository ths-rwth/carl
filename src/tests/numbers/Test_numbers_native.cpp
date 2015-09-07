#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"

using namespace carl;

TEST(NumbersNative, pow)
{
	ASSERT_EQ((unsigned)1, (unsigned)carl::pow(0, 0));
	ASSERT_EQ((unsigned)0, (unsigned)carl::pow(0, 1));
	ASSERT_EQ((unsigned)0, (unsigned)carl::pow(0, 2));
	ASSERT_EQ((unsigned)1, (unsigned)carl::pow(1, 0));
	ASSERT_EQ((unsigned)1, (unsigned)carl::pow(1, 1));
	ASSERT_EQ((unsigned)1, (unsigned)carl::pow(1, 2));
	ASSERT_EQ((unsigned)1, (unsigned)carl::pow(2, 0));
	ASSERT_EQ((unsigned)2, (unsigned)carl::pow(2, 1));
	ASSERT_EQ((unsigned)4, (unsigned)carl::pow(2, 2));
	ASSERT_EQ((unsigned)27, (unsigned)carl::pow(3, 3));
	ASSERT_EQ((unsigned)81, (unsigned)carl::pow(3, 4));
	ASSERT_EQ((unsigned)243, (unsigned)carl::pow(3, 5));
}
	
TEST(NumbersNative, highestPower)
{
    ASSERT_EQ(highestPower(64), 64);
	ASSERT_EQ(highestPower(65), 64);
	ASSERT_EQ(highestPower(66), 64);
	ASSERT_EQ(highestPower(0), 0);
}