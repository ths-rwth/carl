#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"

#include <cstdint>

using namespace carl;

TEST(NumbersNative, NumberWidth)
{
	static_assert(sizeof(short int) == 2,"");
	static_assert(sizeof(unsigned short int) == 2,"");
	static_assert(sizeof(int) == 4,"");
	static_assert(sizeof(unsigned int) == 4,"");
	static_assert(sizeof(long int) == 8,"");
	static_assert(sizeof(unsigned long int) == 8,"");
	static_assert(sizeof(long long int) == 8,"");
	static_assert(sizeof(unsigned long long int) == 8,"");
}

TEST(NumbersNative, highestPower)
{
    ASSERT_EQ(highestPower(64), 64);
	ASSERT_EQ(highestPower(65), 64);
	ASSERT_EQ(highestPower(66), 64);
	ASSERT_EQ(highestPower(0), 0);
}
