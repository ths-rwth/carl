#include "gtest/gtest.h"
#include "arithmetic/core/Monomial.h"

using namespace arithmetic;

TEST(Monomial, Constructor)
{
    Monomial<int> m1(1);
    Monomial<unsigned> m2(3);
    SUCCEED();
}

TEST(Monomial, Operators)
{
    
}

