#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/formula/model/mvroot/MultivariateRoot.h"
#include "carl/formula/Formula.h"
#include <carl-model/Model.h>
#include "carl-model/evaluation/ModelEvaluation.h"

#include "../Common.h"

using namespace carl;

template<typename T>
class MultivariateRootTest: public testing::Test {};

TYPED_TEST_CASE(MultivariateRootTest, RationalTypes);

TYPED_TEST(MultivariateRootTest, Constructor)
{
	using Poly = MultivariatePolynomial<TypeParam>;
	Variable x = freshRealVariable("x");
	Variable y = MultivariateRoot<Poly>::var();
	Poly p = x*y + TypeParam(2)*y*y;
	
	MultivariateRoot<Poly> mr(p, 1);
	
	EXPECT_EQ(p, mr.poly());
	EXPECT_EQ(1, mr.k());
	EXPECT_EQ(y, mr.var());
}

TYPED_TEST(MultivariateRootTest, Evaluate)
{
	using Poly = MultivariatePolynomial<TypeParam>;
	using MultiRoot = MultivariateRoot<Poly>;
	Variable x = freshRealVariable("x");
	Variable y = MultivariateRoot<Poly>::var();
	Poly p = x*y + TypeParam(2)*y*y;
	
	MultiRoot mr1(p, 1);
	MultiRoot mr2(p, 2);
	
	EXPECT_EQ(p, mr1.poly());
	EXPECT_EQ(1, mr1.k());
	EXPECT_EQ(y, mr1.var());
	EXPECT_EQ(p, mr2.poly());
	EXPECT_EQ(2, mr2.k());
	EXPECT_EQ(y, mr2.var());

	typename MultiRoot::EvalMap m;
	m.emplace(x, RealAlgebraicNumber<TypeParam>(2));
	EXPECT_EQ(RealAlgebraicNumber<TypeParam>(-1), *mr1.evaluate(m));
	EXPECT_EQ(RealAlgebraicNumber<TypeParam>(0), *mr2.evaluate(m));
}

TYPED_TEST(MultivariateRootTest, Evaluate2)
{
	// (skoSM !> root((-1)*__z*skoSP+__z^2+2*__z+(-1)*skoSP+1, 2, __z = 1)) on 
	// {skoSP = (IR ]-5793/4096, -181/128[, __r^2 + -2 R), skoSM = (NR -1)}
	using Poly = MultivariatePolynomial<TypeParam>;
	using MultiRoot = MultivariateRoot<Poly>;
	using IntervalT = Interval<TypeParam>;
	using RANT = RealAlgebraicNumber<TypeParam>;
	using ModelT = Model<TypeParam,Poly>;
	
	Variable x = freshRealVariable("x");
	Variable y = freshRealVariable("y");
	Variable z = MultivariateRoot<Poly>::var();
	Poly p = -Poly(z)*x+Poly(z)*z+Poly(2)*z-x+Poly(1);
	
	VariableComparison<Poly> vc(y, MultiRoot(p, 2), Relation::GREATER, true);
	Formula<Poly> f(vc);
	
	ModelT m;
	IntervalT rani(TypeParam(-5793)/4096, BoundType::STRICT, TypeParam(-181)/128, BoundType::STRICT);
	UnivariatePolynomial<TypeParam> ranp(x, {TypeParam(-2), TypeParam(0), TypeParam(1)});
	m.assign(x, RANT(ranp, rani));
	m.assign(y, RANT(-1));
	
	auto res = model::evaluate(f, m);
	EXPECT_TRUE(res.isBool() && res.asBool());
}
