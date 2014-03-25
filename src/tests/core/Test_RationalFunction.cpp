#include "gtest/gtest.h"
#include "carl/core/RationalFunction.h"
#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"
#include <gmpxx.h>

using namespace carl;

typedef RationalFunction<MultivariatePolynomial<mpq_class>>  RFunc;
typedef MultivariatePolynomial<mpq_class> Pol;

TEST(RationalFunction, Construction)
{
	StringParser sp;
    sp.setVariables({"x", "y", "z"});
    Pol p1 = sp.parseMultivariatePolynomial<mpq_class>("3*x*y + x");
    Pol p2 = sp.parseMultivariatePolynomial<mpq_class>("5*y + 3*x");
    
    RFunc r1(p1, p2);
    EXPECT_EQ(p1, r1.nominator());
    EXPECT_EQ(p2, r1.denominator());
    EXPECT_FALSE(r1.isZero());
}

TEST(RationalFunction, Multiplication)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    Pol p1 = sp.parseMultivariatePolynomial<mpq_class>("3*x*y + x");
    Pol p2 = sp.parseMultivariatePolynomial<mpq_class>("5*y");
    
    Pol p3 = sp.parseMultivariatePolynomial<mpq_class>("1*x");
    Pol p4 = sp.parseMultivariatePolynomial<mpq_class>("4*y");
    
    RFunc r1(p1, p2);
    RFunc r2(p3, p4);
    
    RFunc r3 = r1 * r2;
    Pol q1 = sp.parseMultivariatePolynomial<mpq_class>("3*x^2*y + x^2");
    Pol q2 = sp.parseMultivariatePolynomial<mpq_class>("20*y^2");
    EXPECT_EQ(r3.nominator(), q1);
    EXPECT_EQ(r3.denominator(), q2);
}
