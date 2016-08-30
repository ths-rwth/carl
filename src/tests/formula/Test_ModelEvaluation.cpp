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

TEST(ModelEvaluation, EvaluateMVR)
{
	Variable x = freshRealVariable("x");
	Variable z = freshRealVariable("_z");
	ModelT m;
	m.assign(x, Rational(-1));
	MultivariateRoot<Pol> mvr(Pol(x)*z, 1, z);
	auto res = model::evaluate(mvr, m);
	EXPECT_TRUE(res.isRational());
	EXPECT_TRUE(isZero(res.asRational()));
}
