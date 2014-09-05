#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/FactorizedPolynomialCache.h"
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
    
    FactorizedPolynomialCache<P> fpCache;
    FactorizedPolynomial<P> fpA( fxy, fpCache );
    FactorizedPolynomial<P> fpB( fxyz, fpCache );
    
    FactorizedPolynomial<P> fpC( commonDivisor( fpA, fpB ) );
    
    std::cout << fpA << std::endl;
}