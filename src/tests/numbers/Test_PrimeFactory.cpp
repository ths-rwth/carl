#include "gtest/gtest.h"
#include "carl/numbers/PrimeFactory.h"
using namespace carl;

TEST(PrimeFactory, integers)
{
    PrimeFactory<unsigned> primefact;
    EXPECT_EQ(2,primefact.nextPrime());
    EXPECT_EQ(3,primefact.nextPrime());
    EXPECT_EQ(5,primefact.nextPrime());
}


