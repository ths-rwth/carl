#include "gtest/gtest.h"

#include <carl/numbers/numbers.h>

#include <cstdint>
#include <type_traits>

/**
 * We refer to
 * - http://en.cppreference.com/w/cpp/language/types
 * - http://en.cppreference.com/w/cpp/types/integer
 * We check that we are within ILP32, LLP64 or LP64.
 */
TEST(NumbersNative, NumberWidth)
{
	static_assert(sizeof(short) == 2,"");
	static_assert(sizeof(short int) == 2,"");
	static_assert(sizeof(signed short) == 2,"");
	static_assert(sizeof(signed short int) == 2,"");
	static_assert(sizeof(unsigned short) == 2,"");
	static_assert(sizeof(unsigned short int) == 2,"");
	
	static_assert(sizeof(int) == 4,"");
	static_assert(sizeof(signed) == 4,"");
	static_assert(sizeof(signed int) == 4,"");
	static_assert(sizeof(unsigned) == 4,"");
	static_assert(sizeof(unsigned int) == 4,"");
	
	static_assert(sizeof(long) >= 4,"");
	static_assert(sizeof(long int) >= 4,"");
	static_assert(sizeof(signed long) >= 4,"");
	static_assert(sizeof(signed long int) >= 4,"");
	static_assert(sizeof(unsigned long) >= 4,"");
	static_assert(sizeof(unsigned long int) >= 4,"");
	
	static_assert(sizeof(long long) == 8,"");
	static_assert(sizeof(long long int) == 8,"");
	static_assert(sizeof(signed long long) == 8,"");
	static_assert(sizeof(signed long long int) == 8,"");
	static_assert(sizeof(unsigned long long) == 8,"");
	static_assert(sizeof(unsigned long long int) == 8,"");
	
	static_assert(sizeof(std::int8_t) == 1,"");
	static_assert(sizeof(std::int16_t) == 2,"");
	static_assert(sizeof(std::int32_t) == 4,"");
	static_assert(sizeof(std::int64_t) == 8,"");
	
	static_assert(sizeof(std::int_fast8_t) >= 1,"");
	static_assert(sizeof(std::int_fast16_t) >= 2,"");
	static_assert(sizeof(std::int_fast32_t) >= 4,"");
	static_assert(sizeof(std::int_fast64_t) >= 8,"");
	
	static_assert(sizeof(std::int_least8_t) >= 1,"");
	static_assert(sizeof(std::int_least16_t) >= 2,"");
	static_assert(sizeof(std::int_least32_t) >= 4,"");
	static_assert(sizeof(std::int_least64_t) >= 8,"");
	
	static_assert(sizeof(std::uint8_t) == 1,"");
	static_assert(sizeof(std::uint16_t) == 2,"");
	static_assert(sizeof(std::uint32_t) == 4,"");
	static_assert(sizeof(std::uint64_t) == 8,"");
	
	static_assert(sizeof(std::uint_fast8_t) >= 1,"");
	static_assert(sizeof(std::uint_fast16_t) >= 2,"");
	static_assert(sizeof(std::uint_fast32_t) >= 4,"");
	static_assert(sizeof(std::uint_fast64_t) >= 8,"");
	
	static_assert(sizeof(std::uint_least8_t) >= 1,"");
	static_assert(sizeof(std::uint_least16_t) >= 2,"");
	static_assert(sizeof(std::uint_least32_t) >= 4,"");
	static_assert(sizeof(std::uint_least64_t) >= 8,"");
}

TEST(NumbersNative, TypeIdentity)
{
	static_assert(!std::is_same<long,long long>::value, "");
}

TEST(NumbersNative, highestPower)
{
    ASSERT_EQ(carl::highestPower(64), 64);
	ASSERT_EQ(carl::highestPower(65), 64);
	ASSERT_EQ(carl::highestPower(66), 64);
	ASSERT_EQ(carl::highestPower(0), 0);
}
