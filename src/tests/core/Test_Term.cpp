#include "gtest/gtest.h"
#include "arithmetic/core/Term.h"
#include "arithmetic/core/Variable.h"
#include "arithmetic/core/Monomial.h"


using namespace arithmetic;

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


