#include "gtest/gtest.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Monomial_derivative.h"
#include "Util.cpp"
#include <list>
#include <boost/variant.hpp>

using namespace carl;

TEST(Monomial, Operators)
{
    Variable v0 = Variable((unsigned)1);
    Variable v1 = Variable((unsigned)2);
    Variable v2 = Variable((unsigned)3);
    
    Monomial m0(v0);
    m0 *= v1;
    EXPECT_EQ((unsigned)1,m0.exponentOfVariable(v1));
    m0 *= v1;
    EXPECT_EQ((unsigned)2,m0.exponentOfVariable(v1));
    EXPECT_EQ((unsigned)3,m0.tdeg());
    EXPECT_EQ((unsigned)0,m0.exponentOfVariable(v2));
    m0 *= v2;
    EXPECT_EQ((unsigned)4,m0.tdeg());
    EXPECT_EQ((unsigned)3,m0.nrVariables());
    
    Monomial m2(v1);
    Monomial m3(v1);
    m2 *= v1;
    m3 *= v1;
    EXPECT_EQ(m2, m3);
}

TEST(Monomial, multiplication)
{
    Variable v0((unsigned)1);
    Variable v1((unsigned)2);
    Variable v2((unsigned)3);
    
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
    Monomial m = v0 * v0;
    
}

TEST(Monomial, derivative)
{
    Variable v0((unsigned)1);
    Variable v1((unsigned)2);
    Monomial m0 = v0 * v1;
    Term<int>* t = m0.derivative<int>(v0);
    EXPECT_EQ((unsigned)1, t->getNrVariables());
    
}

TEST(Monomial, division)
{
    Variable v0((unsigned)1);
    Variable v1((unsigned)2);
    Variable v2((unsigned)3);
    
    Monomial m0 = v0 * v0 * v1 * v1 * v2;
    Monomial m1 = v0 * v0 * v0;
    Monomial m2 = v0 * v0 * v1 * v2;
    Monomial m0x = v0 * v0 * v1 * v2;
    Monomial m0y = v0 * v0 * v1 * v1;
    EXPECT_EQ(nullptr, m0.divide(m1));
    EXPECT_EQ(nullptr, m1.divide(m0));
    EXPECT_EQ(m0x, *m0.divide(v1));
    EXPECT_EQ(m0y, *m0.divide(v2));
    EXPECT_EQ(Monomial(v1), *m0.divide(m2));
}

TEST(Monomial, Comparison)
{
    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");
    Variable z = pool.getFreshVariable("z");

    ComparisonList<Monomial> monomials;
    monomials.push_back(x * x * x);
    monomials.push_back(x * x * y);
    monomials.push_back(x * y * y);
    monomials.push_back(y * y * y);
    monomials.push_back(x * y * z);
    monomials.push_back(x * y * y * z);
    monomials.push_back(x * x * z * z);

    expectRightOrder(monomials);
}


TEST(Monomial, OtherComparison)
{
    ComparisonList<Variable,Monomial> list;

    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");

    list.push_back(x);
    list.push_back(y);
    list.push_back(x * x);
    list.push_back(x * y);
    list.push_back(y * y);
    list.push_back(x * x * x);
    list.push_back(x * x * y);
    list.push_back(x * x * x * x);

    expectRightOrder(list);
}
