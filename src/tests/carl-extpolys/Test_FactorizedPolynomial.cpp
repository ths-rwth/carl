#include "gtest/gtest.h"
#include <carl/core/MultivariatePolynomial.h>
#include <carl-extpolys/FactorizedPolynomial.h>
#include <carl-extpolys/substitution.h>
#include <carl/core/polynomialfunctions/LCM.h>
#include <carl/core/polynomialfunctions/Quotient.h>
#include <carl/util/stringparser.h>
#include <carl-common/meta/platform.h>
#include <carl/numbers/numbers.h>

#include "../Common.h"

using namespace carl;

typedef mpq_class Rational;
typedef MultivariatePolynomial<Rational> Pol;
typedef FactorizedPolynomial<Pol> FPol;
typedef Cache<PolynomialFactorizationPair<Pol>> CachePol;

/* General testing function for a specific binary operator.
 * It is result = operator( pol1, pol2 ).
 * @param sVars List of variables occuring in test.
 * @param sPol1 String representation of first polynomial.
 * @param sPol2 String representation of second polynomial.
 * @param operatorPol Binary operator on Pol.
 * @param operatorFPol Binary operator on FPol.
 * @return true, if both results are equal, false otherwise.
 */
bool testOperation( const std::list<std::string>& sVars, const std::string& sPol1, const std::string& sPol2, std::function<Pol( Pol, Pol )> operatorPol, std::function<FPol( FPol, FPol )> operatorFPol )
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables(sVars);

    Pol p1 = sp.parseMultivariatePolynomial<Rational>(sPol1);
    Pol p2 = sp.parseMultivariatePolynomial<Rational>(sPol2);

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );

    Pol pResult = operatorPol( p1, p2 );
    FPol fpResult = operatorFPol( fp1, fp2 );

    return pResult == computePolynomial( fpResult );
}


/* General testing function for a specific binary function.
 * It is result = operator( pol1, pol2 ).
 * @param sVars List of variables occuring in test.
 * @param sPol1 String representation of first polynomial.
 * @param sPol2 String representation of second polynomial.
 * @param functionPol Binary function on Pol.
 * @param functionFPol Binary function on FPol.
 * @return true, if both results are equal, false otherwise.
 */
bool testOperation( const std::list<std::string>& sVars, const std::string& sPol1, const std::string& sPol2, const Pol (*functionPol)( const Pol&, const Pol& ), const FPol (*functionFPol)( const FPol&, const FPol& ) )
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables(sVars);

    Pol p1 = sp.parseMultivariatePolynomial<Rational>(sPol1);
    Pol p2 = sp.parseMultivariatePolynomial<Rational>(sPol2);

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );

    Pol pResult = functionPol( p1, p2 );
    FPol fpResult = functionFPol( fp1, fp2 );

    return pResult == computePolynomial( fpResult );
}

/* General testing function for specific unary member function.
 * It is result = pol1.function( pol2 ).
 * @param sVars List of variables occuring in test.
 * @param sPol1 String representation of first polynomial.
 * @param sPol2 String representation of second polynomial.
 * @param functionPol Unary member function on Pol.
 * @param functionFPol Unary member function on FPol.
 * @return true, if both results are equal, false otherwise.
 */
bool testOperation( const std::list<std::string>& sVars, const std::string& sPol1, const std::string& sPol2, const Pol (Pol::*functionPol)( const Pol& ), const FPol (FPol::*functionFPol)( const FPol& ) )
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables(sVars);

    Pol p1 = sp.parseMultivariatePolynomial<Rational>(sPol1);
    Pol p2 = sp.parseMultivariatePolynomial<Rational>(sPol2);

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );

    Pol pResult = (p1.*(functionPol))( p2 );
    FPol fpResult = (fp1.*(functionFPol))( fp2 );

    return pResult == computePolynomial( fpResult );
}

TEST(FactorizedPolynomial, Construction)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol c1 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol c2 = sp.parseMultivariatePolynomial<Rational>("42");
    Pol c3 = sp.parseMultivariatePolynomial<Rational>("-2");
    Pol c4 = sp.parseMultivariatePolynomial<Rational>("0");
    Pol fx = sp.parseMultivariatePolynomial<Rational>("x");
    Pol fz = sp.parseMultivariatePolynomial<Rational>("z");
    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("x*y*z");
    Pol f1 = sp.parseMultivariatePolynomial<Rational>("-1*x + 3*y");
    Pol f2 = sp.parseMultivariatePolynomial<Rational>("x + -1*x^2 + 3*x^3");
    Pol f3 = f1*f1*f2;
    Pol f4 = f1*f2*f2;
    Pol f5 = sp.parseMultivariatePolynomial<Rational>("-1*x + 1");
    Pol f6 = sp.parseMultivariatePolynomial<Rational>("x");
    Pol f7 = f6 - c1;

    auto pCache = std::make_shared<CachePol>();
    FPol fc1( (Rational) 1 );
    FPol fc2( (Rational) 42 );
    FPol fc3( (Rational) -2 );
    FPol fc4( (Rational) 0 );
    FPol fpx( fx, pCache );
    FPol fpz( fz, pCache);
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );
    FPol fp2( f2, pCache );
    FPol fp3( f3, pCache );
    FPol fp4( f4, pCache );
    FPol fp5( f5, pCache );
    FPol fp7( f7, pCache );
    FPol fp8 = fpx * fpA;
    FPol fp9 = fpA * fpA;

    //Common divisor
    FPol fpCD = commonDivisor( fpA, fpB );
    FPol fpAc1 = commonDivisor( fpA, fc1 );
    EXPECT_EQ( fc1, fpAc1 );
    FPol fpc2B = commonDivisor( fc2, fpB );
    EXPECT_EQ( fc1, fpc2B );

    //GCD
    FPol fpRestA0;
    FPol fpRestB0;
    FPol fpGCD0 = gcd( fp8, fp9, fpRestA0, fpRestB0 );
    EXPECT_EQ( fp8, fpRestA0 * fpGCD0 );
    EXPECT_EQ( fp9, fpRestB0 * fpGCD0 );
    EXPECT_EQ( computePolynomial(fp8), computePolynomial(fpRestA0 * fpGCD0) );
    EXPECT_EQ( computePolynomial(fp9), computePolynomial(fpRestB0 * fpGCD0) );

    FPol fpRestA;
    FPol fpRestB;
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );
    EXPECT_EQ( fpA, fpRestA * fpGCD );
    EXPECT_EQ( fpB, fpRestB * fpGCD );

    FPol fpRest3;
    FPol fpRest4;
    FPol fpGCDB = gcd( fp3, fp4, fpRest3, fpRest4 );
    EXPECT_EQ( fp3, fpRest3 * fpGCDB );
    EXPECT_EQ( fp4, fpRest4 * fpGCDB );

    //Common Multiple
    FPol fpCM = commonMultiple( fpA, fpB );
    EXPECT_EQ( fpB, fpCM );
    FPol fpAc3 = commonMultiple( fpA, fc3 );
    EXPECT_EQ( fpAc3, fpA * -fc3 );
    FPol fpc3B = commonMultiple( fc2, fpB );
    EXPECT_EQ( fpc3B, fpB * fc2 );

    //Quotient
    FPol fpQuo = quotient( fpB, fpA );
    EXPECT_EQ( fpQuo, fpz );
    FPol fpBc3 = quotient( fpB, fc3 );
    EXPECT_EQ( fpBc3.constantPart(), -1/2 );
    EXPECT_EQ( fpBc3.polynomial(), fB );
    FPol fpc4A = quotient( fc4, fpA );
    EXPECT_EQ( fpc4A, fc4 );
    FPol fpAc1b = quotient( fpA, fc1 );
    EXPECT_EQ( fpAc1b, fpA );

    //LCM
    Pol fLCM = lcm( fA, fB );
    FPol fpLCM = lcm( fpA, fpB );
    EXPECT_EQ( fLCM, computePolynomial( fpLCM ) );

    //Multiplication
    Pol fMul = fA * fB;
    FPol fpMul = fpA * fpB;
    EXPECT_EQ( fMul, computePolynomial( fpMul ) );
    Pol fMul2 = fA * c2;
    FPol fpMul2 = fpA * fc2;
    EXPECT_EQ( fMul2, computePolynomial( fpMul2 ) );
    Pol fMul3 = c4 * fB;
    FPol fpMul3 = fc4 * fpB;
    EXPECT_EQ( fMul3, computePolynomial( fpMul3 ) );
    Pol fMul4 = c2 * c3;
    FPol fpMul4 = fc2 * fc3;
    EXPECT_EQ( fMul4, computePolynomial( fpMul4 ) );

    //Some test
    FPol fres = fp5;
    FPol ftest = fp7;
    ftest *= fp7;
    ftest *= fp7;
    ftest *= fp7;
    ftest *= fp7;
    ftest *= fp2;
    fres *= ftest;
    EXPECT_EQ( (f5*carl::pow(f7, 5)*f2), computePolynomial( fres ) );

    //Addition
    Pol fAdd = fA + fB;
    FPol fpAdd = fpA + fpB;
    EXPECT_EQ( fAdd, computePolynomial( fpAdd ) );

    //Subtraction
    Pol fSub = fA - fB;
    FPol fpSub = fpA - fpB;
    EXPECT_EQ( fSub, computePolynomial( fpSub ) );

    //Unary minus
    FPol fpAMinus = -fpA;
    FPol fpAMinus2 = -fpAMinus;
    EXPECT_EQ( fpA, fpAMinus2 );
}

TEST(FactorizedPolynomial, Coefficient)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y"});

    Pol fA = sp.parseMultivariatePolynomial<Rational>("3*x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("4*x");

    auto pCache = std::make_shared<CachePol>();
    FPol fpc1( (Rational) 5);
    FPol fpc2( (Rational) -4);
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );
    FPol fpc3 = fpc1 * fpc2;
    FPol fpC = fpA * fpB;

    EXPECT_EQ(fpc3.coefficient(), -20);
    EXPECT_EQ(fpc3.constantPart(), -20);
    EXPECT_EQ(fpC.coefficient(), 12);
    EXPECT_EQ(fpC.constantPart(), 0);
}

TEST(FactorizedPolynomial, CommonDivisor)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("x*y*z");
    Pol fz = sp.parseMultivariatePolynomial<Rational>("z");

    auto pCache = std::make_shared<CachePol>();
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );
    FPol fpz( fz, pCache );
    FPol fpB2 = fpA * fpz;

    FPol fpC = commonDivisor( fpA, fpB );
    EXPECT_EQ(fpC, (Rational)1);
    FPol fpD = commonDivisor( fpA, fpB2 );
    EXPECT_EQ(fpD, fpA);
}

TEST(FactorizedPolynomial, GCD)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("4*x*y");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("2*x*y*z");

    Variable t = freshRealVariable("t");
    Pol one(Rational(1));
    Pol g1 = t*t - one;
    Pol g2 = t - one;

    auto pCache = std::make_shared<CachePol>();
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );
    FPol fpRestA;
    FPol fpRestB;
    FPol fg1( g1, pCache );
    FPol fg2( g2, pCache );

    Pol pGCD = Pol(2)*gcd( pA, pB );
    Pol pRestA = carl::quotient(pA, pGCD );
    Pol pRestB = carl::quotient(pB, pGCD );
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );
    EXPECT_EQ( pGCD, computePolynomial( fpGCD ) );
    EXPECT_EQ( pRestA, computePolynomial( fpRestA ) );
    EXPECT_EQ( pRestB, computePolynomial( fpRestB ) );
    FPol fgRestA;
    FPol fgRestB;
    FPol fgGCD = gcd( fg1, fg2, fgRestA, fgRestB );
    EXPECT_EQ( g1, computePolynomial( fgRestA ) * computePolynomial( fgGCD ) );
    EXPECT_EQ( g2, computePolynomial( fgRestB ) * computePolynomial( fgGCD ) );

    Pol x = sp.parseMultivariatePolynomial<Rational>("x");
    Pol y = sp.parseMultivariatePolynomial<Rational>("y");
    Pol t1 = x*x*x;
    Pol f1 = sp.parseMultivariatePolynomial<Rational>("x^6+6*y^5*x+15*y^4*x^2+20*y^3*x^3+15*y^2*x^4+y^6+6*y*x^5");
    Pol t2 = x+y;
    Pol t4 = Rational(2)*x+y;
    Pol t5 = Rational(4)*x+y;
    Pol t6 = Rational(4)*x+Rational(3)*y;
    Pol t7 = Rational(4)*x*x*x+Rational(6)*y*x*x+Rational(4)*y*y*x+y*y*y;
    FPol fx(x, pCache);
    FPol ft1 = fx * fx * fx;
    FPol ff1(f1, pCache);
    FPol fpol1 = ft1 * ff1;
    FPol ft2(t2, pCache);
    FPol ft3 = ft2 * ft2 * ft2;
    FPol tmp = ft3 * ft2;
    FPol ft4(t4, pCache);
    FPol ft5(t5, pCache);
    FPol ft6(t6, pCache);
    FPol ft7(t7, pCache);
    FPol fpol2 = ft3 * ft4 * ft5 * ft6 * ft7;
    FPol fpolRest1;
    FPol fpolRest2;
    FPol fpolGCD = gcd( fpol1, fpol2, fpolRest1, fpolRest2);
    EXPECT_EQ( computePolynomial( fpol1 ), computePolynomial( fpolRest1 ) * computePolynomial( fpolGCD ) );
    EXPECT_EQ( computePolynomial( fpol2 ), computePolynomial( fpolRest2 ) * computePolynomial( fpolGCD ) );
    EXPECT_EQ( fpolGCD, ft3 );
}

TEST(FactorizedPolynomial, Flattening)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("4*x*y*z");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol pC = sp.parseMultivariatePolynomial<Rational>("x");

    auto pCache = std::make_shared<CachePol>();
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );
    FPol fpC( pC, pCache );
    FPol fpRestA;
    FPol fpRestB;
    FPol fpRestC;

    Pol pGCD = gcd( pA, pB );
    Pol pRestA = carl::quotient(pA, pGCD );
    Pol pRestB = carl::quotient(pB, pGCD );
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );

    EXPECT_EQ( pGCD, computePolynomial( fpGCD ) );
    EXPECT_EQ( pRestA, computePolynomial( fpRestA ) );
    EXPECT_EQ( pRestB, computePolynomial( fpRestB ) );

    Pol pGCD2 = gcd( pA, pC );
    pRestA = carl::quotient(pA, pGCD2 );
    Pol pRestC = carl::quotient(pC, pGCD2 );
    FPol fpGCD2 = gcd( fpA, fpC, fpRestA, fpRestC );

    EXPECT_EQ( pGCD2, computePolynomial( fpGCD2 ) );
    EXPECT_EQ( pRestA, computePolynomial( fpRestA ) );
    EXPECT_EQ( pRestC, computePolynomial( fpRestC ) );
}

TEST(FactorizedPolynomial, Flattening2)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("x");

    auto pCache = std::make_shared<CachePol>();
    FPol fpA( pA, pCache );
    FPol fpB = fpA * fpA;
    FPol fpC = fpB * fpB;
    fpC.factorization();
    EXPECT_EQ( fpC.factorization().size(), 1 );
    EXPECT_EQ( fpC.factorization().begin()->second, 4 );
}

TEST(FactorizedPolynomial, LCM)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("x*y*z");

    auto pCache = std::make_shared<CachePol>();
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );

    Pol pLCM = lcm( pA, pB );
    FPol fpLCM = lcm( fpA, fpB );
    EXPECT_EQ( pLCM, computePolynomial( fpLCM ) );

    carl::VariablePool::getInstance().clear();
    Variable y = carl::freshRealVariable("y");
    Variable x = carl::freshRealVariable("x");
    Pol px( x );
    Pol py( y );
    Pol p1( px*py-py+Rational(1) );
    p1 *= Rational(3);
    Pol p2( Rational(2)*px*py-Rational(2)*py+Rational(3) );

    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    EXPECT_FALSE( (p1<p2) == (p2<p1) );
    EXPECT_FALSE( (fp2<fp1) == (fp1<fp2) );
    Pol p12LCM = lcm( p1, p2 );
    FPol fp12LCM = lcm( fp1, fp2 );
    EXPECT_EQ( p12LCM, computePolynomial( fp12LCM ) );
}

TEST(FactorizedPolynomial, Subtraction)
{
    bool expect = testOperation( {"x", "y"}, "1", "x*y", std::minus<Pol>(), std::minus<FPol>() );
    EXPECT_TRUE( expect );

    expect = testOperation( {"x", "y"}, "x*y", "1", std::minus<Pol>(), std::minus<FPol>() );
    EXPECT_TRUE( expect );

    expect = testOperation( {"x", "y", "z"}, "x*y", "z", std::minus<Pol>(), std::minus<FPol>() );
    EXPECT_TRUE( expect );

    expect = testOperation( {"x", "y", "z"}, "z", "x*y", std::minus<Pol>(), std::minus<FPol>() );
    EXPECT_TRUE( expect );
}

TEST(FactorizedPolynomial, Addition)
{
    bool expect = testOperation( {"x"}, "x", "-1*x+1", std::plus<Pol>(), std::plus<FPol>() );
    EXPECT_TRUE( expect );
}

TEST(FactorizedPolynomial, Multiplication)
{
    bool expect = testOperation( {"x", "y"}, "3*x*y + x", "x*y", std::multiplies<Pol>(), std::multiplies<FPol>() );
    EXPECT_TRUE( expect );

    expect = testOperation( {"x", "y"}, "5*y", "4*y", std::multiplies<Pol>(), std::multiplies<FPol>() );
    EXPECT_TRUE( expect );
}

TEST(FactorizedPolynomial, Quotient)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("9*x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("y");
    Pol p3 = p1*p2;
    Pol p4 = p3*p2;

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = fp1 * fp2;
    FPol fp4 = fp3 * fp2;

    Pol pQuot = carl::quotient(p4, p3);
    FPol fpQuot = fp4.quotient(fp3);
    EXPECT_EQ( pQuot, computePolynomial( fpQuot ) );
}

TEST(FactorizedPolynomial, Destructor)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("9*x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("y");
    Pol p3 = p1*p2;
    Pol p4 = p3*p2;

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = fp1 * fp2;
    FPol fp4 = fp3 * fp2;

    auto pfp5 = std::make_unique<FPol>( fp1 * fp1 );

    Pol pQuot = carl::quotient(p4, p3);
    FPol fpQuot = fp4.quotient(fp3);
    EXPECT_EQ( pQuot, computePolynomial( fpQuot ) );
}

TEST(FactorizedPolynomial, Equality)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("9*x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("x");
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("3*y");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("y");
    Pol p5 = sp.parseMultivariatePolynomial<Rational>("x+1");
    Pol p6 = sp.parseMultivariatePolynomial<Rational>("1");

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3( p3, pCache );
    FPol fp4( p4, pCache );
    FPol fp5( p5, pCache );
    FPol fp6( p6, pCache );

    EXPECT_EQ( fp1 == fp2, false );
    EXPECT_EQ( fp3 == fp4, false );
    EXPECT_EQ( fp2 == fp4, false );
    EXPECT_EQ( fp2 == fp5, false );

    FPol fpEq1 = fp1 * fp3;
    FPol fpEq2 = fp3 * fp1;
    EXPECT_EQ( fpEq1 == fpEq2, true );

    FPol fpEq3 = fp1 * fp4 * fp4;
    FPol fpEq4 = fp2 * fp3 * fp3;
    EXPECT_EQ( fpEq3 == fpEq4, true );

    FPol fpEq5 = fp2 + fp6;
    EXPECT_EQ( fpEq5 == fp5, true );

    FPol fpEq6 = fp5 - fp6;
    EXPECT_EQ( fpEq6 == fp2, true );
}

TEST(FactorizedPolynomial, Evaluation)
{
    carl::VariablePool::getInstance().clear();
    Variable x = freshRealVariable("x");
    Pol p1({(Rational)6*x});
    Pol p2({x});

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = fp1 * fp2;

    std::map<carl::Variable, Rational> substitution;
    substitution[x] = 3;

    Rational result = carl::evaluate(fp1, substitution);
    EXPECT_EQ( Rational(18), result );
    result = carl::evaluate(fp2, substitution);
    EXPECT_EQ( Rational(3), result );
    result = carl::evaluate(fp3, substitution);
    EXPECT_EQ( Rational(54), result );
}

TEST(FactorizedPolynomial, Substitution)
{
    carl::VariablePool::getInstance().clear();
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Pol pc(-3);
    Pol p1({(Rational)6*x});
    Pol p2({x});
    Pol p3({(Rational)5*y});

    auto pCache = std::make_shared<CachePol>();
    FPol fpc( pc, pCache );
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3( p3, pCache );
    FPol fp4 = fp1 * fp2;
    FPol fp5 = fp4 * fp3;

    std::map<carl::Variable, Rational> substitution;
    substitution[x] = 3;

    FPol result = carl::substitute(fpc, substitution);
    EXPECT_TRUE( result.isConstant() );
    EXPECT_EQ( Rational(-3), result.constantPart() );
    EXPECT_EQ( Rational(-3), result );
    result = carl::substitute(fp1, substitution);
    EXPECT_EQ( Rational(18), result );
    result = carl::substitute(fp2, substitution);
    EXPECT_EQ( Rational(3), result );
    result = carl::substitute(fp3, substitution);
    EXPECT_EQ( fp3, result );
    result = carl::substitute(fp4, substitution);
    EXPECT_EQ( Rational(54), result );
    result = carl::substitute(fp5, substitution);
    EXPECT_EQ( Rational(54)*fp3, result );

    std::map<carl::Variable, Rational> substitution2;
    substitution2[y] = 5;
    result = carl::substitute(fp5, substitution2);
    EXPECT_EQ( Rational(150)*fp2*fp2, result );

    std::map<carl::Variable, Rational> substitution3;
    substitution3[x] = -2;
    substitution3[y] = 0;
    result = carl::substitute(fp1, substitution3);
    EXPECT_EQ( Rational(-12), result );
    result = carl::substitute(fp2, substitution3);
    EXPECT_EQ( Rational(-2), result );
    result = carl::substitute(fp3, substitution3);
    EXPECT_EQ( Rational(0), result );
    result = carl::substitute(fp4, substitution3);
    EXPECT_EQ( Rational(24), result );
    result = carl::substitute(fp5, substitution3);
    EXPECT_EQ( Rational(0), result );
    substitution3[y] = 3;
    result = carl::substitute(fp5, substitution3);
    EXPECT_EQ( Rational(360), result );
}

TEST(FactorizedPolynomial, Derivation)
{
    carl::VariablePool::getInstance().clear();
    Variable x = freshRealVariable("x");

    FPol c(3);
    FPol der = c.derivative(x);
    EXPECT_EQ( der, FPol({Rational(0)}) );

    Pol p1({(Rational)6*x});
    Pol p2({x});

    auto pCache = std::make_shared<CachePol>();
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = (fp1*fp2) * (fp2 + fp1);

    FPol derivation = fp3.derivative(x, 1);

    Pol p3({(Rational)126*x*x});
    FPol fp4( p3, pCache );
    EXPECT_EQ( fp4, derivation );
}
