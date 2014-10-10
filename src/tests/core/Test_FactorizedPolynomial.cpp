#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/FactorizedPolynomial.h"
#include "carl/util/stringparser.h"

#include <cln/cln.h>


using namespace carl;

typedef cln::cl_RA Rational;
typedef MultivariatePolynomial<Rational> P;

TEST(FactorizedPolynomial, Construction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    P fxy({(cln::cl_RA)1*x*y});
    P fxyz({(cln::cl_RA)1*x*y*z});
    P f1({(cln::cl_RA)-1*x, (cln::cl_RA)3*y});
    P f2({(cln::cl_RA)1*x, (cln::cl_RA)-1*x*x, (cln::cl_RA)3*x*x*x});
    P f3 = f1*f1*f2;
    P f4 = f1*f2*f2;
    
    Cache<PolynomialFactorizationPair<P>> fpCache;
    fpCache.print();
    FactorizedPolynomial<P> fpA( fxy, &fpCache );
    fpCache.print();
    FactorizedPolynomial<P> fpB( fxyz, &fpCache );
    fpCache.print();
    FactorizedPolynomial<P> fpC( f3, &fpCache );
    fpCache.print();
    FactorizedPolynomial<P> fpD( f4, &fpCache );
    fpCache.print();

    //Common divisor
    FactorizedPolynomial<P> fpE = commonDivisor( fpA, fpB );
    std::cout << std::endl << "Common divisor of " << fpA << " and " << fpB << ": " << fpE << std::endl << std::endl;
    fpCache.print();
    
    //GCD
    FactorizedPolynomial<P> restA( P( 2 ), &fpCache );
    FactorizedPolynomial<P> restB( P( 2 ), &fpCache );
    std::cout << std::endl << "GCD of " << fpA << " and " << fpB << ": ";
    FactorizedPolynomial<P> fpGCD = gcd( fpA, fpB, restA, restB );
    std::cout << fpGCD << " with rest " << restA << " and " << restB << std::endl << std::endl;
    fpCache.print();
    
    FactorizedPolynomial<P> restC( P( 2 ), &fpCache );
    FactorizedPolynomial<P> restD( P( 2 ), &fpCache );
    std::cout << std::endl << "GCD of " << fpC << " and " << fpD << ": ";
    FactorizedPolynomial<P> fpGCDB = gcd( fpC, fpD, restC, restD );
    std::cout << fpGCDB << " with rest " << restC << " and " << restD << std::endl << std::endl;
    fpCache.print();

    //Common Multiple
    FactorizedPolynomial<P> fpCM = commonMultiple( fpA, fpB );
    std::cout << std::endl << "Common multiple of " << fpA << " and " << fpB << ": " << fpCM << std::endl << std::endl;
    fpCache.print();

    //Quotient
    FactorizedPolynomial<P> fpQuo = quotient( fpB, fpA );
    std::cout << std::endl << "Quotient: " << fpB << " / " << fpA << ": " << fpQuo << std::endl << std::endl;
    fpCache.print();

    //LCM
    FactorizedPolynomial<P> fpLCM = lcm( fpA, fpB );
    std::cout << std::endl << "LCM of " << fpA << " and " << fpB << ": " << fpLCM << std::endl << std::endl;
    fpCache.print();

    //Multiplication
    FactorizedPolynomial<P> fpMul = fpA * fpB;
    std::cout << std::endl << fpA << " * " << fpB << ": " << fpMul << std::endl << std::endl;
    fpCache.print();

    //Addition
    FactorizedPolynomial<P> fpAdd = fpA + fpB;
    std::cout << std::endl << fpA << " + " << fpB << ": " << fpAdd << std::endl << std::endl;
    fpCache.print();

    //Subtraction
    FactorizedPolynomial<P> fpSub = fpA - fpB;
    std::cout << std::endl << fpA << " - " << fpB << ": " << fpSub << std::endl << std::endl;
    fpCache.print();
}

TEST(FactorizedPolynomial, CommonDivisor)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});
    
    P fxy = sp.parseMultivariatePolynomial<Rational>("x*y");
    P fxyz = sp.parseMultivariatePolynomial<Rational>("x*y*z");
    
    Cache<PolynomialFactorizationPair<P>> fpCache;
    FactorizedPolynomial<P> fpA( fxy, &fpCache );
    FactorizedPolynomial<P> fpB( fxyz, &fpCache );
    
    std::cout << std::endl << "Common divisor of " << fpA << " and " << fpB << ": ";
    FactorizedPolynomial<P> fpC = commonDivisor( fpA, fpB );
    std::cout << fpC << std::endl << std::endl;
}

TEST(FactorizedPolynomial, GCD)
{
    StringParser sp;
    sp.setVariables({"x", "y", "z"});

    P fxy = sp.parseMultivariatePolynomial<Rational>("x*y");
    P fxyz = sp.parseMultivariatePolynomial<Rational>("x*y*z");

    Cache<PolynomialFactorizationPair<P>> fpCache;
    FactorizedPolynomial<P> fpA( fxy, &fpCache );
    FactorizedPolynomial<P> fpB( fxyz, &fpCache );
    FactorizedPolynomial<P> restA( P( 3 ), &fpCache );
    FactorizedPolynomial<P> restB( P( 3 ), &fpCache );
    std::cout << std::endl << "GCD of " << fpA << " and " << fpB << ": ";
    FactorizedPolynomial<P> fpGCD = gcd( fpA, fpB, restA, restB );
    std::cout << fpGCD << " with rest " << restA << " and " << restB << std::endl << std::endl;
    fpCache.print();
}