#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/FactorizedPolynomial.h"
#include "carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> Pol;
typedef FactorizedPolynomial<Pol> FPol;
typedef Cache<PolynomialFactorizationPair<Pol>> CachePol;

TEST(FactorizedPolynomial, Construction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    Pol fxy({(cln::cl_RA)1*x*y});
    Pol fxyz({(cln::cl_RA)1*x*y*z});
    Pol f1({(cln::cl_RA)-1*x, (cln::cl_RA)3*y});
    Pol f2({(cln::cl_RA)1*x, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x*x*x});
    Pol f3 = f1*f1*f2;
    Pol f4 = f1*f2*f2;
    
    CachePol fpCache;
    fpCache.print();
    FPol fpA( fxy, &fpCache );
    fpCache.print();
    FPol fpB( fxyz, &fpCache );
    fpCache.print();
    FPol fpC( f3, &fpCache );
    fpCache.print();
    FPol fpD( f4, &fpCache );
    fpCache.print();

    //Common divisor
    FPol fpE = commonDivisor( fpA, fpB );
    std::cout << std::endl << "Common divisor of " << fpA << " and " << fpB << ": " << fpE << std::endl << std::endl;
    fpCache.print();
    
    //GCD
    FPol restA( Pol( 2 ), &fpCache );
    FPol restB( Pol( 2 ), &fpCache );
    std::cout << std::endl << "GCD of " << fpA << " and " << fpB << ": ";
    FPol fpGCD = gcd( fpA, fpB, restA, restB );
    std::cout << fpGCD << " with rest " << restA << " and " << restB << std::endl << std::endl;
    fpCache.print();
    
    FPol restC( Pol( 2 ), &fpCache );
    FPol restD( Pol( 2 ), &fpCache );
    std::cout << std::endl << "GCD of " << fpC << " and " << fpD << ": ";
    FPol fpGCDB = gcd( fpC, fpD, restC, restD );
    std::cout << fpGCDB << " with rest " << restC << " and " << restD << std::endl << std::endl;
    fpCache.print();

    //Common Multiple
    FPol fpCM = commonMultiple( fpA, fpB );
    std::cout << std::endl << "Common multiple of " << fpA << " and " << fpB << ": " << fpCM << std::endl << std::endl;
    fpCache.print();

    //Quotient
    FPol fpQuo = quotient( fpB, fpA );
    std::cout << std::endl << "Quotient: " << fpB << " / " << fpA << ": " << fpQuo << std::endl << std::endl;
    fpCache.print();

    //LCM
    FPol fpLCM = lcm( fpA, fpB );
    std::cout << std::endl << "LCM of " << fpA << " and " << fpB << ": " << fpLCM << std::endl << std::endl;
    fpCache.print();

    //Multiplication
    FPol fpMul = fpA * fpB;
    std::cout << std::endl << fpA << " * " << fpB << ": " << fpMul << std::endl << std::endl;
    fpCache.print();

    //Addition
    FPol fpAdd = fpA + fpB;
    std::cout << std::endl << fpA << " + " << fpB << ": " << fpAdd << std::endl << std::endl;
    fpCache.print();

    //Subtraction
    FPol fpSub = fpA - fpB;
    std::cout << std::endl << fpA << " - " << fpB << ": " << fpSub << std::endl << std::endl;
    fpCache.print();
    
    //Unary minus
    std::cout << std::endl << "-(" << fpA << ") = " << (-fpA) << std::endl << std::endl;
    std::cout << std::endl << "-(" << fpB << ") = " << (-fpB) << std::endl << std::endl;
    fpCache.print();
}

TEST(FactorizedPolynomial, CommonDivisor)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("x*y*z");
    
    CachePol fpCache;
    FPol fpA( fA, &fpCache );
    FPol fpB( fB, &fpCache );
    
    std::cout << std::endl << "Common divisor of " << fpA << " and " << fpB << ": ";
    FPol fpC = commonDivisor( fpA, fpB );
    std::cout << fpC << std::endl << std::endl;
}

TEST(FactorizedPolynomial, GCD)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("x*y*z");

    CachePol fpCache;
    FPol fpA( pA, &fpCache );
    FPol fpB( pB, &fpCache );
    FPol fpRestA( Pol( 3 ), &fpCache );
    FPol fpRestB( Pol( 3 ), &fpCache );

    Pol pGCD = gcd( pA, pB );
    Pol pRestA = pA.quotient( pGCD );
    Pol pRestB = pB.quotient( pGCD );
    FPol fpGCD = gcd( fpA, fpB, fpRestA, fpRestB );

    EXPECT_EQ( pGCD, computePolynomial( fpGCD ) );
    EXPECT_EQ( pRestA, computePolynomial( fpRestA ) );
    EXPECT_EQ( pRestB, computePolynomial( fpRestB ) );
}

TEST(FactorizedPolynomial, LCM)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol pA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol pB = sp.parseMultivariatePolynomial<Rational>("x*y*z");

    CachePol fpCache;
    FPol fpA( pA, &fpCache );
    FPol fpB( pB, &fpCache );

    Pol pLCM = lcm( pA, pB );
    FPol fpLCM = lcm( fpA, fpB );
    EXPECT_EQ( pLCM, computePolynomial( fpLCM ) );
}