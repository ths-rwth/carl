#include <gtest/gtest.h>
#include <carl/core/Variable.h>
#include <carl/core/Monomial.h>
#include <carl/core/MonomialPool.h>
#include <list>
#include <boost/variant.hpp>

#include "../Common.h"

using namespace carl;

TEST(Monomial, Constructor)
{
	Variable x = freshRealVariable("x");
	
	auto m = createMonomial(x, 3);
	EXPECT_TRUE(m->exponents().size() == 1);
	EXPECT_TRUE(m->exponents().front().first == x);
	EXPECT_TRUE(m->exponents().front().second == 3);
}

TEST(Monomial, TotalDegree)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");

	Monomial::Arg m1 = x*x*x;
	EXPECT_TRUE(m1->tdeg() == 3);
	Monomial::Arg m2 = x*x*y;
	EXPECT_TRUE(m2->tdeg() == 3);
	Monomial::Arg m3 = x*y*y*y;
	EXPECT_TRUE(m3->tdeg() == 4);
}

TEST(Monomial, degreeCategories)
{
	Variable x = freshRealVariable("x");
	
	Monomial::Arg m1 = createMonomial(x, exponent(1));
	EXPECT_TRUE(m1->isLinear());
	EXPECT_TRUE(m1->isAtMostLinear());
	EXPECT_TRUE(!m1->isSquare());
	Monomial::Arg m2 = createMonomial(x, exponent(2));
	EXPECT_TRUE(!m2->isLinear());
	EXPECT_TRUE(!m2->isAtMostLinear());
	EXPECT_TRUE(m2->isSquare());
}

TEST(Monomial, Operators)
{
	Variable v0 = freshRealVariable("a");
	Variable v1 = freshRealVariable("b");
	Variable v2 = freshRealVariable("c");

	Monomial::Arg m0 = carl::createMonomial(v0, exponent(1));
	m0 = m0 * v1;
	EXPECT_EQ((unsigned)1,m0->exponentOfVariable(v1));
	m0 = m0 * v1;
	EXPECT_EQ((unsigned)2,m0->exponentOfVariable(v1));
	EXPECT_EQ((unsigned)3,m0->tdeg());
	EXPECT_EQ((unsigned)0,m0->exponentOfVariable(v2));
	m0 = m0 * v2;
	EXPECT_EQ((unsigned)4,m0->tdeg());
	EXPECT_EQ((unsigned)3,m0->nrVariables());

	Monomial::Arg m3 = carl::createMonomial(v1, exponent(1));
	Monomial::Arg m2 = carl::createMonomial(v1, exponent(1));
	m2 = m2 * v1;
	m3 = m3 * v1;
	EXPECT_EQ(m2, m3);
}

TEST(Monomial, VariableMultiplication)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1)})), x);
	EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)})), x * y);
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2), std::make_pair(y, 1)})), x * x * y);
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 2)})), y * x * y);
	//EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 3)})), x * x * x);
}

TEST(Monomial, MonomialMultiplication)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	EXPECT_EQ(
		carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2), std::make_pair(y, 3)})),
		carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 2)})) * carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)}))
	);
	EXPECT_EQ(
		carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2), std::make_pair(y, 3)})),
		carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2)})) * carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(y, 3)}))
	);
	EXPECT_EQ(
		carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 5), std::make_pair(y, 3)})),
		carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2)})) * carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 3), std::make_pair(y, 3)}))
	);
}

TEST(Monomial, derivative)
{
	Variable v0((unsigned)1);
	Variable v1((unsigned)2);
	Monomial::Arg m0 = v0 * v1;
	auto d1 = m0->derivative(v0);
	EXPECT_EQ(1, d1.first);
	EXPECT_EQ(v1, d1.second);
}

TEST(Monomial, division)
{
	Variable v0((unsigned)1);
	Variable v1((unsigned)2);
	Variable v2((unsigned)3);

	Monomial::Arg m0 = v0 * v0 * v1 * v1 * v2;
	Monomial::Arg m1 = v0 * v0 * v0;
	Monomial::Arg m2 = v0 * v0 * v1 * v2;
	Monomial::Arg m0x = v0 * v0 * v1 * v2;
	Monomial::Arg m0y = v0 * v0 * v1 * v1;
	Monomial::Arg m0z = v0 * v1;
	Monomial::Arg tmp;
	Monomial::Arg one;
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
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Monomial::Arg m1 = carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(y, 2), std::make_pair(x, 2)}));
	Monomial::Arg m2 = carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)}));
	Monomial::Arg m3 = carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(y, 1), std::make_pair(x, 1)}));
//	std::cout << m1 << " divisible by " << m2 << std::endl;
//	std::cout << m2 << " == " << m3 << std::endl;
	EXPECT_TRUE(m2==m3);
	EXPECT_TRUE(m1->divisible(m2));
	
	{
		Monomial::Arg m1 = createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(y, 2)}));
		Monomial::Arg m2 = createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(y, 2), std::make_pair(x, 2)}));
		EXPECT_TRUE(m2->divisible(m1));
	}
}

TEST(Monomial, Comparison)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Variable z = freshRealVariable("z");

	ComparisonList<Monomial::Arg> monomials;
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
	ComparisonList<Variable,Monomial::Arg> list;

	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");

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
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Monomial::Arg m1 = x*x*y*y*y*y;
	Monomial::Arg m2 = x*y*y;
	EXPECT_EQ(m2, m1->sqrt());
}

TEST(Monomial, pow)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Monomial::Arg one;
	Monomial::Arg m1 = x*y*y;
	Monomial::Arg m2 = x*x*y*y*y*y;
	EXPECT_EQ(one, m1->pow(0));
	EXPECT_EQ(m1, m1->pow(1));
	EXPECT_EQ(m2, m1->pow(2));
}

TEST(Monomial, CalcLCM)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Monomial::Arg m1 = y*y;
	Monomial::Arg m2 = x*x*y;
	EXPECT_EQ(y, Monomial::calcLcmAndDivideBy(m1, m2));
}
