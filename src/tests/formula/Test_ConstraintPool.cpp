#include "gtest/gtest.h"

#include "carl/formula/Constraint.h"
#include "../Common.h"


using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> ConstraintT;

TEST(ConstraintPool, singleton)
{
	ConstraintPool<Pol>& pool1 = ConstraintPool<Pol>::getInstance();
	ConstraintPool<Pol>& pool2 = ConstraintPool<Pol>::getInstance();
	EXPECT_EQ(&pool1, &pool2);
	Variable x = freshRealVariable("x");
	Pol p = Pol(x);
	
	auto m = pool1.create(p, carl::Relation::EQ);
	EXPECT_EQ(pool2.size(), pool1.size());
}

TEST(ConstraintPool, trivial)
{
	ConstraintPool<Pol>& pool = ConstraintPool<Pol>::getInstance();

	auto m_true = pool.create(true);
	auto m_false = pool.create(false);
	EXPECT_EQ(m_true->isConsistent(), 1);
	EXPECT_EQ(m_false->isConsistent(), 0);
}

TEST(ConstraintPool, trivial_constraint)
{
	auto c_true = ConstraintT(true);
	auto c_false = ConstraintT(false);
	EXPECT_EQ(c_true.isConsistent(), 1);
	EXPECT_EQ(c_false.isConsistent(), 0);


}

TEST(ConstraintPool, bound)
{
	ConstraintPool<Pol>& pool = ConstraintPool<Pol>::getInstance();

	Variable x = freshRealVariable("x");

	auto m_bound = pool.create(x, carl::Relation::LESS, Rational(1));
	EXPECT_EQ(m_bound->isConsistent(), 2);

	auto m_bound_2 = pool.create(x, carl::Relation::LESS, Rational(2));
	EXPECT_EQ(m_bound_2->isConsistent(), 2);

	EXPECT_NE(m_bound.get(), m_bound_2.get());
}

TEST(ConstraintPool, bound_constraint)
{
	Variable x = freshRealVariable("x");

	auto c_bound = ConstraintT(x, carl::Relation::LESS, Rational(1));
	EXPECT_EQ(c_bound.isConsistent(), 2);

	auto c_bound_2 = ConstraintT(x, carl::Relation::LESS, Rational(2));
	EXPECT_EQ(c_bound_2.isConsistent(), 2);

	EXPECT_NE(c_bound, c_bound_2);
}


TEST(ConstraintPool, constr)
{
	ConstraintPool<Pol>& pool = ConstraintPool<Pol>::getInstance();

	Variable x = freshRealVariable("x");

	auto m_bound = pool.create(Pol(x)*x + x + Rational(5), carl::Relation::LESS);
	EXPECT_EQ(m_bound->isConsistent(), 2);

	auto m_bound_2 = pool.create(Pol(x)*x + x + Rational(5), carl::Relation::GREATER);
	EXPECT_EQ(m_bound_2->isConsistent(), 2);

	EXPECT_NE(m_bound.get(), m_bound_2.get());
}

TEST(RawConstraint, constr)
{
	Variable x = freshRealVariable("x");

	auto data_1 = RawConstraint(Pol(x)*x + x + Rational(5), carl::Relation::LESS);
	EXPECT_EQ(data_1.is_consistent(), 2);

	auto data_2 = RawConstraint(Pol(x)*x + x + Rational(5), carl::Relation::GREATER);
	EXPECT_EQ(data_2.is_consistent(), 2);
}