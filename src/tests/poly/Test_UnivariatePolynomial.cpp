#include "gtest/gtest.h"
#include <carl-arith/interval/Interval.h>
#include <carl-arith/poly/umvpoly/functions/Content.h>
#include <carl-arith/poly/umvpoly/functions/Division.h>
#include <carl-arith/poly/umvpoly/functions/GCD.h>
#include <carl-arith/poly/umvpoly/functions/PrimitivePart.h>
#include <carl-arith/poly/umvpoly/functions/Resultant.h>
#include <carl-arith/poly/umvpoly/functions/Factorization_univariate.h>
#include <carl-arith/poly/umvpoly/functions/Derivative.h>
#include <carl-arith/poly/umvpoly/functions/Representation.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/core/VariablePool.h>

#include <carl-arith/numbers/GFNumber.h>
#include <carl-arith/numbers/GaloisField.h>
#include <carl-common/debug/debug.h>
#include <carl-common/meta/platform.h>

#include <random>
#include <cmath>

#include "../Common.h"
using namespace carl;

template<typename T>
class UnivariatePolynomialTest: public testing::Test {};
TYPED_TEST_SUITE(UnivariatePolynomialTest, NumberTypes);

template<typename T>
class UnivariatePolynomialRatTest: public testing::Test {};
TYPED_TEST_SUITE(UnivariatePolynomialRatTest, RationalTypes);

template<typename T>
class UnivariatePolynomialIntTest: public testing::Test {};
TYPED_TEST_SUITE(UnivariatePolynomialIntTest, IntegerTypes);

TEST(UnivariatePolynomial, Constructor)
{
    Variable x = fresh_real_variable("x");
    UnivariatePolynomial<Rational> pol(x, {(Rational)0, (Rational)2});
}

TEST(UnivariatePolynomial, toInteger)
{
	Variable x = fresh_real_variable("x");
	UnivariatePolynomial<Rational> pRA(x, {(Rational)0, (Rational)2});
	UnivariatePolynomial<IntegralType<Rational>::type> pI(x, {(IntegralType<Rational>::type)0, (IntegralType<Rational>::type)2});
	EXPECT_EQ(pI, pRA.to_integer_domain());
}

TEST(UnivariatePolynomial, Reduction)
{
    Variable x = fresh_real_variable("x");

	{
		UnivariatePolynomial<Rational> p(x, {(Rational)1, (Rational)1,(Rational)0, (Rational)0,(Rational)1});
		UnivariatePolynomial<Rational> q(x, {(Rational)1,(Rational)1});
		auto res = carl::remainder(p, q);
		ASSERT_EQ(res.degree(), (unsigned)0);
		ASSERT_EQ(res.lcoeff(), 1);
	}
	{
		UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {MultivariatePolynomial<Rational>(1), MultivariatePolynomial<Rational>(1),MultivariatePolynomial<Rational>(0), MultivariatePolynomial<Rational>(0), MultivariatePolynomial<Rational>(1)});
		UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {MultivariatePolynomial<Rational>(1), MultivariatePolynomial<Rational>(1)});
		auto res = carl::remainder(p, q, MultivariatePolynomial<Rational>(1));
		ASSERT_EQ(res.degree(), (unsigned)0);
		ASSERT_EQ(res.lcoeff(), MultivariatePolynomial<Rational>(1));
	}
}

TEST(UnivariatePolynomial, Divide)
{
    Variable x = fresh_real_variable("x");
    UnivariatePolynomial<Rational> p(x, {(Rational)6, (Rational)7,(Rational)1});
    UnivariatePolynomial<Rational> q(x, {(Rational)-6, (Rational)-5,(Rational)1});
    DivisionResult<UnivariatePolynomial<Rational>> d = carl::divide(p, p);
//    std::cout << d.quotient << std::endl;
//    std::cout << d.remainder << std::endl;
}

TYPED_TEST(UnivariatePolynomialIntTest, DivideInteger)
{
    Variable x = fresh_real_variable("x");
    UnivariatePolynomial<TypeParam> p(x, {(TypeParam)0, (TypeParam)0,(TypeParam)5});
    UnivariatePolynomial<TypeParam> q(x, {(TypeParam)0, (TypeParam)0,(TypeParam)3});
    //DivisionResult<UnivariatePolynomial<TypeParam>> d = p.divideBy(q);
    //std::cout << d.quotient << std::endl;
    //std::cout << d.remainder << std::endl;
}

TEST(UnivariatePolynomial, GCD)
{
    Variable x = fresh_real_variable("x");
	UnivariatePolynomial<Rational> s(x);
    UnivariatePolynomial<Rational> t(x);

	UnivariatePolynomial<Rational> v(x, {(Rational)1, (Rational)1});
    UnivariatePolynomial<Rational> p(x, {(Rational)6, (Rational)7,(Rational)1});
	UnivariatePolynomial<Rational> g = carl::extended_gcd(p,v,s,t);
	EXPECT_EQ(v,g);
	EXPECT_EQ((Rational)0,s);
	EXPECT_EQ((Rational)1,t);
    g = carl::extended_gcd(v,p,s,t);
	EXPECT_EQ(v,g);
	EXPECT_EQ((Rational)1,s);
	EXPECT_EQ((Rational)0,t);
	g = carl::gcd(v,p);
	EXPECT_EQ(v,g);
	g = carl::gcd(p,v);
	EXPECT_EQ(v,g);


    UnivariatePolynomial<Rational> q(x, {(Rational)-6, (Rational)-5,(Rational)1});
    //std::cout << "gcd" << UnivariatePolynomial<Rational>::gcd(p,q) << std::endl;
    g = carl::extended_gcd(p,q,s,t);
    EXPECT_EQ(v,g);
	EXPECT_EQ((Rational)1/(Rational)12,s);
    EXPECT_EQ((Rational)-1/(Rational)12,t);
	g = carl::gcd(p,q);
	EXPECT_EQ(v,g);
}

TYPED_TEST(UnivariatePolynomialIntTest, GCD)
{
    Variable x = fresh_real_variable("x");
	UnivariatePolynomial<Rational> s(x);
    UnivariatePolynomial<Rational> t(x);
	
    UnivariatePolynomial<TypeParam> A1(x, {(TypeParam)0, (TypeParam)2});
	auto gf5 = std::make_unique<GaloisField<TypeParam>>(5);
    UnivariatePolynomial<GFNumber<TypeParam>> a1 = A1.toFiniteDomain(gf5.get());
	UnivariatePolynomial<TypeParam> A2(x, 1, 5);
	UnivariatePolynomial<GFNumber<TypeParam>> a2 = A2.toFiniteDomain(gf5.get());
	UnivariatePolynomial<GFNumber<TypeParam>> s1(x);
	UnivariatePolynomial<GFNumber<TypeParam>> t1(x);
    UnivariatePolynomial<GFNumber<TypeParam>> gp = carl::extended_gcd(a1,a2,s1,t1);
	std::cout << t1 << std::endl;
	std::cout << s1 << std::endl;
	std::cout << gp << std::endl;

    UnivariatePolynomial<Rational> pola(x, {(Rational)-2, (Rational)5, (Rational)-5, (Rational)3});
	UnivariatePolynomial<Rational> polb(x, {(Rational)5, (Rational)-10, (Rational)9});
	UnivariatePolynomial<Rational> g = carl::extended_gcd(pola,polb,s,t);
	std::cout << g << std::endl;
	g = carl::gcd(pola,polb);
	std::cout << g << std::endl;
}



TEST(UnivariatePolynomial, cauchyBounds)
{
    Variable x = fresh_real_variable("x");

    UnivariatePolynomial<Rational> p(x, {(Rational)6, (Rational)7,(Rational)1});
    //p.cauchyBound();
    //p.modifiedCauchyBound();
}

TYPED_TEST(UnivariatePolynomialIntTest, toFiniteDomain)
{
    Variable x = fresh_real_variable("x");

    UnivariatePolynomial<TypeParam> pol(x, {(TypeParam)1, (TypeParam)2});
    auto gf5 = std::make_unique<GaloisField<TypeParam>>(5);
    UnivariatePolynomial<GFNumber<TypeParam>> polF = pol.toFiniteDomain(gf5.get());
    std::cout << polF << std::endl;
    UnivariatePolynomial<TypeParam> pol5(x, {(TypeParam)1, (TypeParam)5});
    UnivariatePolynomial<GFNumber<TypeParam>> pol5F = pol5.toFiniteDomain(gf5.get());
    std::cout << pol5F << std::endl;
}

TYPED_TEST(UnivariatePolynomialIntTest, normalizeCoefficients)
{
    Variable x = fresh_real_variable("x");

	UnivariatePolynomial<TypeParam> pol(x, {(TypeParam)1, (TypeParam)2});
    auto gf5 = std::make_unique<GaloisField<TypeParam>>(5);
    UnivariatePolynomial<GFNumber<TypeParam>> polF = pol.toFiniteDomain(gf5.get());

	pol.normalizeCoefficients();
	polF.normalizeCoefficients();
}

TEST(UnivariatePolynomial, factorization)
{
    Variable x = fresh_real_variable("x");

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
    for (int e = 2; e <= 10; ++e) {
        std::cauchy_distribution<double> distribution(0.0,3.0+10.0*std::pow(0.9,e));
        for (int j=0; j<10; ++j) {
            std::vector<Rational> coeffs;
            for(int i=0; i<=e; ++i) {
                double ran = distribution(generator);
                int coe = (ran > INT_MAX || ran < INT_MIN) ? 0 : (int) ran;
                coeffs.push_back((Rational) coe);
            }
            if (coeffs.back() == (Rational) 0) {
                coeffs.back() = (Rational) 1;
            }
            UnivariatePolynomial<Rational> randomPol(x, coeffs);
            polys.push_back(randomPol);
        }
    }

    for(const auto& pol : polys)
    {
        std::cout << "Factorizing " << pol << "..." << std::endl;
        const auto& factors = carl::factorization(pol);
        std::cout << "Factorization of " << pol << " = " << factors << std::endl;
        UnivariatePolynomial<Rational> productOfFactors = UnivariatePolynomial<Rational>(x, (Rational)1);
        for (const auto& factor: factors) {
            EXPECT_NE((unsigned)0, factor.second);
            for(unsigned i=0; i < factor.second; ++i) {
                productOfFactors *= factor.first;
            }
        }
        EXPECT_EQ(pol, productOfFactors);
    }
    UnivariatePolynomial<Rational> pol2(x, {(Rational)-1, (Rational)2, (Rational)-6, (Rational)2});
    std::cout << "synthetic_division of " << pol2 << std::endl;
    EXPECT_EQ(5, pol2.synthetic_division((Rational)3) );

    UnivariatePolynomial<Rational> pol3(x, {(Rational)-42, (Rational)0, (Rational)-12, (Rational)1});
    std::cout << "synthetic_division of " << pol3 << std::endl;
    EXPECT_EQ(-123, pol3.synthetic_division((Rational)3) );

    UnivariatePolynomial<Rational> pol4(x, {(Rational)1, (Rational)0, (Rational)1});
    UnivariatePolynomial<Rational> pol5(x, {(Rational)1, (Rational)0, (Rational)-1});
    UnivariatePolynomial<Rational> pol6 = pol4*pol5*pol5*pol5;
    std::cout << "Square free factorization of  " << pol6 << "  is  " << std::endl;
    auto sffactors = carl::squareFreeFactorization(pol6);
    UnivariatePolynomial<Rational> productOfFactors = UnivariatePolynomial<Rational>(x, (Rational)1);
    for(auto factor = sffactors.begin(); factor != sffactors.end(); ++factor)
    {
//        std::cout << "        ";
//        if(factor != sffactors.begin())
//            std::cout << "* ";
        std::cout << "(" << factor->second << ")^" << factor->first << std::endl;
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
	Variable x = fresh_real_variable("x");
	EXPECT_FALSE(UnivariatePolynomial<Rational>(x, {1,2,3}).is_number());
	EXPECT_TRUE(UnivariatePolynomial<Rational>(x, 1).is_number());
}

TEST(UnivariatePolynomial, isUnivariate)
{
	Variable x = fresh_real_variable("x");
	//Variable y = VariablePool::getInstance().fresh_real_variable();
	EXPECT_TRUE(UnivariatePolynomial<Rational>(x, {1,2,3}).is_univariate());
	//TODO: How does this work?
	//EXPECT_FALSE(UnivariatePolynomial<UnivariatePolynomial<int>>(x, UnivariatePolynomial<int>(y, {1,2,3})).is_univariate());
}

TEST(UnivariatePolynomial, numeric_unit)
{
	Variable x = fresh_real_variable("x");
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {1,2,3}).numeric_unit(), 1);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {1,2,-3}).numeric_unit(), -1);
}

TEST(UnivariatePolynomial, numericContent)
{
	Variable x = fresh_real_variable("x");
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {1,2,3}).numeric_content(), 1);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {15,27,3}).numeric_content(), 3);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {-1,-2,-3}).numeric_content(), 1);
	EXPECT_EQ(UnivariatePolynomial<Rational>(x, {-15,-27,-3}).numeric_content(), 3);
}

TYPED_TEST(UnivariatePolynomialIntTest, unit_part)
{
	Variable x = fresh_real_variable("x");
	EXPECT_EQ(1,UnivariatePolynomial<TypeParam>(x, {1,2,3}).unit_part());
	EXPECT_EQ(1,UnivariatePolynomial<TypeParam>(x, {15,27,3}).unit_part());
	EXPECT_EQ(-1,UnivariatePolynomial<TypeParam>(x, {-1,-2,-3}).unit_part());
	EXPECT_EQ(-1,UnivariatePolynomial<TypeParam>(x, {-15,-27,-3}).unit_part());
}

TYPED_TEST(UnivariatePolynomialIntTest, content)
{
    Variable x = fresh_real_variable("x");
	EXPECT_EQ(1,carl::content(UnivariatePolynomial<TypeParam>(x, {1,2,3}).normalized()));
	EXPECT_EQ(3,carl::content(UnivariatePolynomial<TypeParam>(x, {15,27,3}).normalized()));
	EXPECT_EQ(1,carl::content(UnivariatePolynomial<TypeParam>(x, {-1,-2,-3}).normalized()));
	EXPECT_EQ(3,carl::content(UnivariatePolynomial<TypeParam>(x, {-15,-27,-3}).normalized()));
}


TYPED_TEST(UnivariatePolynomialIntTest, primitivePart)
{
	Variable x = fresh_real_variable("x");
	EXPECT_EQ(UnivariatePolynomial<TypeParam>(x, {1,2,3}),carl::primitive_part(UnivariatePolynomial<TypeParam>(x, {1,2,3}).normalized()));
	EXPECT_EQ(UnivariatePolynomial<TypeParam>(x, {5,9,1}),carl::primitive_part(UnivariatePolynomial<TypeParam>(x, {15,27,3}).normalized()));
	EXPECT_EQ(UnivariatePolynomial<TypeParam>(x, {1,2,3}),carl::primitive_part(UnivariatePolynomial<TypeParam>(x, {-1,-2,-3}).normalized()));
	EXPECT_EQ(UnivariatePolynomial<TypeParam>(x, {5,9,1}),carl::primitive_part(UnivariatePolynomial<TypeParam>(x, {-15,-27,-3}).normalized()));
}

TYPED_TEST(UnivariatePolynomialIntTest, switchVariable)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");
	UnivariatePolynomial<TypeParam> p(x, {1,2,3});
	MultivariatePolynomial<TypeParam> mp = TypeParam(3)*x*x + TypeParam(2)*x + TypeParam(1);
	UnivariatePolynomial<MultivariatePolynomial<TypeParam>> res(y, {mp});
	auto q = carl::switch_main_variable(p, y);
	EXPECT_EQ(q, res);
}

TYPED_TEST(UnivariatePolynomialTest, derivative)
{
	Variable x = fresh_real_variable("x");

	{
		UnivariatePolynomial<TypeParam> p(x, {1,2,3,4,5,6,7});
		EXPECT_EQ(carl::derivative(p, 0), p);
		EXPECT_EQ(carl::derivative(p, 1), UnivariatePolynomial<TypeParam>(x, {2,6,12,20,30,42}));
		EXPECT_EQ(carl::derivative(p, 2), UnivariatePolynomial<TypeParam>(x, {6,24,60,120,210}));
		EXPECT_EQ(carl::derivative(p, 3), UnivariatePolynomial<TypeParam>(x, {24,120,360,840}));
		EXPECT_EQ(carl::derivative(p, 4), UnivariatePolynomial<TypeParam>(x, {120,720,2520}));
		EXPECT_EQ(carl::derivative(p, 5), UnivariatePolynomial<TypeParam>(x, {720, 5040}));
		EXPECT_EQ(carl::derivative(p, 6), UnivariatePolynomial<TypeParam>(x, {5040}));
		EXPECT_EQ(carl::derivative(p, 7), UnivariatePolynomial<TypeParam>(x));
		EXPECT_EQ(carl::derivative(p, 8), UnivariatePolynomial<TypeParam>(x));
	}
	{
		UnivariatePolynomial<TypeParam> p(x, {1,2,3,4,5,6,7});
		EXPECT_EQ(carl::derivative(p, x, 0), p);
		EXPECT_EQ(carl::derivative(p, x, 1), UnivariatePolynomial<TypeParam>(x, {2,6,12,20,30,42}));
		EXPECT_EQ(carl::derivative(p, x, 2), UnivariatePolynomial<TypeParam>(x, {6,24,60,120,210}));
		EXPECT_EQ(carl::derivative(p, x, 3), UnivariatePolynomial<TypeParam>(x, {24,120,360,840}));
		EXPECT_EQ(carl::derivative(p, x, 4), UnivariatePolynomial<TypeParam>(x, {120,720,2520}));
		EXPECT_EQ(carl::derivative(p, x, 5), UnivariatePolynomial<TypeParam>(x, {720, 5040}));
		EXPECT_EQ(carl::derivative(p, x, 6), UnivariatePolynomial<TypeParam>(x, {5040}));
		EXPECT_EQ(carl::derivative(p, x, 7), UnivariatePolynomial<TypeParam>(x));
		EXPECT_EQ(carl::derivative(p, x, 8), UnivariatePolynomial<TypeParam>(x));
	}
}

TEST(UnivariatePolynomial, resultant)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");

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

	EXPECT_EQ(r1, carl::resultant(p1, p2));
	EXPECT_EQ(r1, carl::resultant(p2, p1));
	EXPECT_EQ(r2, carl::resultant(p1, p3));
	EXPECT_EQ(r2, carl::resultant(p3, p1));
	EXPECT_EQ(r3, carl::resultant(p2, p3));
	EXPECT_EQ(r3, carl::resultant(p3, p2));
}

TEST(UnivariatePolynomial, resultant2)
{
	Variable x = fresh_real_variable("x");
	Variable c = fresh_real_variable("c");
	Variable t = fresh_real_variable("t");

	MultivariatePolynomial<Rational> mc(c);
	MultivariatePolynomial<Rational> mt(t);

	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {mt, mc, mc});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {(Rational)0, (Rational)0, (Rational)0, (Rational)0, (Rational)1});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> r(x, MultivariatePolynomial<Rational>(Term<Rational>(t)*t*t*t));

	//std::cout << "resultant[" << p << ", " << q << "] = " << p.resultant(q) << std::endl;

	EXPECT_EQ(r, carl::resultant(p, q));
}

TEST(UnivariatePolynomial, resultant3)
{
	Variable a = fresh_real_variable("a");
	Variable t = fresh_real_variable("t");

	MultivariatePolynomial<Rational> ma(a);
	MultivariatePolynomial<Rational> mt(t);
	MultivariatePolynomial<Rational> one((Rational)1);

	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(a, {carl::pow(mt, 10) + carl::pow(mt, 8), carl::pow(mt, 8) + carl::pow(mt, 6), carl::pow(mt, 4)});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(a, {carl::pow(mt, 4), carl::pow(mt, 2), carl::pow(mt, 2), one});
	//UnivariatePolynomial<MultivariatePolynomial<Rational>> r(b, MultivariatePolynomial<Rational>(Term<Rational>(t)*t*t*t));

	std::cout << "resultant[" << p << ", " << q << "] =\n\t" << carl::resultant(p, q) << std::endl;

	//EXPECT_EQ(r, p.resultant(q));
}

TEST(UnivariatePolynomial, resultant4)
{
    Variable m = fresh_real_variable("m");
    Variable r = fresh_real_variable("r");

    MultivariatePolynomial<Rational> mr(r);
    MultivariatePolynomial<Rational> one((Rational)1);
    MultivariatePolynomial<Rational> zero((Rational)0);

    UnivariatePolynomial<MultivariatePolynomial<Rational>> p(m, {mr+one, zero, -one});
    UnivariatePolynomial<MultivariatePolynomial<Rational>> q(m, {31, 16, 2});
    UnivariatePolynomial<MultivariatePolynomial<Rational>> res(r, {833, -124, 4});

    EXPECT_EQ(res, carl::resultant(p, q));
}

TEST(UnivariatePolynomial, PrincipalSubresultantCoefficient)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");
    MultivariatePolynomial<Rational> my(y);
    MultivariatePolynomial<Rational> one((Rational)1);
    MultivariatePolynomial<Rational> zero((Rational)0);
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {my, zero, zero, one});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {my, zero, one});
	
	UnivariatePolynomial<MultivariatePolynomial<Rational>> res1(x, {my*my+my*my*my});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> res2(x, {-my});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> res3(x, {one});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> res4(x, {one});
	
	auto r = carl::principalSubresultantsCoefficients(p, q);
	
	EXPECT_EQ(4, r.size());
	EXPECT_EQ(res1, r[0]);
	EXPECT_EQ(res2, r[1]);
	EXPECT_EQ(res3, r[2]);
	EXPECT_EQ(res4, r[3]);
}

TEST(UnivariatePolynomial, intervalCoeffs)
{
	Variable a = fresh_real_variable("a");
	UnivariatePolynomial<carl::Interval<double>> p(a);
	p *= p;
	p += p;
}


TEST(UnivariatePolynomial, coprimeFactor)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");
	MultivariatePolynomial<Rational> polY(y);

	MultivariatePolynomial<Rational> coeff1(Rational(12)*polY*polY + Rational(6)*polY);
	MultivariatePolynomial<Rational> coeff2(Rational(5)*polY);

	std::cout << coeff1 << " -> " << coeff1.coprime_factor() << std::endl;
	std::cout << coeff2 << " -> " << coeff2.coprime_factor() << std::endl;

	UnivariatePolynomial<MultivariatePolynomial<Rational>> pol(x, {coeff1, coeff2});

	std::cout << pol << " -> " << pol.coprime_factor() << std::endl;

	ASSERT_EQ(carl::get_denom(pol.coprime_factor()), 1);
}
