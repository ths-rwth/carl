#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/FactorizedPolynomial.h"

#include <cln/cln.h>


using namespace carl;

TEST(FactorizedPolynomial, Construction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    typedef MultivariatePolynomial<cln::cl_RA> P;
    P fxy({(cln::cl_RA)1*x*y});
    P fxyz({(cln::cl_RA)1*x*y*z});
    
    Cache<PolynomialFactorizationPair<P>> fpCache;
    fpCache.print();
    FactorizedPolynomial<P> fpA( fxy, fpCache );
    fpCache.print();
    FactorizedPolynomial<P> fpB( fxyz, fpCache );
    fpCache.print();

    //Common divisor
    FactorizedPolynomial<P> fpC = commonDivisor( fpA, fpB );
    std::cout << std::endl << "Common divisor of " << fpA << " and " << fpB << ": " << fpC << std::endl << std::endl;
    fpCache.print();
    
    //GCD
    FactorizedPolynomial<P> restA( P( 2 ), fpCache );
    FactorizedPolynomial<P> restB( P( 2 ), fpCache );
    FactorizedPolynomial<P> fpGCD = gcd( fpA, fpB, restA, restB );
    std::cout << std::endl << "GCD of " << fpA << " and " << fpB << ": " << fpGCD << " with rest " << restA << " and " << restB << std::endl << std::endl;
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