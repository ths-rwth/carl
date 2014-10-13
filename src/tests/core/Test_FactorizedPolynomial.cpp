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
    carl::VariablePool::getInstance().clear();
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    Pol fA({(cln::cl_RA)1*x*y});
    Pol fB({(cln::cl_RA)1*x*y*z});
    Pol f1({(cln::cl_RA)-1*x, (cln::cl_RA)3*y});
    Pol f2({(cln::cl_RA)1*x, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x*x*x});
    Pol f3 = f1*f1*f2;
    Pol f4 = f1*f2*f2;
    
    CachePol fpCache;
    FPol fpA( fA, &fpCache );
    FPol fpB( fB, &fpCache );
    FPol fp3( f3, &fpCache );
    FPol fp4( f4, &fpCache );

    //Common divisor
    FPol fpCD = commonDivisor( fpA, fpB );
    std::cout << "Common divisor of " << fpA << " and " << fpB << ": " << fpCD << std::endl;
    
    //GCD
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
    FPol fpCM = commonMultiple( fpA, fpB );
    std::cout << "Common multiple of " << fpA << " and " << fpB << ": " << fpCM  << std::endl;

    //Quotient
    FPol fpQuo = quotient( fpB, fpA );
    std::cout<< "Quotient: " << fpB << " / " << fpA << ": " << fpQuo << std::endl;

    //LCM
    Pol fLCM = lcm( fA, fB );
    FPol fpLCM = lcm( fpA, fpB );
    std::cout<< "LCM of " << fpA << " and " << fpB << ": " << fpLCM << std::endl;
    EXPECT_EQ( fLCM, computePolynomial( fpLCM ) );

    //Multiplication
    Pol fMul = fA * fB;
    FPol fpMul = fpA * fpB;
    std::cout<< fpA << " * " << fpB << ": " << fpMul << std::endl;
    EXPECT_EQ( fMul, computePolynomial( fpMul ) );

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