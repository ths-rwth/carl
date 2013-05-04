#include "gtest/gtest.h"
#include "arithmetic/core/Variable.h"
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
    Variable v0 = Variable((unsigned)0);
    Variable v1 = Variable((unsigned)1);
    Variable v2 = Variable((unsigned)2);
    
    Monomial<int> m0(v0);
    m0 *= v1;
    EXPECT_EQ(1,m0.exponentOfVariable(v1));
    m0 *= v1;
    EXPECT_EQ(2,m0.exponentOfVariable(v1));
    EXPECT_EQ(3,m0.tdeg());
    EXPECT_EQ(0,m0.exponentOfVariable(v2));
    m0 *= v2;
    EXPECT_EQ(4,m0.tdeg());
    EXPECT_EQ(3,m0.nrVariables());
    Monomial<int> m1(m0);
    m1 *= 0;
    EXPECT_EQ(0,m1.tdeg());
    EXPECT_EQ(0,m1.exponentOfVariable(v1));
    EXPECT_EQ(0,m1.nrVariables());
}

