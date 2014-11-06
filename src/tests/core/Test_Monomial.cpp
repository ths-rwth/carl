#include "gtest/gtest.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Monomial_derivative.h"
#include "Util.cpp"
#include <list>
#include <boost/variant.hpp>

using namespace carl;

TEST(Monomial, Operators)
{
    Variable v0 = Variable((unsigned)1);
    Variable v1 = Variable((unsigned)2);
    Variable v2 = Variable((unsigned)3);
    
#ifdef USE_MONOMIAL_POOL
	Monomial::Arg m0 = MonomialPool::getInstance().create(v0, 1);
#else
    Monomial::Arg m0 = std::make_shared<Monomial>(v0);
#endif
    m0 = m0 * v1;
    EXPECT_EQ((unsigned)1,m0->exponentOfVariable(v1));
    m0 = m0 * v1;
    EXPECT_EQ((unsigned)2,m0->exponentOfVariable(v1));
    EXPECT_EQ((unsigned)3,m0->tdeg());
    EXPECT_EQ((unsigned)0,m0->exponentOfVariable(v2));
    m0 = m0 * v2;
    EXPECT_EQ((unsigned)4,m0->tdeg());
    EXPECT_EQ((unsigned)3,m0->nrVariables());
    
#ifdef USE_MONOMIAL_POOL
	Monomial::Arg m2 = MonomialPool::getInstance().create(v1, 1);
	Monomial::Arg m3 = MonomialPool::getInstance().create(v1, 1);
#else
    Monomial::Arg m2 = std::make_shared<Monomial>(v1);
    Monomial::Arg m3 = std::make_shared<Monomial>(v1);
#endif
    m2 = m2 * v1;
    m3 = m3 * v1;
    EXPECT_EQ(m2, m3);
}

TEST(Monomial, VariableMultiplication)
{
    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");
#ifdef USE_MONOMIAL_POOL
	EXPECT_EQ(MonomialPool::getInstance().create({std::make_pair(x, 1)}), x);
    EXPECT_EQ(MonomialPool::getInstance().create({std::make_pair(x, 1), std::make_pair(y, 1)}), x * y);
    EXPECT_EQ(MonomialPool::getInstance().create({std::make_pair(x, 2), std::make_pair(y, 1)}), x * x * y);
    EXPECT_EQ(MonomialPool::getInstance().create({std::make_pair(x, 1), std::make_pair(y, 2)}), y * x * y);
    EXPECT_EQ(MonomialPool::getInstance().create({std::make_pair(x, 3)}), x * x * x);
#else
    EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1)})), x);
    EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 1)})), x * y);
    EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 2), std::make_pair(y, 1)})), x * x * y);
    EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 1), std::make_pair(y, 2)})), y * x * y);
    EXPECT_EQ(carl::createMonomial(std::initializer_list<std::pair<Variable, exponent>>({std::make_pair(x, 3)})), x * x * x);
#endif
}

TEST(Monomial, MonomialMultiplication)
{
    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");
#ifdef USE_MONOMIAL_POOL
    EXPECT_EQ(
		MonomialPool::getInstance().create({std::make_pair(x, 2), std::make_pair(y, 3)}), 
		MonomialPool::getInstance().create({std::make_pair(x, 1), std::make_pair(y, 2)}) * MonomialPool::getInstance().create({std::make_pair(x, 1), std::make_pair(y, 1)})
	);
    EXPECT_EQ(
		MonomialPool::getInstance().create({std::make_pair(x, 2), std::make_pair(y, 3)}), 
		MonomialPool::getInstance().create({std::make_pair(x, 2)}) * MonomialPool::getInstance().create({std::make_pair(y, 3)})
	);
    EXPECT_EQ(
		MonomialPool::getInstance().create({std::make_pair(x, 5), std::make_pair(y, 3)}), 
		MonomialPool::getInstance().create({std::make_pair(x, 2)}) * MonomialPool::getInstance().create({std::make_pair(x, 3), std::make_pair(y, 3)})
	);
#else
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
#endif
}

TEST(Monomial, derivative)
{
    Variable v0((unsigned)1);
    Variable v1((unsigned)2);
    Monomial::Arg m0 = v0 * v1;
    Term<int>* t = m0->derivative<int>(v0);
    EXPECT_EQ((unsigned)1, t->getNrVariables());
    
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
    EXPECT_FALSE(m0->divide(m1).second);
    EXPECT_FALSE(m1->divide(m0).second);
    EXPECT_EQ(m0x, m0->divide(v1));
    EXPECT_EQ(m0y, m0->divide(v2));
    EXPECT_EQ(v1, m0->divide(m2).first);
}

TEST(Monomial, Comparison)
{
    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");
    Variable z = pool.getFreshVariable("z");

    ComparisonList<Monomial::Arg> monomials;
    monomials.push_back(y * y * y);
    monomials.push_back(x * y * z);
    monomials.push_back(x * y * y);
    monomials.push_back(x * x * y);
    monomials.push_back(x * x * x);
    monomials.push_back(x * y * y * z);
    monomials.push_back(x * x * z * z);

    expectRightOrder(monomials);
}


TEST(Monomial, OtherComparison)
{
    ComparisonList<Variable,Monomial::Arg> list;

    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");

    list.push_back(y);
    list.push_back(x);
    list.push_back(y * y);
    list.push_back(x * y);
    list.push_back(x * x);
    list.push_back(x * x * y);
    list.push_back(x * x * x);
    list.push_back(x * x * x * x);

    expectRightOrder(list);
}
