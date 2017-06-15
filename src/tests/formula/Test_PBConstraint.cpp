#include "gtest/gtest.h"

#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/VariablePool.h>
#include <carl/formula/Formula.h>
#include <carl/formula/model/Model.h>
#include <carl/formula/pseudoboolean/PBConstraint.h>
#include <carl/numbers/numbers.h>

TEST(PBConstraint, Basic)
{
	using Rational = mpq_class;
	using Poly = carl::MultivariatePolynomial<Rational>;
	auto x = carl::freshIntegerVariable("x");
	auto y = carl::freshIntegerVariable("y");
	auto z = carl::freshIntegerVariable("z");
	carl::PBConstraint<Poly> pbc;
	std::vector<std::pair<Rational, carl::Variable>> lhs;
	lhs.emplace_back(2, x);
	lhs.emplace_back(5, y);
	lhs.emplace_back(4, z);
	pbc.setLHS(lhs);
	pbc.setRHS(6);
	pbc.setRelation(carl::Relation::GREATER);
	
	carl::Model<Rational, Poly> m;
	m.emplace(x, Rational(1));
	m.emplace(z, Rational(0));
	
	pbc = carl::model::substitute(pbc, m);

	EXPECT_TRUE(pbc.getLHS().size() == 1);
	EXPECT_TRUE(pbc.getLHS()[0].first == 5);
	EXPECT_TRUE(pbc.getLHS()[0].second == y);
	EXPECT_TRUE(pbc.getRHS() == 4);
	
	m.emplace(y, Rational(1));
	auto res = carl::model::evaluate(pbc, m);
	EXPECT_TRUE(res.isBool() && res.asBool());
}

TEST(PBConstraint, Formula)
{
	using Rational = mpq_class;
	using Poly = carl::MultivariatePolynomial<Rational>;
	auto x = carl::freshIntegerVariable("x");
	auto y = carl::freshIntegerVariable("y");
	auto z = carl::freshIntegerVariable("z");
	carl::PBConstraint<Poly> pbc;
	std::vector<std::pair<Rational,carl::Variable>> lhs;
	lhs.emplace_back(2, x);
	lhs.emplace_back(5, y);
	lhs.emplace_back(4, z);
	pbc.setLHS(lhs);
	pbc.setRHS(6);
	pbc.setRelation(carl::Relation::GREATER);
	
	
	carl::Formula<Poly> f(pbc);
}
