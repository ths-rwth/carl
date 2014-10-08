#include "gtest/gtest.h"
#include "carl/core/RationalFunction.h"
#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"

#ifdef COMPARE_WITH_GINAC
#include <cln/cln.h>
typedef cln::cl_RA Rational;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
#endif

using namespace carl;

typedef RationalFunction<MultivariatePolynomial<Rational>>  RFunc;
typedef MultivariatePolynomial<Rational> Pol;

TEST(RationalFunction, Construction)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("3*x*y + x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("5*y + 3*x");
    
    RFunc r1(p1, p2);
    EXPECT_EQ(p1, r1.nominator());
    EXPECT_EQ(p2, r1.denominator());
    EXPECT_FALSE(r1.isZero());
}

TEST(RationalFunction, Multiplication)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("3*x*y + x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("5*y");
    
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1*x");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("4*y");
    
    RFunc r1(p1, p2);
    RFunc r2(p3, p4);
    
    RFunc r3 = r1 * r2;
    Pol q1 = sp.parseMultivariatePolynomial<Rational>("3*x^2*y + x^2");
    Pol q2 = sp.parseMultivariatePolynomial<Rational>("20*y^2");
    EXPECT_EQ(q1, r3.nominator());
    EXPECT_EQ(q2, r3.denominator());
}

TEST(RationalFunction, Addition)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("1*x*z");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("x");
    
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1*z");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("5*x");
    
    RFunc r1(p1, p2);
    RFunc r2(p3, p4);
    
    RFunc r3 = r1 + r2;
    Pol q2 = sp.parseMultivariatePolynomial<Rational>("5*x*y");
    EXPECT_EQ(p4, r3.denominator());
}
