#include "gtest/gtest.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Term.h"

using namespace carl;

TEST(Term, Constructor)
{
    Term<int> t(1);
    Variable v0(0);
    Monomial m0(v0);
    Term<int> t0(m0);
}

TEST(Term, Operators)
{
    Term<int> t(1);
    Variable v0(0);
    Monomial m0(v0);
    Term<int> t0(m0);
    EXPECT_TRUE(t0.isLinear());
    EXPECT_TRUE(t.isLinear());
    EXPECT_EQ(0, t.getNrVariables());
}

TEST(Term, Multiplication)
{
    Term<int> t(1);
    Variable v0(0);
    t *= v0;
    Term<int> t0(v0);
    EXPECT_EQ(t0,t);
    t *= v0;
    Term<int> t1(Monomial(v0,2));
    EXPECT_EQ(t1,t);
    
}

TEST(Term, Derivative)
{
    Variable v0(0);
    Variable v1(1);
    Term<int> t(3);
    t *= v0 * v0 * v0 * v1;
    Term<int>* tprime = t.derivative(v0);
    EXPECT_EQ(9,tprime->coeff());
    
}
