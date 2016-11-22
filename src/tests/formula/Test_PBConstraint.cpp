#include "gtest/gtest.h"

#include <carl/formula/pseudoboolean/PBConstraint.h>

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
	
	std::cout << pbc << std::endl;
	pbc.substituteIn(m);
	std::cout << pbc << std::endl;
	
	m.emplace(y, true);
	carl::ModelValue<Rational,Poly> res;
	pbc.evaluate(res, m);
	std::cout << pbc << std::endl;
	std::cout << res << std::endl;
}
