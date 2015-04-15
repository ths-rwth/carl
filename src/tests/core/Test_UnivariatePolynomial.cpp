#include "gtest/gtest.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"

#include "carl/numbers/GFNumber.h"
#include "carl/numbers/GaloisField.h"
#include "carl/util/debug.h"
#include "carl/interval/Interval.h"
#include "carl/util/platform.h"

#include <random>
#include <cmath>
using namespace carl;

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#elif defined(__WIN)
#include <mpirxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

TEST(UnivariatePolynomial, Constructor)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    UnivariatePolynomial<Rational> pol(x, {(Rational)0, (Rational)2});
 
}

TEST(UnivariatePolynomial, toInteger)
{
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = vpool.getFreshVariable();
	vpool.setName(x, "x");
	UnivariatePolynomial<Rational> pRA(x, {(Rational)0, (Rational)2});
	UnivariatePolynomial<Integer> pI(x, {(Integer)0, (Integer)2});
	EXPECT_EQ(pI, pRA.toIntegerDomain());
}

TEST(UnivariatePolynomial, Reduction)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");

	{
		UnivariatePolynomial<Rational> p(x, {(Rational)1, (Rational)1,(Rational)0, (Rational)0,(Rational)1});
		UnivariatePolynomial<Rational> q(x, {(Rational)1,(Rational)1});
		auto res = p.remainder(q);
		ASSERT_EQ(res.degree(), (unsigned)0);
		ASSERT_EQ(res.lcoeff(), 1);
	}
	{
		UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {MultivariatePolynomial<Rational>(1), MultivariatePolynomial<Rational>(1),MultivariatePolynomial<Rational>(0), MultivariatePolynomial<Rational>(0), MultivariatePolynomial<Rational>(1)});
		UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {MultivariatePolynomial<Rational>(1), MultivariatePolynomial<Rational>(1)});
		auto res = p.remainder(q, MultivariatePolynomial<Rational>(1));
		ASSERT_EQ(res.degree(), (unsigned)0);
		ASSERT_EQ(res.lcoeff(), MultivariatePolynomial<Rational>(1));
	}
}

TEST(UnivariatePolynomial, Divide)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    UnivariatePolynomial<Rational> p(x, {(Rational)6, (Rational)7,(Rational)1});
    UnivariatePolynomial<Rational> q(x, {(Rational)-6, (Rational)-5,(Rational)1});
    DivisionResult<UnivariatePolynomial<Rational>> d = p.divideBy(p);
//    std::cout << d.quotient << std::endl;
//    std::cout << d.remainder << std::endl;
}

TEST(UnivariatePolynomial, DivideInteger)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    UnivariatePolynomial<Integer> p(x, {(Integer)0, (Integer)0,(Integer)5});
    UnivariatePolynomial<Integer> q(x, {(Integer)0, (Integer)0,(Integer)3});
    DivisionResult<UnivariatePolynomial<Integer>> d = p.divideBy(q);
    std::cout << d.quotient << std::endl;
    std::cout << d.remainder << std::endl;
}

TEST(UnivariatePolynomial, GCD)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
	UnivariatePolynomial<Rational> s(x);
    UnivariatePolynomial<Rational> t(x);
    
	UnivariatePolynomial<Rational> v(x, {(Rational)1, (Rational)1});
    UnivariatePolynomial<Rational> p(x, {(Rational)6, (Rational)7,(Rational)1});
	UnivariatePolynomial<Rational> g = UnivariatePolynomial<Rational>::extended_gcd(p,v,s,t);
	EXPECT_EQ(v,g);
	EXPECT_EQ((Rational)0,s);
	EXPECT_EQ((Rational)1,t);
    g = UnivariatePolynomial<Rational>::extended_gcd(v,p,s,t);
	EXPECT_EQ(v,g);
	EXPECT_EQ((Rational)1,s);
	EXPECT_EQ((Rational)0,t);
	g = UnivariatePolynomial<Rational>::gcd(v,p);
	EXPECT_EQ(v,g);
	g = UnivariatePolynomial<Rational>::gcd(p,v);
	EXPECT_EQ(v,g);
	
    
    UnivariatePolynomial<Rational> q(x, {(Rational)-6, (Rational)-5,(Rational)1});
    //std::cout << "gcd" << UnivariatePolynomial<Rational>::gcd(p,q) << std::endl;
    g = UnivariatePolynomial<Rational>::extended_gcd(p,q,s,t);
    EXPECT_EQ(v,g);
	EXPECT_EQ((Rational)1/(Rational)12,s);
    EXPECT_EQ((Rational)-1/(Rational)12,t);
	g = UnivariatePolynomial<Rational>::gcd(p,q);
	EXPECT_EQ(v,g);
	
    UnivariatePolynomial<Integer> A1(x, {(Integer)0, (Integer)2});
	const GaloisField<Integer>* gf5 = new GaloisField<Integer>(5);
    UnivariatePolynomial<GFNumber<Integer>> a1 = A1.toFiniteDomain(gf5);
	UnivariatePolynomial<Integer> A2(x, 1, 5);
	UnivariatePolynomial<GFNumber<Integer>> a2 = A2.toFiniteDomain(gf5);
	UnivariatePolynomial<GFNumber<Integer>> s1(x);
	UnivariatePolynomial<GFNumber<Integer>> t1(x);
    UnivariatePolynomial<GFNumber<Integer>> gp = UnivariatePolynomial<GFNumber<Integer>>::extended_gcd(a1,a2,s1,t1);
	std::cout << t1 << std::endl;
	std::cout << s1 << std::endl;
	std::cout << gp << std::endl;
    
    UnivariatePolynomial<Rational> pola(x, {(Rational)-2, (Rational)5, (Rational)-5, (Rational)3});
	UnivariatePolynomial<Rational> polb(x, {(Rational)5, (Rational)-10, (Rational)9});
	g = UnivariatePolynomial<Rational>::extended_gcd(pola,polb,s,t);
	std::cout << g << std::endl;
	g = UnivariatePolynomial<Rational>::gcd(pola,polb);
	std::cout << g << std::endl;
}



TEST(UnivariatePolynomial, cauchyBounds)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");

    
    UnivariatePolynomial<Rational> p(x, {(Rational)6, (Rational)7,(Rational)1});
    //p.cauchyBound();
    //p.modifiedCauchyBound();
}

TEST(UnivariatePolynomial, toFiniteDomain)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    
    UnivariatePolynomial<Integer> pol(x, {(Integer)1, (Integer)2});
    const GaloisField<Integer>* gf5 = new GaloisField<Integer>(5);
    UnivariatePolynomial<GFNumber<Integer>> polF = pol.toFiniteDomain(gf5);
    std::cout << polF << std::endl;
    UnivariatePolynomial<Integer> pol5(x, {(Integer)1, (Integer)5});
    UnivariatePolynomial<GFNumber<Integer>> pol5F = pol5.toFiniteDomain(gf5);
    std::cout << pol5F << std::endl;
}

TEST(UnivariatePolynomial, normalizeCoefficients)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    Variable y = vpool.getFreshVariable();
    vpool.setName(y, "y");
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    
	UnivariatePolynomial<Integer> pol(x, {(Integer)1, (Integer)2});
    const GaloisField<Integer>* gf5 = new GaloisField<Integer>(5);
    UnivariatePolynomial<GFNumber<Integer>> polF = pol.toFiniteDomain(gf5);
    
	pol.normalizeCoefficients();
	polF.normalizeCoefficients();
}

TEST(UnivariatePolynomial, factorization)
{
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    vpool.setName(x, "x");
    
    UnivariatePolynomial<Rational> linA(x, {(Rational)-2, (Rational)5});
    UnivariatePolynomial<Rational> linB(x, {(Rational)1, (Rational)3});
    UnivariatePolynomial<Rational> linC(x, {(Rational)2014, (Rational)68});
    UnivariatePolynomial<Rational> linD(x, {(Rational)-13, (Rational)4});
    UnivariatePolynomial<Rational> linE(x, {(Rational)-20, (Rational)5});
    UnivariatePolynomial<Rational> quaA(x, {(Rational)-2, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> quaB(x, {(Rational)3, (Rational)6, (Rational)9});

    UnivariatePolynomial<Rational> polA(x, {(Rational)-2, (Rational)5, (Rational)-5, (Rational)3});
    UnivariatePolynomial<Rational> polB = linA*linA;
    UnivariatePolynomial<Rational> polC = linA*linA*linA;
    UnivariatePolynomial<Rational> polD = linA*linB*linC*linD*linE;
    UnivariatePolynomial<Rational> polE = quaA*linC*linD*linE;
    UnivariatePolynomial<Rational> polF = linA*quaB*linE;
    UnivariatePolynomial<Rational> polG = quaA*quaB*linD*linE;
    UnivariatePolynomial<Rational> polH = polA*quaB*linD;
    UnivariatePolynomial<Rational> polI = linA*linA*quaA*quaA*quaB*quaB*quaB*quaB;
    
    std::vector<UnivariatePolynomial<Rational>> polys = {linA, linC, linE, quaA, quaB, polA, polB, polC, polD, polE, polF, polG, polH, polI};
    
    unsigned seed = 21344325;
    std::default_random_engine generator (seed);
    for(int e = 2; e <= 10; ++e)
    {
        std::cauchy_distribution<double> distribution(0.0,3.0+10.0*std::pow(0.9,e));
        for(int j=0; j<10; ++j)
        {
            std::vector<Rational> coeffs;
            for(int i=0; i<=e; ++i)
            {
                double ran = distribution(generator);
                int coe = (ran > INT_MAX || ran < INT_MIN) ? 0 : (int) ran;
                coeffs.push_back((Rational) coe);
            }
            if(coeffs.back() == (Rational) 0)
            {
                coeffs.back() = (Rational) 1;
            }
            UnivariatePolynomial<Rational> randomPol(x, coeffs);
            polys.push_back(randomPol);
        }
    }
    
    for(UnivariatePolynomial<Rational> pol : polys)
    {
//        std::cout << "Factorization of  " << pol << "  is  " << std::endl;
        std::map<UnivariatePolynomial<Rational>, unsigned> factors = pol.factorization();
        UnivariatePolynomial<Rational> productOfFactors = UnivariatePolynomial<Rational>(x, (Rational)1);
        for(auto factor = factors.begin(); factor != factors.end(); ++factor)
        {
//            std::cout << "           ";
//            if(factor != factors.begin())
//                std::cout << "* ";
//            std::cout << "(" << factor->first << ")^" << factor->second << std::endl;
            EXPECT_NE((unsigned)0, factor->second);
            for(unsigned i=0; i < factor->second; ++i)
            {
                productOfFactors *= factor->first;
            }
        }
//        std::cout << std::endl;
        EXPECT_EQ(pol, productOfFactors);
    }
    
    UnivariatePolynomial<Rational> pol2(x, {(Rational)-1, (Rational)2, (Rational)-6, (Rational)2});
    EXPECT_EQ(5, pol2.syntheticDivision((Rational)3) );
    
    UnivariatePolynomial<Rational> pol3(x, {(Rational)-42, (Rational)0, (Rational)-12, (Rational)1});
    EXPECT_EQ(-123, pol3.syntheticDivision((Rational)3) );
    
    UnivariatePolynomial<Rational> pol4(x, {(Rational)1, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> pol5(x, {(Rational)1, (Rational)0, (Rational)-1});
    UnivariatePolynomial<Rational> pol6 = pol4*pol5*pol5*pol5;
    std::map<unsigned, UnivariatePolynomial<Rational>> sffactors = pol6.squareFreeFactorization();
//    std::cout << "Square free factorization of  " << pol6 << "  is  " << std::endl;
    UnivariatePolynomial<Rational> productOfFactors = UnivariatePolynomial<Rational>(x, (Rational)1);
    for(auto factor = sffactors.begin(); factor != sffactors.end(); ++factor)
    {
//        std::cout << "        ";
//        if(factor != sffactors.begin())
//            std::cout << "* ";
//        std::cout << "(" << factor->second << ")^" << factor->first << std::endl;
        EXPECT_NE((unsigned)0, factor->first);
        for(unsigned i=0; i < factor->first; ++i)
        {
            productOfFactors *= factor->second;
        }
    }
//    std::cout << std::endl;
    EXPECT_EQ(pol6, productOfFactors);
}

TEST(UnivariatePolynomial, isNumber)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	EXPECT_FALSE(UnivariatePolynomial<Rational>(x, {1,2,3}).isNumber());
	EXPECT_TRUE(UnivariatePolynomial<Rational>(x, 1).isNumber());
}

TEST(UnivariatePolynomial, isUnivariate)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	//Variable y = VariablePool::getInstance().getFreshVariable();
	EXPECT_TRUE(UnivariatePolynomial<Rational>(x, {1,2,3}).isUnivariate());
	//TODO: How does this work?
	//EXPECT_FALSE(UnivariatePolynomial<UnivariatePolynomial<int>>(x, UnivariatePolynomial<int>(y, {1,2,3})).isUnivariate());
}

TEST(UnivariatePolynomial, numericUnit)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {1,2,3}).numericUnit(), 1);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {1,2,-3}).numericUnit(), -1);
}

TEST(UnivariatePolynomial, numericContent)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {1,2,3}).numericContent(), 1);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {15,27,3}).numericContent(), 3);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {-1,-2,-3}).numericContent(), 1);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {-15,-27,-3}).numericContent(), 3);
}

TEST(UnivariatePolynomial, unitPart)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	EXPECT_EQ(1,UnivariatePolynomial<Integer>(x, {1,2,3}).unitPart());
	EXPECT_EQ(1,UnivariatePolynomial<Integer>(x, {15,27,3}).unitPart());
	EXPECT_EQ(-1,UnivariatePolynomial<Integer>(x, {-1,-2,-3}).unitPart());
	EXPECT_EQ(-1,UnivariatePolynomial<Integer>(x, {-15,-27,-3}).unitPart());
}

TEST(UnivariatePolynomial, content)
{
    Variable x = VariablePool::getInstance().getFreshVariable();
	EXPECT_EQ(1,UnivariatePolynomial<Integer>(x, {1,2,3}).normalized().content());
	EXPECT_EQ(3,UnivariatePolynomial<Integer>(x, {15,27,3}).normalized().content());
	EXPECT_EQ(1,UnivariatePolynomial<Integer>(x, {-1,-2,-3}).normalized().content());
	EXPECT_EQ(3,UnivariatePolynomial<Integer>(x, {-15,-27,-3}).normalized().content());
}


TEST(UnivariatePolynomial, primitivePart)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	EXPECT_EQ(UnivariatePolynomial<Integer>(x, {1,2,3}),UnivariatePolynomial<Integer>(x, {1,2,3}).normalized().primitivePart());
	EXPECT_EQ(UnivariatePolynomial<Integer>(x, {5,9,1}),UnivariatePolynomial<Integer>(x, {15,27,3}).normalized().primitivePart());
	EXPECT_EQ(UnivariatePolynomial<Integer>(x, {1,2,3}),UnivariatePolynomial<Integer>(x, {-1,-2,-3}).normalized().primitivePart());
	EXPECT_EQ(UnivariatePolynomial<Integer>(x, {5,9,1}),UnivariatePolynomial<Integer>(x, {-15,-27,-3}).normalized().primitivePart().normalized().primitivePart());
}

TEST(UnivariatePolynomial, switchVariable)
{
	Variable x = VariablePool::getInstance().getFreshVariable();
	Variable y = VariablePool::getInstance().getFreshVariable();
	UnivariatePolynomial<Integer> p(x, {1,2,3});
	auto q = p.switchVariable(y);
	std::cout << p << " -> " << q << std::endl;
}

TEST(UnivariatePolynomial, resultant)
{
	Variable x = VariablePool::getInstance().getFreshVariable("x");
	Variable y = VariablePool::getInstance().getFreshVariable("y");
	
	// p1 = x - y
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p1(x, {MultivariatePolynomial<Rational>(-Term<Rational>(y)), MultivariatePolynomial<Rational>(1)});
	// p2 = x^2 + y^2 - 1
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p2(x, {MultivariatePolynomial<Rational>({Term<Rational>(y)*y, Term<Rational>(-1)}), MultivariatePolynomial<Rational>(0), MultivariatePolynomial<Rational>(1)});
	// p3 = x^2 - 1
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p3(x, {MultivariatePolynomial<Rational>(-Term<Rational>(1)), MultivariatePolynomial<Rational>(0), MultivariatePolynomial<Rational>(1)});
	
	// r1 = 2*y^2 - 1
	UnivariatePolynomial<MultivariatePolynomial<Rational>> r1(x, MultivariatePolynomial<Rational>({Term<Rational>(y)*y*Rational(2), Term<Rational>(-1)}));
	// r2 = y^2 - 1
	UnivariatePolynomial<MultivariatePolynomial<Rational>> r2(x, MultivariatePolynomial<Rational>({Term<Rational>(y)*y, Term<Rational>(-1)}));
	// r3 = y^4
	UnivariatePolynomial<MultivariatePolynomial<Rational>> r3(x, MultivariatePolynomial<Rational>(Term<Rational>(y)*y*y*y));
	
	EXPECT_EQ(r1, p1.resultant(p2));
	EXPECT_EQ(r1, p2.resultant(p1));
	EXPECT_EQ(r2, p1.resultant(p3));
	EXPECT_EQ(r2, p3.resultant(p1));
	EXPECT_EQ(r3, p2.resultant(p3));
	EXPECT_EQ(r3, p3.resultant(p2));
}

TEST(UnivariatePolynomial, resultant2)
{
	Variable x = VariablePool::getInstance().getFreshVariable("x");
	Variable c = VariablePool::getInstance().getFreshVariable("c");
	Variable t = VariablePool::getInstance().getFreshVariable("t");
	
	MultivariatePolynomial<Rational> mc(c);
	MultivariatePolynomial<Rational> mt(t);
	
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {mt, mc, mc});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {(Rational)0, (Rational)0, (Rational)0, (Rational)0, (Rational)1});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> r(x, MultivariatePolynomial<Rational>(Term<Rational>(t)*t*t*t));
	
	//std::cout << "resultant[" << p << ", " << q << "] = " << p.resultant(q) << std::endl;
	
	EXPECT_EQ(r, p.resultant(q));
}

TEST(UnivariatePolynomial, resultant3)
{
	Variable a = VariablePool::getInstance().getFreshVariable("a");
	Variable t = VariablePool::getInstance().getFreshVariable("t");
	
	MultivariatePolynomial<Rational> ma(a);
	MultivariatePolynomial<Rational> mt(t);
	MultivariatePolynomial<Rational> one((Rational)1);
	
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(a, {mt.pow(10) + mt.pow(8), mt.pow(8) + mt.pow(6), mt.pow(4)});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(a, {mt.pow(4), mt.pow(2), mt.pow(2), one});
	//UnivariatePolynomial<MultivariatePolynomial<Rational>> r(b, MultivariatePolynomial<Rational>(Term<Rational>(t)*t*t*t));
	
	std::cout << "resultant[" << p << ", " << q << "] =\n\t" << p.resultant(q) << std::endl;
	
	//EXPECT_EQ(r, p.resultant(q));
}

TEST(UnivariatePolynomial, resultant4)
{
    Variable m = VariablePool::getInstance().getFreshVariable("m");
    Variable r = VariablePool::getInstance().getFreshVariable("r");

    MultivariatePolynomial<Rational> mr(r);
    MultivariatePolynomial<Rational> one((Rational)1);
    MultivariatePolynomial<Rational> zero((Rational)0);

    UnivariatePolynomial<MultivariatePolynomial<Rational>> p(m, {mr+one, zero, -one});
    UnivariatePolynomial<MultivariatePolynomial<Rational>> q(m, {31, 16, 2});
    UnivariatePolynomial<MultivariatePolynomial<Rational>> res(r, {833, -124, 4});

    EXPECT_EQ(res, p.resultant(q));
}

TEST(UnivariatePolynomial, intervalCoeffs)
{
	Variable a = VariablePool::getInstance().getFreshVariable("a");
	UnivariatePolynomial<carl::Interval<double>> p(a);
	p *= p;
	p += p;
}
