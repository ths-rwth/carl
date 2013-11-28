
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
	UnivariatePolynomial<cln::cl_RA> s(x);
    UnivariatePolynomial<cln::cl_RA> t(x);
    
	UnivariatePolynomial<cln::cl_RA> v(x, {(cln::cl_RA)1, (cln::cl_RA)1});
    UnivariatePolynomial<cln::cl_RA> p(x, {(cln::cl_RA)6, (cln::cl_RA)7,(cln::cl_RA)1});
	UnivariatePolynomial<cln::cl_RA> g = UnivariatePolynomial<cln::cl_RA>::extended_gcd(p,v,s,t);
	std::cout << g << std::endl;
    std::cout << s << std::endl;
    std::cout << t << std::endl;
	g = UnivariatePolynomial<cln::cl_RA>::extended_gcd(v,p,s,t);
	std::cout << g << std::endl;
    std::cout << s << std::endl;
    std::cout << t << std::endl;
	
	
    UnivariatePolynomial<cln::cl_RA> q(x, {(cln::cl_RA)-6, (cln::cl_RA)-5,(cln::cl_RA)1});
    //std::cout << "gcd" << UnivariatePolynomial<cln::cl_RA>::gcd(p,q) << std::endl;
    g = UnivariatePolynomial<cln::cl_RA>::extended_gcd(p,q,s,t);
    std::cout << g << std::endl;
    std::cout << s << std::endl;
    std::cout << t << std::endl;
	
    UnivariatePolynomial<mpz_class> A1(x, {(mpz_class)0, (mpz_class)2});
	const GaloisField<mpz_class>* gf5 = new GaloisField<mpz_class>(5);
    UnivariatePolynomial<GFNumber<mpz_class>> a1 = A1.toFiniteDomain(gf5);
	UnivariatePolynomial<mpz_class> A2(x, 1, 5);
	UnivariatePolynomial<GFNumber<mpz_class>> a2 = A2.toFiniteDomain(gf5);
	UnivariatePolynomial<GFNumber<mpz_class>> s1(x);
	UnivariatePolynomial<GFNumber<mpz_class>> t1(x);
    UnivariatePolynomial<GFNumber<mpz_class>> gp = UnivariatePolynomial<GFNumber<mpz_class>>::extended_gcd(a1,a2,s1,t1);
	std::cout << t1 << std::endl;
	std::cout << s1 << std::endl;
	std::cout << gp << std::endl;
    
    
}



TEST(UnivariatePolynomial, cauchyBounds)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");

    
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

TEST(UnivariatePolynomial, factorization)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setVariableName(x, "x");
    
	UnivariatePolynomial<cln::cl_RA> linA(x, {(cln::cl_RA)-2, (cln::cl_RA)5});
	UnivariatePolynomial<cln::cl_RA> linB(x, {(cln::cl_RA)1, (cln::cl_RA)3});
	UnivariatePolynomial<cln::cl_RA> linC(x, {(cln::cl_RA)2014, (cln::cl_RA)68});
	UnivariatePolynomial<cln::cl_RA> linD(x, {(cln::cl_RA)-13, (cln::cl_RA)4});
	UnivariatePolynomial<cln::cl_RA> linE(x, {(cln::cl_RA)-20, (cln::cl_RA)5});
	UnivariatePolynomial<cln::cl_RA> quaA(x, {(cln::cl_RA)-2, (cln::cl_RA)0, (cln::cl_RA)1});
	UnivariatePolynomial<cln::cl_RA> quaB(x, {(cln::cl_RA)3, (cln::cl_RA)6, (cln::cl_RA)9});
    
	UnivariatePolynomial<cln::cl_RA> polA(x, {(cln::cl_RA)-2, (cln::cl_RA)5, (cln::cl_RA)-5, (cln::cl_RA)3});
    UnivariatePolynomial<cln::cl_RA> polB = linA*linA;
    UnivariatePolynomial<cln::cl_RA> polC = linA*linA*linA;
    UnivariatePolynomial<cln::cl_RA> polD = linA*linB*linC*linD*linE;
    UnivariatePolynomial<cln::cl_RA> polE = quaA*linC*linD*linE;
    UnivariatePolynomial<cln::cl_RA> polF = linA*quaB*linE;
    UnivariatePolynomial<cln::cl_RA> polG = quaA*quaB*linD*linE;
    UnivariatePolynomial<cln::cl_RA> polH = polA*quaB*linD;
    UnivariatePolynomial<cln::cl_RA> polI = linA*linA*quaA*quaA*quaB*quaB*quaB*quaB;
    
    std::vector<UnivariatePolynomial<cln::cl_RA>> polys = {linA, linC, linE, quaA, quaB, polA, polB, polC, polD, polE, polF, polG, polH, polI};
    
    for(UnivariatePolynomial<cln::cl_RA> pol : polys)
    {
        std::map<UnivariatePolynomial<cln::cl_RA>, unsigned> factors = pol.factorization();
        UnivariatePolynomial<cln::cl_RA> productOfFactors = UnivariatePolynomial<cln::cl_RA>(x, (cln::cl_RA)1);
        std::cout << "Factorization of  " << pol << "  is  ";
        for(auto factor = factors.begin(); factor != factors.end(); ++factor)
        {
            if(factor != factors.begin())
                std::cout << " * ";
            std::cout << "(" << factor->first << ")^" << factor->second;
            EXPECT_NE(0, factor->second);
            for(unsigned i=0; i < factor->second; ++i)
            {
                productOfFactors *= factor->first;
            }
        }
        std::cout << std::endl;
        EXPECT_EQ(pol, productOfFactors);
    }
    
    UnivariatePolynomial<cln::cl_RA> pol2(x, {(cln::cl_RA)-1, (cln::cl_RA)2, (cln::cl_RA)-6, (cln::cl_RA)2});
    EXPECT_EQ(5, pol2.syntheticDivision((cln::cl_RA)3) );
    
    UnivariatePolynomial<cln::cl_RA> pol3(x, {(cln::cl_RA)-42, (cln::cl_RA)0, (cln::cl_RA)-12, (cln::cl_RA)1});
    EXPECT_EQ(-123, pol3.syntheticDivision((cln::cl_RA)3) );
}