#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/FactorizedPolynomial.h"
#include "carl/util/stringparser.h"
#include "carl/util/platform.h"

#ifdef __WIN
	#pragma warning(push, 0)
	#include <mpirxx.h>
	#pragma warning(pop)
#else
	#include <gmpxx.h>
#endif

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

    std::shared_ptr<CachePol> pCache( new CachePol );
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

    std::shared_ptr<CachePol> pCache( new CachePol );
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

    std::shared_ptr<CachePol> pCache( new CachePol );
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
    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("x*y*z");
    Pol f1 = sp.parseMultivariatePolynomial<Rational>("-1*x + 3*y");
    Pol f2 = sp.parseMultivariatePolynomial<Rational>("x + -1*x^2 + 3*x^3");
    Pol f3 = f1*f1*f2;
    Pol f4 = f1*f2*f2;
    Pol f5 = sp.parseMultivariatePolynomial<Rational>("-1*x + 1");
    Pol f6 = sp.parseMultivariatePolynomial<Rational>("x");
    Pol f7 = f6 - c1;

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fc1( (Rational) 1 );
    FPol fc2( (Rational) 42 );
    FPol fc3( (Rational) -2 );
    FPol fc4( (Rational) 0 );
    FPol fpx( fx, pCache );
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
    std::cout << "Common divisor of " << fpA << " and " << fpB;
    FPol fpCD = commonDivisor( fpA, fpB );
    std::cout << ": " << fpCD << std::endl;
    std::cout << "Common divisor of " << fpA << " and " << fc1;
    FPol fpAc1 = commonDivisor( fpA, fc1 );
    std::cout << ": " << fpAc1 << std::endl;
    EXPECT_EQ( fc1, fpAc1 );
    std::cout << "Common divisor of " << fc2 << " and " << fpB;
    FPol fpc2B = commonDivisor( fc2, fpB );
    std::cout << ": " << fpc2B << std::endl;
    EXPECT_EQ( fc1, fpc2B );

    //GCD
    FPol fpRestA0;
    FPol fpRestB0;
    std::cout << "GCD of " << fp8 << " and " << fp9 << ": ";
    FPol fpGCD0 = gcd( fp8, fp9, fpRestA0, fpRestB0 );
    std::cout << fpGCD0 << " with rest " << fpRestA0 << " and " << fpRestB0 << std::endl;
    EXPECT_EQ( fp8, fpRestA0 * fpGCD0 );
    EXPECT_EQ( fp9, fpRestB0 * fpGCD0 );
    EXPECT_EQ( computePolynomial(fp8), computePolynomial(fpRestA0 * fpGCD0) );
    EXPECT_EQ( computePolynomial(fp9), computePolynomial(fpRestB0 * fpGCD0) );

    FPol fpRestA;
    FPol fpRestB;
    std::cout << "GCD of " << fpA << " and " << fpB << ": ";
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );
    std::cout << fpGCD << " with rest " << fpRestA << " and " << fpRestB << std::endl;
    EXPECT_EQ( fpA, fpRestA * fpGCD );
    EXPECT_EQ( fpB, fpRestB * fpGCD );

    FPol fpRest3;
    FPol fpRest4;
    std::cout << "GCD of " << fp3 << " and " << fp4 << ": ";
    FPol fpGCDB = gcd( fp3, fp4, fpRest3, fpRest4 );
    std::cout << fpGCDB << " with rest " << fpRest3 << " and " << fpRest4 << std::endl;
    EXPECT_EQ( fp3, fpRest3 * fpGCDB );
    EXPECT_EQ( fp4, fpRest4 * fpGCDB );

    //Common Multiple
    std::cout << "Common multiple of " << fpA << " and " << fpB << ": ";
    FPol fpCM = commonMultiple( fpA, fpB );
    std::cout << fpCM  << std::endl;
    std::cout << "Common multiple of " << fpA << " and " << fc3 << ": ";
    FPol fpAc3 = commonMultiple( fpA, fc3 );
    std::cout << fpAc3  << std::endl;
    std::cout << "Common multiple of " << fc2 << " and " << fpB << ": ";
    FPol fpc3B = commonMultiple( fc2, fpB );
    std::cout << fpc3B  << std::endl;

    //Quotient
    std::cout<< "Quotient: " << fpB << " / " << fpA << ": ";
    FPol fpQuo = quotient( fpB, fpA );
    std::cout << fpQuo << std::endl;
    std::cout<< "Quotient: " << fpB << " / " << fc3 << ": ";
    FPol fpBc3 = quotient( fpB, fc3 );
    std::cout << fpBc3 << std::endl;
    std::cout<< "Quotient: " << fc4 << " / " << fpA << ": ";
    FPol fpc4A = quotient( fc4, fpA );
    std::cout << fpc4A << std::endl;
    std::cout<< "Quotient: " << fpA << " / " << fc1 << ": ";
    FPol fpAc1b = quotient( fpA, fc1 );
    std::cout << fpAc1b << std::endl;
    EXPECT_EQ( fpAc1b, fpA );

    //LCM
    Pol fLCM = lcm( fA, fB );
    FPol fpLCM = lcm( fpA, fpB );
    std::cout<< "LCM of " << fpA << " and " << fpB << ": " << fpLCM << std::endl;
    EXPECT_EQ( fLCM, computePolynomial( fpLCM ) );

    //Multiplication
    Pol fMul = fA * fB;
    std::cout<< fpA << " * " << fpB << ": ";
    FPol fpMul = fpA * fpB;
    std::cout << fpMul << std::endl;
    EXPECT_EQ( fMul, computePolynomial( fpMul ) );
    Pol fMul2 = fA * c2;
    std::cout << fpA << " * " << fc2 << ": ";
    FPol fpMul2 = fpA * fc2;
    std::cout << fpMul2 << std::endl;
    EXPECT_EQ( fMul2, computePolynomial( fpMul2 ) );
    Pol fMul3 = c4 * fB;
    std::cout << fc4 << " * " << fpB << ": ";
    FPol fpMul3 = fc4 * fpB;
    std::cout << fpMul3 << std::endl;
    EXPECT_EQ( fMul3, computePolynomial( fpMul3 ) );
    std::cout << "fMul4 = " << c2 << " * " << c3 << " = ";
    Pol fMul4 = c2 * c3;
    std::cout << fMul4 << std::endl;
    std::cout << "fpMul4 = " << fc2 << " * " << fc3 << " = ";
    FPol fpMul4 = fc2 * fc3;
    std::cout << fpMul4 << std::endl;
    EXPECT_EQ( fMul4, computePolynomial( fpMul4 ) );

    //Some test
    FPol fres = fp5;
    FPol ftest = fp7;
    ftest *= fp7;
    ftest *= fp7;
    ftest *= fp7;
    ftest *= fp7;
    ftest *= fp2;
    std::cout << "fp5 = " << fp5 << std::endl;
    std::cout << "fp7 = " << fp7 << std::endl;
    std::cout << "ftest = " << ftest << std::endl;
    fres *= ftest;
    std::cout << "fres = " << fres << std::endl;
    EXPECT_EQ( (f5*f7.pow(5)*f2), computePolynomial( fres ) );

    //Addition
    Pol fAdd = fA + fB;
    FPol fpAdd = fpA + fpB;
    std::cout<< fpA << " + " << fpB << ": " << fpAdd << std::endl;
    EXPECT_EQ( fAdd, computePolynomial( fpAdd ) );

    //Subtraction
    Pol fSub = fA - fB;
    FPol fpSub = fpA - fpB;
    std::cout<< fpA << " - " << fpB << ": " << fpSub << std::endl;
    EXPECT_EQ( fSub, computePolynomial( fpSub ) );

    //Unary minus
    std::cout<< "-(" << fpA << ") = " << (-fpA) << std::endl;
    std::cout << "-(" << fpB << ") = " << (-fpB) << std::endl;
}

TEST(FactorizedPolynomial, CommonDivisor)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("x*y*z");

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );

    std::cout << std::endl << "Common divisor of " << fpA << " and " << fpB << ": ";
    FPol fpC = commonDivisor( fpA, fpB );
    std::cout << fpC << std::endl << std::endl;
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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );
    FPol fpRestA;
    FPol fpRestB;
    FPol fg1( g1, pCache );
    FPol fg2( g2, pCache );

    Pol pGCD = gcd( pA, pB );
    Pol pRestA = pA.quotient( pGCD );
    Pol pRestB = pB.quotient( pGCD );
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );

    EXPECT_EQ( pGCD, computePolynomial( fpGCD ) );
    EXPECT_EQ( pRestA, computePolynomial( fpRestA ) );
    EXPECT_EQ( pRestB, computePolynomial( fpRestB ) );
    FPol fgRestA;
    FPol fgRestB;
    std::cout << "GCD of " << fg1 << " and " << fg2 << ": ";
    FPol fgGCD = gcd( fg1, fg2, fgRestA, fgRestB );
    std::cout << fgGCD << " with rest " << fgRestA << " and " << fgRestB << std::endl;
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
}

TEST(FactorizedPolynomial, Flattening)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("4*x*y*z");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol pC = sp.parseMultivariatePolynomial<Rational>("x");

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );
    FPol fpC( pC, pCache );
    FPol fpRestA;
    FPol fpRestB;
    FPol fpRestC;

    Pol pGCD = gcd( pA, pB );
    Pol pRestA = pA.quotient( pGCD );
    Pol pRestB = pB.quotient( pGCD );
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );

    EXPECT_EQ( pGCD, computePolynomial( fpGCD ) );
    EXPECT_EQ( pRestA, computePolynomial( fpRestA ) );
    EXPECT_EQ( pRestB, computePolynomial( fpRestB ) );

    Pol pGCD2 = gcd( pA, pC );
    pRestA = pA.quotient( pGCD2 );
    Pol pRestC = pC.quotient( pGCD2 );
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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( pA, pCache );
    FPol fpB = fpA * fpA;
    FPol fpC = fpB * fpB;
    std::cout << fpA << ", " << fpB << ", " << fpC << std::endl;
    fpC.factorization();
    std::cout << fpC << std::endl;
}

TEST(FactorizedPolynomial, LCM)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("x*y*z");

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );

    Pol pLCM = lcm( pA, pB );
    FPol fpLCM = lcm( fpA, fpB );
    EXPECT_EQ( pLCM, computePolynomial( fpLCM ) );

    carl::VariablePool::getInstance().clear();
    Variable y(1);
    Variable x(2);
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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = fp1 * fp2;
    FPol fp4 = fp3 * fp2;

    Pol pQuot = p4.quotient(p3);
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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = fp1 * fp2;
    FPol fp4 = fp3 * fp2;

    FPol* pfp5 = new FPol( fp1 * fp1 );
    std::cout << std::endl << *pfp5 << std::endl << std::endl;
    delete pfp5;

    Pol pQuot = p4.quotient(p3);
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

    std::shared_ptr<CachePol> pCache( new CachePol );
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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = fp1 * fp2;

    std::map<carl::Variable, Rational> substitution;
    substitution[x] = 3;

    Rational result = fp1.evaluate(substitution);
    EXPECT_EQ( Rational(18), result );
    result = fp2.evaluate(substitution);
    EXPECT_EQ( Rational(3), result );
    result = fp3.evaluate(substitution);
    EXPECT_EQ( Rational(54), result );
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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3 = (fp1*fp2) * (fp2 + fp1);

    FPol derivation = fp3.derivative(x, 1);

    Pol p3({(Rational)126*x*x});
    FPol fp4( p3, pCache );
    EXPECT_EQ( fp4, derivation );
}

