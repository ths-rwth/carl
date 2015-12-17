#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"

using namespace carl;

TEST(NumbersNative, highestPower)
{
    ASSERT_EQ(highestPower(64), 64);
	ASSERT_EQ(highestPower(65), 64);
	ASSERT_EQ(highestPower(66), 64);
	ASSERT_EQ(highestPower(0), 0);
}
