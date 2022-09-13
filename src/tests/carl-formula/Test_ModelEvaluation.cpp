#include "gtest/gtest.h"

#include <carl-formula/formula/Formula.h>
#include <carl-arith/constraint/Substitution.h>
#include <carl-formula/model/Model.h>
#include <carl-formula/model/evaluation/ModelEvaluation.h>

#include "../Common.h"

using namespace carl;

typedef MultivariatePolynomial<Rational> Pol;
typedef Constraint<Pol> ConstraintT;
typedef MultivariateRoot<Pol> MVRootT;
typedef Formula<Pol> FormulaT;
typedef Interval<Rational> IntervalT;
typedef IntRepRealAlgebraicNumber<Rational> RANT;
typedef Model<Rational,Pol> ModelT;

TEST(ModelEvaluation, Formula)
{
	ModelT m;
	FormulaT f = FormulaT(FormulaType::TRUE);
	auto res = substitute(f, m);
	EXPECT_TRUE(res.is_true());
}

TEST(ModelEvaluation, Constraint)
{
	Variable x = fresh_real_variable("x");
	Variable y = fresh_real_variable("y");
	Variable z = fresh_real_variable("z");
	ModelT m;
	Pol p = Pol(x) + Pol(y)*z + Pol(1);
	ConstraintT c1(p, carl::Relation::EQ);
	ConstraintT c2(Pol(x), carl::Relation::EQ);

	auto subs = carl::get_substitution(c1);
	EXPECT_EQ(subs->first, x);
	EXPECT_EQ(subs->second, -Pol(y)*z - Pol(1));

	m.emplace(subs->first, carl::createSubstitution<Rational,Pol,carl::ModelPolynomialSubstitution<Rational, Pol>>(subs->second));
	substitute_inplace(c2, m);
	EXPECT_EQ(c2, ConstraintT(Pol(y)*z + Pol(1), carl::Relation::EQ));
}


TEST(ModelEvaluation, EvaluateMVR)
{
	Variable x = fresh_real_variable("x");
	Variable z = fresh_real_variable("R");
	ModelT m;
	m.assign(x, Rational(-1));
	MultivariateRoot<Pol> mvr(Pol(x)*z, 1, z);
	auto res = evaluate(mvr, m);
	EXPECT_TRUE(res.isRational());
	EXPECT_TRUE(is_zero(res.asRational()));
}

TEST(ModelEvaluation, EvaluateRANIR)
{
	Variable x = fresh_real_variable("x");
	ModelT m;
	IntervalT i(Rational(-3)/2, BoundType::STRICT, Rational(-1), BoundType::STRICT);
	UnivariatePolynomial<Rational> p(x, {Rational(-2), Rational(0), Rational(1)});
	m.assign(x, RANT::create_safe(p, i));
	FormulaT f = FormulaT(ConstraintT(Pol(p), Relation::EQ));
	auto res = evaluate(f, m);
	EXPECT_TRUE(res.isBool());
	EXPECT_TRUE(res.asBool());
}

TEST(ModelEvaluation, EvaluateRANIRConstraint)
{
	Variable x = fresh_real_variable("x");
	ModelT m;
	IntervalT i(Rational(27)/32, BoundType::STRICT, Rational(7)/8, BoundType::STRICT);
	UnivariatePolynomial<Rational> p(x, {Rational(-3), Rational(0), Rational(4)}); // 4*r^2 -3
	m.assign(x, RANT::create_safe(p, i)); // x = (IR ]27/32, 7/8[, (4)*__r^2 + -3 R)
	FormulaT f = FormulaT(ConstraintT(Pol(x), Relation::LEQ));
	auto res = evaluate(f, m);
	EXPECT_TRUE(res.isBool());
	EXPECT_FALSE(res.asBool());
}

TEST(ModelEvaluation, EvaluateBV)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort bvSort = sm.addSort("BitVec", carl::VariableType::VT_UNINTERPRETED);
	sm.makeSortIndexable(bvSort, 1, carl::VariableType::VT_BITVECTOR);
	carl::Variable a = carl::fresh_bitvector_variable("a");
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
	
	auto res = carl::evaluate(f, m);
	std::cout << res << std::endl;
}
