#include "gtest/gtest.h"
#include "arithmetic/core/MultivariatePolynomial.h"


using namespace arithmetic;

TEST(MultivariatePolynomial, Constructor)
{
    Variable v0(0);
    Term<int> t0(v0);
    MultivariatePolynomial<int> p0(t0);
}

TEST(MultivariatePolynomial, Operators)
{
    Variable v0(0);
    Term<int> t0(v0);
    MultivariatePolynomial<int> p0a(t0);
    MultivariatePolynomial<int> p0b(v0);
    EXPECT_EQ(p0a, p0b);
    
}

TEST(MultivariatePolynomial, Addition)
{
    Variable v0(0);
    Term<int> t0(v0);
    MultivariatePolynomial<int> p0(v0);
    p0 += 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 += 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 += -6;
    EXPECT_EQ(1, p0.nrTerms());
    
    Variable v1(1);
    Variable v2(2);
    p0 += v1;
    p0 += Monomial(v2);
    EXPECT_EQ(3,p0.nrTerms());
    p0 += Monomial(v2);
    EXPECT_EQ(3,p0.nrTerms());
    p0 += Term<int>(-2,v2);
    EXPECT_EQ(2,p0.nrTerms());
    
    MultivariatePolynomial<int> p1(v0);
    p1 += v1;
    p0 += p1;
    EXPECT_EQ(2,p0.nrTerms());   
}

TEST(MultivariatePolynomial, Substraction)
{
    Variable v0(0);
    MultivariatePolynomial<int> p0(v0);
    p0 -= 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 -= 3;
    EXPECT_EQ(2, p0.nrTerms());
    p0 -= -6;
    EXPECT_EQ(1, p0.nrTerms());
    Variable v1(1);
    Variable v2(2);
    p0 -= v1;
    EXPECT_EQ(2,p0.nrTerms());
    p0 -= Monomial(v2);
    EXPECT_EQ(3,p0.nrTerms());
    p0 -= Monomial(v2);
    
    EXPECT_EQ(3,p0.nrTerms());
    p0 -= Term<int>(-2,v2);
    EXPECT_EQ(2,p0.nrTerms());
    
    MultivariatePolynomial<int> p1(v0);
    p1 -= v1;
    p0 -= p1;
    EXPECT_EQ(0,p0.nrTerms());
}

TEST(MultivariatePolynomial, Multiplication)
{
    
}


