#include "gtest/gtest.h"

#include <carl/formula/Formula.h>
#include <carl/formula/model/Model.h>
#include <carl/formula/model/evaluation/ModelEvaluation.h>

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> ConstraintT;
typedef MultivariateRoot<Pol> MVRootT;
typedef Formula<Pol> FormulaT;
typedef Interval<Rational> IntervalT;
typedef RealAlgebraicNumber<Rational> RANT;
typedef Model<Rational,Pol> ModelT;

TEST(ModelEvaluation, Formula)
{
	ModelT m;
	FormulaT f = FormulaT(FormulaType::TRUE);
	auto res = model::substitute(f, m);
	EXPECT_TRUE(res.isTrue());
}

TEST(ModelEvaluation, EvaluateMVR)
{
	Variable x = freshRealVariable("x");
	Variable z = MultivariateRoot<Pol>::var();
	ModelT m;
	m.assign(x, Rational(-1));
	MultivariateRoot<Pol> mvr(Pol(x)*z, 1);
	auto res = model::evaluate(mvr, m);
	EXPECT_TRUE(res.isRational());
	EXPECT_TRUE(isZero(res.asRational()));
}

TEST(ModelEvaluation, EvaluateRANIR)
{
	Variable x = freshRealVariable("x");
	ModelT m;
	IntervalT i(Rational(-3)/2, BoundType::STRICT, Rational(-1), BoundType::STRICT);
	UnivariatePolynomial<Rational> p(x, {Rational(-2), Rational(0), Rational(1)});
	m.assign(x, RANT(p, i));
	FormulaT f = FormulaT(ConstraintT(Pol(p), Relation::EQ));
	auto res = model::evaluate(f, m);
	EXPECT_TRUE(res.isBool());
	EXPECT_TRUE(res.asBool());
}
