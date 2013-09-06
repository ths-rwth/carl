
#include "gtest/gtest.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"

#include <cln/cln.h>
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
    std::cout << pol << std::endl;
}

TEST(UnivariatePolynomial, Reduction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)1, (cln::cl_RA)1,(cln::cl_RA)0, (cln::cl_RA)0,(cln::cl_RA)1});
    UnivariatePolynomial<cln::cl_RA> q(x, {(cln::cl_RA)1,(cln::cl_RA)1});
    std::cout << p << "/" << q << "=" << p.reduce(q)<< std::endl;
    
}

TEST(UnivariatePolynomial, GCD)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)6, (cln::cl_RA)7,(cln::cl_RA)1});
    UnivariatePolynomial<cln::cl_RA> q(x, {(cln::cl_RA)-6, (cln::cl_RA)-5,(cln::cl_RA)1});
    std::cout << "gcd" << UnivariatePolynomial<cln::cl_RA>::gcd(p,q) << std::endl;
    
    
}



TEST(UnivariatePolynomial, cauchyBounds)
{
    EXPECT_TRUE(false);
}
