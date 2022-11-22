#include "../Common.h"

#include <carl-arith/numbers/PrimeFactory.h>
#include <carl-arith/numbers/numbers.h>

template<typename T>
class PrimeFactory: public testing::Test {};

TYPED_TEST_SUITE(PrimeFactory, IntegerTypes);

TYPED_TEST(PrimeFactory, integers)
{
    carl::PrimeFactory<TypeParam> primefact;
    EXPECT_EQ(TypeParam(2), primefact.next_prime());
    EXPECT_EQ(TypeParam(3), primefact.next_prime());
    EXPECT_EQ(TypeParam(5), primefact.next_prime());
	EXPECT_EQ(TypeParam(7), primefact.next_prime());
	EXPECT_EQ(TypeParam(11), primefact.next_prime());
	EXPECT_EQ(TypeParam(13), primefact.next_prime());
	EXPECT_EQ(TypeParam(17), primefact.next_prime());
}
