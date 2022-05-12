#include "gtest/gtest.h"

#include <carl/core/VariablePool.h>
#include "carl-formula/formula/Formula.h"
#include <carl-io/SMTLIBStream.h>

#include "../Common.h"

using FormulaT = carl::Formula<carl::MultivariatePolynomial<Rational>>;

TEST(SMTLIBStream, Base)
{
	carl::Variable x = carl::freshRealVariable("x");
	Rational r = 4;
	carl::MultivariatePolynomial<Rational> mp = Rational(r*r)*x*x + r*x + r;

	FormulaT f(mp, carl::Relation::GEQ);
	std::cout << carl::io::outputSMTLIB(carl::Logic::QF_NRA, {f}) << std::endl;
}
