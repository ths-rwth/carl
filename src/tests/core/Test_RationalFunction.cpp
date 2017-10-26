#include "gtest/gtest.h"
#include "carl/core/RationalFunction.h"
#include "carl/core/VariablePool.h"
#include "carl/util/stringparser.h"
#include "carl/core/FactorizedPolynomial.h"
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef FactorizedPolynomial<Pol> FPol;
const bool AutoSimplify = false;
typedef RationalFunction<Pol,AutoSimplify> RFunc;
typedef RationalFunction<FPol,AutoSimplify> RFactFunc;
typedef Cache<PolynomialFactorizationPair<Pol>> CachePol;

TEST(RationalFunction, Construction)
{
    StringParser sp;
    std::shared_ptr<CachePol> pCacheA( new CachePol );
    sp.setVariables({"x", "y", "z", "t", "u"});
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("3*x*y + x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("5*y + 3*x");
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("x");
    Pol p4 = p3;
    p4 *= Pol::CoeffType(-1);
    p4 += Pol::CoeffType(1);
    Pol p5 = sp.parseMultivariatePolynomial<Rational>("2184*x^17+15708*x+(-126672)*x^2+643384*x^3+(-2306444)*x^4+4162512*x^13+(-10186920)*x^12+18820800*x^11+(-27118448)*x^10+31123477*x^9+6199788*x^5+(-12956461)*x^6+21524503*x^7+(-28784511)*x^8+(-1226048)*x^14+245224*x^15+(-31192)*x^16+(-924)");
    Pol p6 = sp.parseMultivariatePolynomial<Rational>("3360*x^16+(-33600)*x^13+33600*x^14+(-16800)*x^15+16800*x^12+(-3360)*x^11");
    Pol p7 = sp.parseMultivariatePolynomial<Rational>("t^3*u^6+(-1)*u^6+(-3)*t^2*u^6+3*t*u^6+(-1)*t^3*u^5+(-1)*t^3+(-3)*t+3*t^2+(-3)*u+3*t^3*u+9*t*u+(-9)*t^2*u+5*u^2+(-5)*t^3*u^2+(-15)*t*u^2+15*t^2*u^2+3*t^2*u^4+(-3)*t*u^4+(-1)*t^3*u^4+(-4)*u^3+u^4+(-12)*t^2*u^3+12*t*u^3+4*t^3*u^3+u^5+3*t^2*u^5+(-3)*t*u^5+1");
    FPol fp7(p7, pCacheA);
    Pol p8 = sp.parseMultivariatePolynomial<Rational>("u^2+(-1)*u+1");
    FPol fp8(p8, pCacheA);
    Pol p9 = sp.parseMultivariatePolynomial<Rational>("2*u^2+(-2)*u+1");
    FPol fp9(p9, pCacheA);
    Pol p10 = sp.parseMultivariatePolynomial<Rational>("2*t^2+(-2)*t+1");
    FPol fp10(p10, pCacheA);
    FPol fpDenom = fp8 * fp9 * fp10;

    RFunc r1(p1, p2);
    EXPECT_EQ(p1, r1.nominator());
    EXPECT_EQ(p2, r1.denominator());
    EXPECT_FALSE(r1.isZero());

    RFunc r2(p4);
    std::cout << "Construct rational function from " << p4 << " leads to " << r2 << std::endl;
    EXPECT_EQ(p4, r2.nominator());

    RFunc r3(p5, p6);

    std::shared_ptr<CachePol> pCache( new CachePol );

    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);
    FPol fp3(p4, pCache);
    FPol fp5(p5, pCache);
    FPol fp6(p6, pCache);

    RFactFunc rf1(fp1, fp2);
    EXPECT_EQ(computePolynomial(fp1), computePolynomial(rf1.nominator()));
    EXPECT_EQ(computePolynomial(fp2), computePolynomial(rf1.denominator()));
    EXPECT_FALSE(rf1.isZero());

    RFactFunc rf2(fp3);
    std::cout << "Construct factorized rational function from " << p4 << " leads to " << r2 << std::endl;
    EXPECT_EQ(p4, computePolynomial(rf2.nominator()));

    RFactFunc rf3(fp5, fp6);

    RFactFunc rf4(fp7, fpDenom);
    std::cout << rf4 << std::endl;
    rf4.simplify();
    std::cout << rf4 << std::endl;
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
    EXPECT_TRUE(needs_cache<FPol>::value);

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
    RFactFunc rf4 = rf3 * 2;
    RFactFunc rf5 = 2 * rf3;
    EXPECT_EQ(rf4, rf5);


//(1/4*PF)/((-1/4)*PF+1) * ((-1/4)*PF+1)/((-1/2)*PF+1)
    carl::VariablePool::getInstance().clear();
    Variable t = carl::freshRealVariable("t");
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

TEST(RationalFunction, Division)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("2");
    Pol p5 = p1*p4 + p2;
    RFunc r1(p1, p2);
    RFunc r2(p3, p4);

    RFunc r3 = r1 + r2;
    RFunc r4 = r2 / r3;
    EXPECT_EQ(p5, r3.nominator());
    EXPECT_EQ(p5, r4.denominator());
    EXPECT_EQ(p2, r4.nominator());

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);
    FPol fp3(p3, pCache);
    FPol fp4(p4, pCache);

    RFactFunc rf1(fp1, fp2);
    RFactFunc rf2(fp3, fp4);

    RFactFunc rf3 = rf1 + rf2;
    RFactFunc rf4 = rf2 / rf3;
    std::cout << rf4 << std::endl;
    EXPECT_EQ(p5, computePolynomial(rf3.nominator()));
    EXPECT_EQ(p5, computePolynomial(rf4.denominator()));
    EXPECT_EQ(p2, computePolynomial(rf4.nominator()));
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

    Pol p0 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("1");

    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1+x");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol p7 = sp.parseMultivariatePolynomial<Rational>("x");
    Pol p6 = p7;
    p6 *= Pol::CoeffType(-1);
    p6 += Pol::CoeffType(1);

    RFunc r1(p1, p2);
    RFunc r2(p3, p4);

    RFunc r3 = r1 - r2;
    EXPECT_EQ(p4, r3.denominator());

    RFunc r4(p0);
    RFunc r5(p7);
    RFunc r6 = r4 - r5;
    std::cout << "Calculate: " << r4 << " - " << r5 << " = " << r6 << std::endl;
    EXPECT_EQ(p6, r6.nominator());

    std::shared_ptr<CachePol> pCache( new CachePol );

    FPol fp0(p0, pCache);
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);

    FPol fp3(p3, pCache);
    FPol fp4(p4, pCache);
    FPol fp6(p6, pCache);
    FPol fp7(p7, pCache);

    RFactFunc rf1(fp1, fp2);
    RFactFunc rf2(fp3, fp4);

    RFactFunc rf3 = rf1 - rf2;
    FPol tmp = fp1 - fp3;
    EXPECT_EQ(computePolynomial(fp4), computePolynomial(rf3.denominator()));

    RFactFunc rf4(fp0);
    RFactFunc rf5(fp7);
    RFactFunc rf6 = rf4 - rf5;
    std::cout << "Calculate: " << rf4 << " - " << rf5 << " = " << rf6 << std::endl;
    EXPECT_EQ(fp6, rf6.nominator());
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

TEST(RationalFunction, Derivative)
{
	carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

	// from http://de.wikipedia.org/wiki/Quotientenregel#Beispiel
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("1*x^2 + -1");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("2 + -3*x");

    Pol p3 = sp.parseMultivariatePolynomial<Rational>("-3*x^2 + 4*x + -3");

	std::shared_ptr<CachePol> pCache( new CachePol );
	FPol fp1(p1, pCache);
	FPol fp2(p2, pCache);
	FPol fp3(p3, pCache);

    RFactFunc r1(fp1, fp2);
    RFactFunc r2(fp3, fp2*fp2);

	EXPECT_EQ(r2, r1.derivative(sp.variables().at("x")));
}

TEST(RationalFunction, Simplification)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y"});
    Pol p1 = sp.parseMultivariatePolynomial<Rational>("2*x*y+(-2)*y+3");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("128*x^12*y^7+43740*x^3+8748*x^5+109350*x^2*y+257580*x^2*y^3+63990*x^2*y^5+(-12960)*x^2*y^6+(-168750)*x^2*y^4+1080*x^2*y^7+(-228420)*x^2*y^2+(-313470)*x^3*y+(-177147)*x^5*y+975402*x^5*y^2+59292*x^7*y^2+2292705*x^4*y^3+1156860*x^4*y^5+(-311760)*x^4*y^6+(-2200500)*x^4*y^4+33480*x^4*y^7+(-2345355)*x^5*y^3+(-99720)*x^9*y^5+(-19728)*x^10*y^6+9168*x^10*y^7+444960*x^8*y^5+560880*x^7*y^6+(-100968)*x^7*y^7+(-295920)*x^8*y^6+67480*x^8*y^7+(-30760)*x^9*y^7+100080*x^9*y^6+(-1135710)*x^7*y^5+1037340*x^7*y^4+(-2222910)*x^6*y^4+(-1835892)*x^5*y^5+587088*x^5*y^6+1808910*x^6*y^5+(-705168)*x^6*y^6+2859840*x^5*y^4+(-73208)*x^5*y^7+104216*x^6*y^7+(-270000)*x^8*y^4+(-421605)*x^7*y^3+1355535*x^6*y^3+1728*x^11*y^6+9792*x^10*y^5+30240*x^9*y^4+55080*x^8*y^3+(-1284255)*x^4*y^2+(-377379)*x^6*y^2+(-1193940)*x^3*y^3+(-413190)*x^3*y^5+95760*x^3*y^6+934740*x^3*y^4+(-9000)*x^3*y^7+855360*x^3*y^2+346275*x^4*y+34992*x^6*y+(-21870)*x^2+(-1616)*x^11*y^7+(-32805)*x^4");
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("x+1");
    Pol q1 = sp.parseMultivariatePolynomial<Rational>("x*y+(-1)*y+1");
    Pol q2 = sp.parseMultivariatePolynomial<Rational>("y+1");
    Pol q3 = sp.parseMultivariatePolynomial<Rational>("2*x*y+(-2)*y+3");
    Pol p4 = p3*q2*q2;
    
    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);
    FPol fq1(q1, pCache);
    FPol fq2(q2, pCache);
    FPol fq3(q3, pCache);
    FPol fp4(p4, pCache);
    
    RFactFunc r1( fp4, (fq2*fq2) );
    std::cout << r1 << std::endl;
    r1.simplify();
    std::cout << r1 << std::endl;
    EXPECT_TRUE( r1.denominator().isOne() );

    RFactFunc r2( (fq2*fq2), fp4 );
    r2.simplify();
    EXPECT_TRUE( r2.nominator().isOne() );
}

TEST(RationalFunction, Evaluation)
{
    carl::VariablePool::getInstance().clear();
    Variable x = freshRealVariable("x");
    Pol p1({Rational(3)*x});
    Pol p2(Rational(2));

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1(p1, pCache);
    FPol fp2(p2, pCache);

    RFunc r1( p1, p2 );
    RFactFunc rf1( fp1, fp2 );

    std::map<Variable,Rational> substitutions;
    substitutions[x] = Rational(4);

    Rational resr1 = r1.evaluate(substitutions);
    EXPECT_EQ( Rational(6), resr1 );
    Rational resrf1 = rf1.evaluate(substitutions);
    EXPECT_EQ( Rational(6), resrf1 );
}

TEST(RationalFunction, Substitute)
{
    carl::StringParser parser;
    parser.setVariables({"x", "y", "z"});
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Variable z = freshRealVariable("z");

    // Nessecessary - if not present, later formula parsing fails with BUGGY exception!
    RFunc rf1 = RFunc(Pol(x));
    RFunc rf2 = RFunc(Pol(y));
    RFunc rf3 = RFunc(Pol(z));
   
    RFunc rf4(RFunc(Rational(2)) * rf1);
    RFunc rf5(rf1 * rf2);
    RFunc rf6(rf3 / RFunc(Rational(2)));
   
    RFunc rationalFunction(rf4 + rf5 + rf6);

    RFunc rf7(Pol(z), Pol(Rational(2)));
   
    std::map<Variable, Rational> replacement = {{x, Rational(2)}};
    RFunc subX = rationalFunction.substitute(replacement);
   
    RFunc cmp(z / Rational(2) + Rational(2) * y + Rational(4), Pol(1));
    EXPECT_EQ(subX, cmp);
   
    Pol poly(Rational(2) * x + x*y + z / Rational(2));
    Pol polySub = poly.substitute(replacement);
    EXPECT_EQ(polySub, z / Rational(2) + Rational(2) * y + Rational(4));
}
