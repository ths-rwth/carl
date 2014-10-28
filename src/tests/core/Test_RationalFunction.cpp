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
const bool AutoSimplify = true;
typedef RationalFunction<Pol,AutoSimplify> RFunc;
typedef RationalFunction<FPol,AutoSimplify> RFactFunc;
typedef Cache<PolynomialFactorizationPair<Pol>> CachePol;

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
    
    std::shared_ptr<CachePol> pCache( new CachePol );
    
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);
    
    RFactFunc rf1(fp1, fp2);
    EXPECT_EQ(computePolynomial(fp1), computePolynomial(rf1.nominator()));
    EXPECT_EQ(computePolynomial(fp2), computePolynomial(rf1.denominator()));
    EXPECT_FALSE(rf1.isZero());
}

TEST(RationalFunction, Multiplication)
{
    carl::VariablePool::getInstance().clear();
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
    
    Variable x = carl::VariablePool::getInstance().findVariableWithName("x");
    RFunc r4( x );
    r4 *= x;
    EXPECT_EQ(sp.parseMultivariatePolynomial<Rational>("x^2"), r4.nominator());
    EXPECT_FALSE(needs_cache<Pol>::value);
    
    std::shared_ptr<CachePol> pCache( new CachePol );
    
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);
    
    FPol fp3(p3, pCache);
    FPol fp4(p4, pCache);
    
    RFactFunc rf1(fp1, fp2);
    RFactFunc rf2(fp3, fp4);
    
    RFactFunc rf3 = rf1 * rf2;
    FPol qf1(q1, pCache);
    FPol qf2(q2, pCache);
    EXPECT_EQ(computePolynomial(qf1), computePolynomial(rf3.nominator()));
    EXPECT_EQ(computePolynomial(qf2), computePolynomial(rf3.denominator()));
    
    //(1/4*PF)/((-1/4)*PF+1) * ((-1/4)*PF+1)/((-1/2)*PF+1)
    carl::VariablePool::getInstance().clear();
    Variable t(1);
    Pol pf(t);
    Pol nomA( Rational(1)/Rational(4)*pf );
    Pol denA( Rational(-1)/Rational(4)*pf+Rational(1) );
    Pol nomB( Rational(-1)/Rational(4)*pf+Rational(1) );
    Pol denB( Rational(-1)/Rational(2)*pf+Rational(1) );
    FPol fpNomA(nomA, pCache);
    FPol fpDenA(denA, pCache);
    FPol fpNomB(nomB, pCache);
    FPol fpDenB(denB, pCache);
    RFactFunc rfA(fpNomA, fpDenA);
    RFactFunc rfB(fpNomB, fpDenB);
    std::cout << rfA << "*" << rfB << " = ";
    RFactFunc rfC = rfA * rfB;
    if( !AutoSimplify )
        rfC.simplify();
    std::cout << rfC << std::endl;
    EXPECT_TRUE( computePolynomial( FPol( fpNomA*fpDenA*fpNomB*fpDenB ) ).remainder( computePolynomial( FPol(rfC.nominator()*rfC.denominator()) ) ).isZero() );
}

TEST(RationalFunction, Addition)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("1*x*z");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("x");
    
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1*z");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("5*x");
    
    RFunc r1(p1, p2);
    RFunc r2(p3, p4);
    
    RFunc r3 = r1 + r2;
    EXPECT_EQ(p4, r3.denominator());
    
    std::shared_ptr<CachePol> pCache( new CachePol );
    
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);
    
    FPol fp3(p3, pCache);
    FPol fp4(p4, pCache);
    
    RFactFunc rf1(fp1, fp2);
    RFactFunc rf2(fp3, fp4);
    
    RFactFunc rf3 = rf1 + rf2;
    EXPECT_EQ(computePolynomial(fp4), computePolynomial(rf3.denominator()));
}

TEST(RationalFunction, Subtraction)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("1");

    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1+x");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("1");

    RFunc r1(p1, p2);
    RFunc r2(p3, p4);

    RFunc r3 = r1 - r2;
    EXPECT_EQ(p4, r3.denominator());

    std::shared_ptr<CachePol> pCache( new CachePol );

    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);

    FPol fp3(p3, pCache);
    FPol fp4(p4, pCache);

    RFactFunc rf1(fp1, fp2);
    RFactFunc rf2(fp3, fp4);

    RFactFunc rf3 = rf1 - rf2;
    FPol tmp = fp1 - fp3;
    EXPECT_EQ(computePolynomial(fp4), computePolynomial(rf3.denominator()));
}

TEST(RationalFunction, Hash)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("1+x");

    std::shared_ptr<CachePol> pCache( new CachePol );

    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);

    FPol tmp = fp2 - fp1;
}