#include <gtest/gtest.h>

#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-formula/formula/Formula.h>
#include <carl-formula/symmetry/symmetry.h>

#include "../number_types.h"

typedef carl::MultivariatePolynomial<Rational> Pol;
typedef carl::Constraint<Pol> Constr;
typedef carl::Formula<Pol> FormulaT;

void report_aut(void*, const unsigned int n, const unsigned int* aut)
{
  std::cout << "Generator: ";
  for (unsigned int i = 0; i < n; ++i) {
	  std::cout << aut[i] << " ";
  }
  std::cout << std::endl;
}

TEST(Symmetry, BlissBase)
{
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");
	
	Pol lhsA = Rational(1)*x*x + Rational(1)*y*y + Rational(-1);
	Pol lhsB = Rational(1)*x*x*y + Rational(1)*x*y*y + Rational(-1);

	FormulaT fA(Constr(lhsA, carl::Relation::EQ));
	FormulaT fB(Constr(lhsB, carl::Relation::EQ));
	FormulaT f(carl::FormulaType::AND, {fA, fB});

#ifdef USE_BLISS
	FormulaT symm(Constr(Rational(1)*x - Rational(1)*y, carl::Relation::LEQ));
#else
	FormulaT symm(carl::FormulaType::TRUE);
#endif
	
	auto res2 = carl::formula::breakSymmetries(f);
	EXPECT_EQ(res2, symm);
}
