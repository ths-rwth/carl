#include "gtest/gtest.h"

#include <carl/formula/Formula.h>
#include <carl/formula/model/Model.h>
#include <carl/formula/model/evaluation/ModelEvaluation.h>

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> Constr;
typedef Formula<Pol> FormulaT;
typedef Model<Rational,Pol> ModelT;

TEST(ModelEvaluation, Formula)
{
	ModelT m;
	FormulaT f = FormulaT(FormulaType::TRUE);
	std::cout << model::substitute(f, m) << std::endl;
}
