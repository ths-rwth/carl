#include "gtest/gtest.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/formula/model/mvroot/MultivariateRoot.h"

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
