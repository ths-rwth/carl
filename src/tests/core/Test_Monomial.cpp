#include "gtest/gtest.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"

using namespace carl;

TEST(Monomial, Constructor)
{
    Monomial m1();
    Monomial m2();
    SUCCEED();
}

TEST(Monomial, Operators)
{
    Variable v0 = Variable((unsigned)0);
    Variable v1 = Variable((unsigned)1);
    Variable v2 = Variable((unsigned)2);
    
    Monomial m0(v0);
    m0 *= v1;
    EXPECT_EQ(1,m0.exponentOfVariable(v1));
    m0 *= v1;
    EXPECT_EQ(2,m0.exponentOfVariable(v1));
    EXPECT_EQ(3,m0.tdeg());
    EXPECT_EQ(0,m0.exponentOfVariable(v2));
    m0 *= v2;
    EXPECT_EQ(4,m0.tdeg());
    EXPECT_EQ(3,m0.nrVariables());
    Monomial m1;
    EXPECT_EQ(0,m1.tdeg());
    EXPECT_EQ(0,m1.exponentOfVariable(v1));
    EXPECT_EQ(0,m1.nrVariables());
    
    Monomial m2;
    Monomial m3;
    m2 *= v1;
    m3 *= v1;
    EXPECT_EQ(m2, m3);
}

TEST(Monomial, multiplication)
{
    Variable v0 = Variable((unsigned)0);
    Variable v1 = Variable((unsigned)1);
    Variable v2 = Variable((unsigned)2);
    
    Monomial m0(v0);
    Monomial m1(v1);
    Monomial m01(v0);
    m01 *= v1;
    m0 *= m1;
    EXPECT_EQ(m01, m0);
    m01 *= m01;
    m0 *= v0;
    m0 *= v1;
    //m0 *= m0;
    EXPECT_EQ(m01, m0);
    
}

