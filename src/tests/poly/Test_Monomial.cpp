#include <gtest/gtest.h>
#include <carl/core/Variable.h>
#include <carl/poly/umvpoly/Monomial.h>
#include <carl/poly/umvpoly/MonomialPool.h>
#include <list>
#include <boost/variant.hpp>

#include <carl/poly/umvpoly/functions/Derivative.h>
#include <carl/poly/umvpoly/functions/Power.h>

#include "../Common.h"

TEST(Monomial, Constructor)
{
	auto x = carl::freshRealVariable("x");
	
	auto m = carl::createMonomial(x, 3);
	EXPECT_TRUE(m->exponents().size() == 1);
	EXPECT_TRUE(m->exponents().front().first == x);
	EXPECT_TRUE(m->exponents().front().second == 3);
}

TEST(Monomial, tdeg)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");

	carl::Monomial::Arg m1 = x*x*x;
	EXPECT_TRUE(m1->tdeg() == 3);
	carl::Monomial::Arg m2 = x*x*y;
	EXPECT_TRUE(m2->tdeg() == 3);
	carl::Monomial::Arg m3 = x*y*y*y;
	EXPECT_TRUE(m3->tdeg() == 4);
}

TEST(Monomial, degreeCategories)
{
	auto x = carl::freshRealVariable("x");
	
	carl::Monomial::Arg m1 = carl::createMonomial(x, 1);
	EXPECT_FALSE(m1->isConstant());
	EXPECT_TRUE(m1->isLinear());
	EXPECT_TRUE(m1->isAtMostLinear());
	EXPECT_FALSE(m1->isSquare());
	carl::Monomial::Arg m2 = carl::createMonomial(x, 2);
	EXPECT_FALSE(m2->isConstant());
	EXPECT_FALSE(m2->isLinear());
	EXPECT_FALSE(m2->isAtMostLinear());
	EXPECT_TRUE(m2->isSquare());
}

TEST(Monomial, hasNoOtherVariable)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	carl::Monomial::Arg m1 = carl::createMonomial(x, 1);
	carl::Monomial::Arg m2 = x*x;
	carl::Monomial::Arg m3 = x*x*y;
	carl::Monomial::Arg m4 = carl::createMonomial(y, 1);
	carl::Monomial::Arg m5 = y*y;
	carl::Monomial::Arg m6 = y*y*x;
	
	EXPECT_TRUE(m1->hasNoOtherVariable(x));
	EXPECT_FALSE(m1->hasNoOtherVariable(y));
	EXPECT_TRUE(m2->hasNoOtherVariable(x));
	EXPECT_FALSE(m2->hasNoOtherVariable(y));
	EXPECT_FALSE(m3->hasNoOtherVariable(x));
	EXPECT_FALSE(m3->hasNoOtherVariable(y));
	EXPECT_FALSE(m4->hasNoOtherVariable(x));
	EXPECT_TRUE(m4->hasNoOtherVariable(y));
	EXPECT_FALSE(m5->hasNoOtherVariable(x));
	EXPECT_TRUE(m5->hasNoOtherVariable(y));
	EXPECT_FALSE(m6->hasNoOtherVariable(x));
	EXPECT_FALSE(m6->hasNoOtherVariable(y));
}

TEST(Monomial, Operators)
{
	auto v0 = carl::freshRealVariable("a");
	auto v1 = carl::freshRealVariable("b");
	auto v2 = carl::freshRealVariable("c");

	carl::Monomial::Arg m0 = carl::createMonomial(v0, 1);
	m0 = m0 * v1;
	EXPECT_EQ((unsigned)1,m0->exponentOfVariable(v1));
	m0 = m0 * v1;
	EXPECT_EQ((unsigned)2,m0->exponentOfVariable(v1));
	EXPECT_EQ((unsigned)3,m0->tdeg());
	EXPECT_EQ((unsigned)0,m0->exponentOfVariable(v2));
	m0 = m0 * v2;
	EXPECT_EQ((unsigned)4,m0->tdeg());
	EXPECT_EQ((unsigned)3,m0->nrVariables());

	carl::Monomial::Arg m3 = carl::createMonomial(v1, 1);
	carl::Monomial::Arg m2 = carl::createMonomial(v1, 1);
	m2 = m2 * v1;
	m3 = m3 * v1;
	EXPECT_EQ(m2, m3);
}

TEST(Monomial, VariableMultiplication)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1)})), x);
	EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)})), x * y);
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2), std::make_pair(y, 1)})), x * x * y);
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 2)})), y * x * y);
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 3)})), x * x * x);
}

TEST(Monomial, MonomialMultiplication)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	EXPECT_EQ(
		carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 2), std::make_pair(y, 3)})),
		carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 1), std::make_pair(y, 2)})) * carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)}))
	);
	EXPECT_EQ(
		carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 2), std::make_pair(y, 3)})),
		carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 2)})) * carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(y, 3)}))
	);
	EXPECT_EQ(
		carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 5), std::make_pair(y, 3)})),
		carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 2)})) * carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 3), std::make_pair(y, 3)}))
	);
}

#define DERIVATIVE_EQUAL(n,f,expr) { \
	auto res = carl::derivative(m, y, n); \
	auto ref = std::pair<std::size_t,carl::Monomial::Arg>(f, expr); \
	EXPECT_EQ(ref, res); \
	}
TEST(Monomial, derivative)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	auto z = carl::freshRealVariable("z");

	{
		auto m = x*x*y*y*y*z;
		DERIVATIVE_EQUAL(0, 1, m);
		DERIVATIVE_EQUAL(1, 3, x*x*y*y*z);
		DERIVATIVE_EQUAL(2, 6, x*x*y*z);
		DERIVATIVE_EQUAL(3, 6, x*x*z);
		DERIVATIVE_EQUAL(4, 0, nullptr);
		DERIVATIVE_EQUAL(5, 0, nullptr);
	}
}

TEST(Monomial, division)
{
	auto v0 = carl::freshRealVariable("x");
	auto v1 = carl::freshRealVariable("y");
	auto v2 = carl::freshRealVariable("z");

	carl::Monomial::Arg m0 = v0 * v0 * v1 * v1 * v2;
	carl::Monomial::Arg m1 = v0 * v0 * v0;
	carl::Monomial::Arg m2 = v0 * v0 * v1 * v2;
	carl::Monomial::Arg m0x = v0 * v0 * v1 * v2;
	carl::Monomial::Arg m0y = v0 * v0 * v1 * v1;
	carl::Monomial::Arg m0z = v0 * v1;
	carl::Monomial::Arg tmp;
	carl::Monomial::Arg one;
	EXPECT_TRUE(m0->divide(one, tmp));
	EXPECT_EQ(m0, tmp);
	EXPECT_TRUE(m1->divide(one, tmp));
	EXPECT_EQ(m1, tmp);
	EXPECT_FALSE(m0->divide(m1, tmp));
	EXPECT_FALSE(m1->divide(m0, tmp));
	EXPECT_TRUE(m0->divide(v1, tmp));
	EXPECT_EQ(m0x, tmp);
	EXPECT_TRUE(m0->divide(v2, tmp));
	EXPECT_EQ(m0y, tmp);
	EXPECT_TRUE(m0->divide(m2, tmp));
	EXPECT_EQ(v1, tmp);
}

TEST(Monomial, divisible)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	auto m1 = carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(y, 2), std::make_pair(x, 2)}));
	auto m2 = carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)}));
	auto m3 = carl::createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(y, 1), std::make_pair(x, 1)}));
//	std::cout << m1 << " divisible by " << m2 << std::endl;
//	std::cout << m2 << " == " << m3 << std::endl;
	EXPECT_TRUE(m2==m3);
	EXPECT_TRUE(m1->divisible(m2));
	
	{
		auto m1 = createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(y, 2)}));
		auto m2 = createMonomial(std::initializer_list<std::pair<carl::Variable, carl::exponent>>({std::make_pair(y, 2), std::make_pair(x, 2)}));
		EXPECT_TRUE(m2->divisible(m1));
	}
}

TEST(Monomial, Comparison)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	auto z = carl::freshRealVariable("z");

	ComparisonList<carl::Monomial::Arg> monomials;
	monomials.push_back(x * x * x);
	monomials.push_back(x * x * y);
	monomials.push_back(x * y * y);
	monomials.push_back(x * y * z);
	monomials.push_back(y * y * y);
	monomials.push_back(x * x * z * z);
	monomials.push_back(x * y * y * z);

	expectRightOrder(monomials);
}


TEST(Monomial, OtherComparison)
{
	ComparisonList<carl::Variable,carl::Monomial::Arg> list;

	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");

	list.push_back(x);
	list.push_back(y);
	list.push_back(x * x);
	list.push_back(x * y);
	list.push_back(y * y);
	list.push_back(x * x * x);
	list.push_back(x * x * y);
	list.push_back(x * x * x * x);

	expectRightOrder(list);
}

TEST(Monomial, sqrt)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	carl::Monomial::Arg m1 = x*x*y*y*y*y;
	carl::Monomial::Arg m2 = x*y*y;
	EXPECT_EQ(m2, m1->sqrt());
}

TEST(Monomial, pow)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	carl::Monomial::Arg one;
	carl::Monomial::Arg m1 = x*y*y;
	carl::Monomial::Arg m2 = x*x*y*y*y*y;
	EXPECT_EQ(one, carl::pow(m1, 0));
	EXPECT_EQ(m1, carl::pow(m1, 1));
	EXPECT_EQ(m2, carl::pow(m1, 2));
}

TEST(Monomial, CalcLCM)
{
	auto x = carl::freshRealVariable("x");
	auto y = carl::freshRealVariable("y");
	carl::Monomial::Arg m1 = y*y;
	carl::Monomial::Arg m2 = x*x*y;
	EXPECT_EQ(y, carl::Monomial::calcLcmAndDivideBy(m1, m2));
}
