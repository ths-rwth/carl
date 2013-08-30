#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include <gmpxx.h>


using namespace carl;

TEST(MultivariatePolynomial, Constructor)
{
    Variable v0(0);
    Term<int> t0(v0);
    MultivariatePolynomial<int> p0(t0);
    
    MultivariatePolynomial<int> p1(3);
    EXPECT_EQ(1, p1.nrTerms());
    EXPECT_TRUE(p1.isLinear());
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
    
    MultivariatePolynomial<int> p10a;
    p10a += v0*v0;
    p10a += v1;
    EXPECT_EQ(v0*v0, *p10a.lterm());
    MultivariatePolynomial<int> p10b;
    p10b += v1;
    p10b += v0*v0;
    EXPECT_EQ(v0*v0, *p10b.lterm());
    
    MultivariatePolynomial<cln::cl_RA> mp2(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(v0, *mp2.lterm());
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
    Variable v0(0);
    Variable v1(1);
    MultivariatePolynomial<int> p0(v0);
    
    p0 *= v0;
    EXPECT_EQ(Term<int>(1,v0,2), *(p0.lterm()));
    
    MultivariatePolynomial<int> q({2*v0*v0, 1*v0*v1});
    Term<int> t(1, v0,2);
    MultivariatePolynomial<int> res = q * t;
    EXPECT_EQ(MultivariatePolynomial<int>({2*v0*v0*v0*v0, 1*v0*v0*v0*v1}), res);
    
    p0 += v1;
    p0 += 1;
    //std::cout << p0 << std::endl;
    p0 *= p0;
    //std::cout << p0 << std::endl;
    
}

TEST(MultivariatePolynomial, Normalize)
{
    Variable v0(0);
    Variable v1(1);
    Variable v2(2);
    MultivariatePolynomial<cln::cl_RA> mp;
    mp += v0;
    MultivariatePolynomial<cln::cl_RA> mp2 = mp * (cln::cl_RA)2;
    EXPECT_EQ(mp, mp.normalize());
    EXPECT_EQ(mp, mp2.normalize());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)2 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += v1;
    EXPECT_EQ(mp2, mp.normalize());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(mp2, mp.normalize());
}

TEST(MultivariatePolynomial, Coprime)
{
    Variable v0(0);
    Variable v1(1);
    MultivariatePolynomial<cln::cl_RA> mp;
    mp += v0;
    MultivariatePolynomial<cln::cl_RA> mp2 = mp * (cln::cl_RA)2;
    EXPECT_EQ(mp, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>(v0);
    mp2 += (cln::cl_RA)2 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1 * v0);
    mp2 += (cln::cl_RA)24 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    mp = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)1/4 * v1;
    mp2 = MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)2 * v0);
    mp2 += (cln::cl_RA)3 * v1;
    EXPECT_EQ(mp2, mp.coprimeCoefficients());
    
    MultivariatePolynomial<mpq_class> mpG = MultivariatePolynomial<mpq_class>(mpq_class(1,6) * v0);
    mpG += mpq_class(1,4) * v1;
    MultivariatePolynomial<mpq_class> mp2G = MultivariatePolynomial<mpq_class>(mpq_class(2) * v0);
    mp2G += mpq_class(3) * v1;
    EXPECT_EQ(mp2G, mpG.coprimeCoefficients());
}
        
TEST(MultivariatePolynomial, Substitute)
{
    Variable v0(0);
    Variable v1(1);
    MultivariatePolynomial<cln::cl_RA> mp((cln::cl_RA)1/6 * v0);
    mp += (cln::cl_RA)4 * v1;
    MultivariatePolynomial<cln::cl_RA> mp2((cln::cl_RA)2);
    mp2 += (cln::cl_RA)4 * v1;
    std::map<Variable, cln::cl_RA> substitutions;
    substitutions[v0] = (cln::cl_RA)12;
    EXPECT_EQ(mp2, mp.substitute(substitutions));
    substitutions[v0] = (cln::cl_RA)0;
    EXPECT_EQ(MultivariatePolynomial<cln::cl_RA>((cln::cl_RA)4 * v1), mp.substitute(substitutions));
}

TEST(MultivariatePolynomial, SPolynomial)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    MultivariatePolynomial<cln::cl_RA> f1({(cln::cl_RA)1*x*x*x*y*y, (cln::cl_RA)-1*x*x*y*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> g1({(cln::cl_RA)3*x*x*x*x*y, (cln::cl_RA)1*y*y});
    EXPECT_EQ(3,MultivariatePolynomial<cln::cl_RA>::SPolynomial(f1.normalize(), g1.normalize()).nrTerms());
    //MultivariatePolynomial<cln::cl_RA> s1({(cln::cl_RA)-1*x*x});
    //EXPECT_EQ(s1, MultivariatePolynomial::SPolynomial(f1.normalize(), g1.normalize()));
    
    
    MultivariatePolynomial<cln::cl_RA> f2({(cln::cl_RA)1*x*x*x, (cln::cl_RA)-2*x*y} );
    MultivariatePolynomial<cln::cl_RA> g2({(cln::cl_RA)1*x*x*y, (cln::cl_RA)-2*y*y, (cln::cl_RA)1*x});
    MultivariatePolynomial<cln::cl_RA> s2({(cln::cl_RA)-1*x*x});
    EXPECT_EQ(s2, MultivariatePolynomial<cln::cl_RA>::SPolynomial(f2, g2));
    
    
    
}

TEST(MultivariatePolynomial, GatherVariables)
{
    EXPECT_TRUE(false);
}

TEST(MultivariatePolynomial, Derive)
{
    EXPECT_TRUE(false);
}