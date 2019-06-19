#include "gtest/gtest.h"

#include <carl/formula/Formula.h>
#include <carl-model/Model.h>
#include <carl-model/evaluation/ModelEvaluation.h>

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

TEST(ModelEvaluation, Constraint)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Variable z = freshRealVariable("z");
	ModelT m;
	Pol p = Pol(x) + Pol(y)*z + Pol(1);
	ConstraintT c1(p, carl::Relation::EQ);
	ConstraintT c2(Pol(x), carl::Relation::EQ);

	Variable v;
	Pol repl;
	c1.getSubstitution(v, repl);
	EXPECT_EQ(v, x);
	EXPECT_EQ(repl, -Pol(y)*z - Pol(1));

	m.emplace(v, carl::createSubstitution<Rational,Pol,carl::ModelPolynomialSubstitution<Rational, Pol>>(repl));
	model::substituteIn(c2, m);
	EXPECT_EQ(c2, ConstraintT(Pol(y)*z + Pol(1), carl::Relation::EQ));
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

TEST(ModelEvaluation, EvaluateRANIRConstraint)
{
	Variable x = freshRealVariable("x");
	ModelT m;
	IntervalT i(Rational(27)/32, BoundType::STRICT, Rational(7)/8, BoundType::STRICT);
	UnivariatePolynomial<Rational> p(x, {Rational(-3), Rational(0), Rational(4)}); // 4*r^2 -3
	m.assign(x, RANT(p, i)); // x = (IR ]27/32, 7/8[, (4)*__r^2 + -3 R)
	FormulaT f = FormulaT(ConstraintT(Pol(x), Relation::LEQ));
	auto res = model::evaluate(f, m);
	EXPECT_TRUE(res.isBool());
	EXPECT_FALSE(res.asBool());
}

TEST(ModelEvaluation, EvaluateWithMVR)
{
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	ModelT m;
	Pol p = Pol(x*x) - y*y - Rational(4);
	Pol mvrpol = Pol(MVRootT::var() * MVRootT::var()) - Rational(3);
	m.assign(y, createSubstitution<Rational>(MVRootT(mvrpol, 1)));
	auto res = carl::model::realRoots(p, x, m);

	using UPol = UnivariatePolynomial<Rational>;

	UPol upol(x, {Rational(-7), 0, 1});
	RealAlgebraicNumber<Rational> r1(upol, Interval<Rational>(-3,BoundType::STRICT,-1,BoundType::STRICT));
	RealAlgebraicNumber<Rational> r2(upol, Interval<Rational>(1,BoundType::STRICT,3,BoundType::STRICT));

	EXPECT_EQ(res, std::decay<decltype(res)>::type({r1, r2}));
}

TEST(ModelEvaluation, EvaluateBV)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort bvSort = sm.addSort("BitVec", carl::VariableType::VT_UNINTERPRETED);
	sm.makeSortIndexable(bvSort, 1, carl::VariableType::VT_BITVECTOR);
	carl::Variable a = carl::freshBitvectorVariable("a");
	carl::Sort s = carl::getSort("BitVec", std::vector<std::size_t>({4}));
	carl::BVVariable v(a, s);
	carl::BVTerm bvt_v(carl::BVTermType::VARIABLE, v);
	
	carl::BVValue bv1(4, 0);
	carl::BVTerm bvt_v1(carl::BVTermType::CONSTANT, bv1);
	carl::BVValue bv2(4, 17);
	
	FormulaT f = FormulaT(BVConstraint::create(BVCompareRelation::EQ, bvt_v, bvt_v1));
	
	std::cout << f << std::endl;
	
	ModelT m;
	m.emplace(v, bv2);
	std::cout << m << std::endl;
	
	auto res = carl::model::evaluate(f, m);
	std::cout << res << std::endl;
}
