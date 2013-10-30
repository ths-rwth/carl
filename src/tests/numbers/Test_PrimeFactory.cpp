#include "gtest/gtest.h"
#include "carl/numbers/PrimeFactory.h"
using namespace carl;

TEST(PrimeFactory, integers)
{
    PrimeFactory<unsigned> primefact;
    EXPECT_EQ((unsigned)2,primefact.nextPrime());
    EXPECT_EQ((unsigned)3,primefact.nextPrime());
    EXPECT_EQ((unsigned)5,primefact.nextPrime());
}


