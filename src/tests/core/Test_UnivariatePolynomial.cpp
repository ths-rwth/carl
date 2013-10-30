
#include "gtest/gtest.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"

#include "carl/numbers/GFNumber.h"
#include "carl/numbers/GaloisField.h"

#include <cln/cln.h>
#include <gmpxx.h>
using namespace carl;

TEST(UnivariatePolynomial, Constructor)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    UnivariatePolynomial<cln::cl_RA> pol(x, {(cln::cl_RA)0, (cln::cl_RA)2});
 
}

TEST(UnivariatePolynomial, Reduction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)1, (cln::cl_RA)1,(cln::cl_RA)0, (cln::cl_RA)0,(cln::cl_RA)1});
    UnivariatePolynomial<cln::cl_RA> q(x, {(cln::cl_RA)1,(cln::cl_RA)1});
    //std::cout << p << "/" << q << "=" << p.reduce(q)<< std::endl;
    
}

TEST(UnivariatePolynomial, Divide)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)6, (cln::cl_RA)7,(cln::cl_RA)1});
    UnivariatePolynomial<cln::cl_RA> q(x, {(cln::cl_RA)-6, (cln::cl_RA)-5,(cln::cl_RA)1});
    DivisionResult<UnivariatePolynomial<cln::cl_RA>> d = p.divide(p);
//    std::cout << d.quotient << std::endl;
//    std::cout << d.remainder << std::endl;
}

TEST(UnivariatePolynomial, GCD)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)6, (cln::cl_RA)7,(cln::cl_RA)1});
    UnivariatePolynomial<cln::cl_RA> q(x, {(cln::cl_RA)-6, (cln::cl_RA)-5,(cln::cl_RA)1});
    //std::cout << "gcd" << UnivariatePolynomial<cln::cl_RA>::gcd(p,q) << std::endl;
    UnivariatePolynomial<cln::cl_RA> s(x);
    UnivariatePolynomial<cln::cl_RA> t(x);
    UnivariatePolynomial<cln::cl_RA> g = UnivariatePolynomial<cln::cl_RA>::extended_gcd(p,q,s,t);
    std::cout << g << std::endl;
    std::cout << s << std::endl;
    std::cout << t << std::endl;
    
    
}



TEST(UnivariatePolynomial, cauchyBounds)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)6, (cln::cl_RA)7,(cln::cl_RA)1});
    //p.cauchyBound();
    //p.modifiedCauchyBound();
}

TEST(UnivariatePolynomial, toFiniteDomain)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    
    UnivariatePolynomial<mpz_class> pol(x, {(mpz_class)1, (mpz_class)2});
    const GaloisField<mpz_class>* gf5 = new GaloisField<mpz_class>(5);
    UnivariatePolynomial<GFNumber<mpz_class>> polF = pol.toFiniteDomain(gf5);
    std::cout << polF << std::endl;
    UnivariatePolynomial<mpz_class> pol5(x, {(mpz_class)1, (mpz_class)5});
    UnivariatePolynomial<GFNumber<mpz_class>> pol5F = pol5.toFiniteDomain(gf5);
    std::cout << pol5F << std::endl;
}

TEST(UnivariatePolynomial, normalizeCoefficients)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setVariableName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setVariableName(z, "z");
    
	UnivariatePolynomial<mpz_class> pol(x, {(mpz_class)1, (mpz_class)2});
    const GaloisField<mpz_class>* gf5 = new GaloisField<mpz_class>(5);
    UnivariatePolynomial<GFNumber<mpz_class>> polF = pol.toFiniteDomain(gf5);
    
	pol.normalizeCoefficients();
	polF.normalizeCoefficients();
}