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
    EXPECT_FALSE(t.isLinear());
    EXPECT_TRUE(t0.isAtMostLinear());
    EXPECT_TRUE(t.isAtMostLinear());
    EXPECT_EQ(0, t.nrVariables());
}

TEST(Term, Multiplication)
{
 
}


