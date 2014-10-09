#include "gtest/gtest.h"
#include "carl/core/RationalFunction.h"
#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"
#include "carl/core/FactorizedPolynomial.h"

#ifdef COMPARE_WITH_GINAC
#include <cln/cln.h>
typedef cln::cl_RA Rational;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
#endif

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef FactorizedPolynomial<Pol> FPol;
typedef RationalFunction<Pol> RFunc;
typedef RationalFunction<FPol> RFactFunc;

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
    
    Cache<PolynomialFactorizationPair<Pol>> fpCache;
    
    FPol fp1(p1, fpCache);
    FPol fp2(p2, fpCache);
    
    RFactFunc rf1(fp1, fp2);
    EXPECT_EQ(fp1, rf1.nominator());
    EXPECT_EQ(fp2, rf1.denominator());
    EXPECT_FALSE(rf1.isZero());
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
    
    Cache<PolynomialFactorizationPair<Pol>> fpCache;
    
    FPol fp1(p1, fpCache);
    FPol fp2(p2, fpCache);
    
    FPol fp3(p3, fpCache);
    FPol fp4(p4, fpCache);
    
    RFactFunc rf1(fp1, fp2);
    RFactFunc rf2(fp3, fp4);
    
    RFactFunc rf3 = rf1 * rf2;
    FPol qf1(q1, fpCache);
    FPol qf2(q2, fpCache);
    EXPECT_EQ(qf1, rf3.nominator());
    EXPECT_EQ(qf2, rf3.denominator());
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
