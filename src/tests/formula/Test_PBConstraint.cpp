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
	auto x = carl::freshBooleanVariable("x");
	auto y = carl::freshBooleanVariable("y");
	auto z = carl::freshBooleanVariable("z");
	carl::PBConstraint pbc;
	std::vector<std::pair<carl::Variable, int>> lhs;
	lhs.emplace_back(x, 2);
	lhs.emplace_back(y, 5);
	lhs.emplace_back(z, 4);
	pbc.setLHS(lhs);
	pbc.setRHS(6);
	pbc.setRelation(carl::Relation::GREATER);
	
	carl::Model<Rational, Poly> m;
	m.emplace(x, true);
	m.emplace(z, false);
	
	pbc = carl::model::substitute(pbc, m);
	EXPECT_TRUE(pbc.getLHS().size() == 1);
	EXPECT_TRUE(pbc.getLHS()[0].first == y);
	EXPECT_TRUE(pbc.getLHS()[0].second == 5);
	EXPECT_TRUE(pbc.getRHS() == 4);
	
	m.emplace(y, true);
	auto res = carl::model::evaluate(pbc, m);
	EXPECT_TRUE(res.isBool() && res.asBool());
}

TEST(PBConstraint, Formula)
{
	using Rational = mpq_class;
	using Poly = carl::MultivariatePolynomial<Rational>;
	auto x = carl::freshBooleanVariable("x");
	auto y = carl::freshBooleanVariable("y");
	auto z = carl::freshBooleanVariable("z");
	carl::PBConstraint pbc;
	std::vector<std::pair<carl::Variable, int>> lhs;
	lhs.emplace_back(x, 2);
	lhs.emplace_back(y, 5);
	lhs.emplace_back(z, 4);
	pbc.setLHS(lhs);
	pbc.setRHS(6);
	pbc.setRelation(carl::Relation::GREATER);
	
	
	carl::Formula<Poly> f(pbc);
}
