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
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol c1 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol c2 = sp.parseMultivariatePolynomial<Rational>("42");
    Pol c3 = sp.parseMultivariatePolynomial<Rational>("-2");
    Pol c4 = sp.parseMultivariatePolynomial<Rational>("0");
    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("x*y*z");
    Pol f1 = sp.parseMultivariatePolynomial<Rational>("-1*x + 3*y");
    Pol f2 = sp.parseMultivariatePolynomial<Rational>("x + -1*x^2 + 3*x^3");
    Pol f3 = f1*f1*f2;
    Pol f4 = f1*f2*f2;
    
    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fc1( (cln::cl_RA) 1 );
    FPol fc2( (cln::cl_RA) 42 );
    FPol fc3( (cln::cl_RA) -2 );
    FPol fc4( (cln::cl_RA) 0 );
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );
    FPol fp3( f3, pCache );
    FPol fp4( f4, pCache );

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
    Pol fMul4 = c2 * c3;
    std::cout << fc2 << " * " << fc3 << ": ";
    FPol fpMul4 = fc2 * fc3;
    std::cout << fpMul4 << std::endl;
    EXPECT_EQ( fMul4, computePolynomial( fpMul4 ) );

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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );
    FPol fpRestA;
    FPol fpRestB;

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

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( pA, pCache );
    FPol fpB( pB, pCache );

    Pol pLCM = lcm( pA, pB );
    FPol fpLCM = lcm( fpA, fpB );
    EXPECT_EQ( pLCM, computePolynomial( fpLCM ) );
}

TEST(FactorizedPolynomial, Subtraction)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    Pol c1 = sp.parseMultivariatePolynomial<Rational>("1");
    Pol fA = sp.parseMultivariatePolynomial<Rational>("x*y");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("z");

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fc1( c1, pCache );
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );

    Pol pSub = c1 - fA;
    FPol fpSub = fc1 - fpA;
    EXPECT_EQ( pSub, computePolynomial( fpSub ) );

    Pol pSub2 = fA - c1;
    FPol fpSub2 = fpA - fc1;
    EXPECT_EQ( pSub2, computePolynomial( fpSub2 ) );

    Pol pSub3 = fA - fB;
    FPol fpSub3 = fpA - fpB;
    EXPECT_EQ( pSub3, computePolynomial( fpSub3 ) );

    Pol pSub4 = fB - fA;
    FPol fpSub4 = fpB - fpA;
    EXPECT_EQ( pSub4, computePolynomial( fpSub4 ) );
}

TEST(FactorizedPolynomial, Addition)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x"});

    Pol fA = sp.parseMultivariatePolynomial<Rational>("x");
    Pol fB = sp.parseMultivariatePolynomial<Rational>("-1*x+1");

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fpA( fA, pCache );
    FPol fpB( fB, pCache );

    Pol pAdd = fA + fB;
    FPol fpAdd = fpA + fpB;
    EXPECT_EQ( pAdd, computePolynomial( fpAdd ) );
}

TEST(FactorizedPolynomial, Multiplication)
{
    carl::VariablePool::getInstance().clear();
    StringParser sp;
    sp.setVariables({"x", "y"});

    Pol p1 = sp.parseMultivariatePolynomial<Rational>("3*x*y + x");
    Pol p2 = sp.parseMultivariatePolynomial<Rational>("5*y");
    Pol p3 = sp.parseMultivariatePolynomial<Rational>("1*x");
    Pol p4 = sp.parseMultivariatePolynomial<Rational>("4*y");

    std::shared_ptr<CachePol> pCache( new CachePol );
    FPol fp1( p1, pCache );
    FPol fp2( p2, pCache );
    FPol fp3( p3, pCache );
    FPol fp4( p4, pCache );

    Pol pMul = p1 * p3;
    FPol fpMul = fp1 * fp3;
    EXPECT_EQ( pMul, computePolynomial( fpMul ) );

    Pol pMul2 = p2 * p4;
    FPol fpMul2 = fp2 * fp4;
    EXPECT_EQ( pMul2, computePolynomial( fpMul2 ) );
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