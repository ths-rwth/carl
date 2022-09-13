#include <gtest/gtest.h>


#include <carl-arith/ran/interval/helper/LazardEvaluation.h>
#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>

#include "../Common.h"

#ifdef USE_COCOA
TEST(LazardEvaluation, Test)
{
	using Poly = carl::MultivariatePolynomial<Rational>;
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");
	carl::Variable z = carl::fresh_real_variable("z");
	
	Poly p = (Poly(x)-Poly(y))*z;
	
	carl::ran::interval::LazardEvaluation<Rational,Poly> le(p);
	
	{
		carl::UnivariatePolynomial<Rational> p(x, std::initializer_list<Rational>{-2, 0, 1});
		carl::Interval<Rational> i(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
		carl::IntRepRealAlgebraicNumber<Rational> ran = carl::IntRepRealAlgebraicNumber<Rational>::create_safe(p, i);
		
		le.substitute(x, ran);
	}
	
	{
		carl::UnivariatePolynomial<Rational> p(y, std::initializer_list<Rational>{-2, 0, 1});
		carl::Interval<Rational> i(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
		carl::IntRepRealAlgebraicNumber<Rational> ran = carl::IntRepRealAlgebraicNumber<Rational>::create_safe(p, i);
		
		le.substitute(y, ran);
	}
	
	EXPECT_EQ(-Poly(z), le.getLiftingPoly());
}

TEST(LazardEvaluation, Test2)
{
	using Poly = carl::MultivariatePolynomial<Rational>;
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");
	carl::Variable z = carl::fresh_real_variable("z");
	
	Poly p = (Poly(y)*Poly(y)-Poly(x))*z;
	
	carl::ran::interval::LazardEvaluation<Rational,Poly> le(p);
	
	{
		carl::UnivariatePolynomial<Rational> p(x, std::initializer_list<Rational>{-2, 0, 1});
		carl::Interval<Rational> i(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
		carl::IntRepRealAlgebraicNumber<Rational> ran = carl::IntRepRealAlgebraicNumber<Rational>::create_safe(p, i);
		
		le.substitute(x, ran);
	}
	
	{
		carl::UnivariatePolynomial<Rational> p(y, std::initializer_list<Rational>{-2, 0, 0, 0, 1});
		carl::Interval<Rational> i(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
		carl::IntRepRealAlgebraicNumber<Rational> ran = carl::IntRepRealAlgebraicNumber<Rational>::create_safe(p, i);
		
		le.substitute(y, ran);
	}
	
	std::cout << le.getLiftingPoly() << std::endl;
}

class LazardTest : public ::testing::Test {
protected:
	using Poly = carl::MultivariatePolynomial<Rational>;
	carl::Variable x, y, z;

	LazardTest() :
		x(carl::fresh_real_variable("x")),
		y(carl::fresh_real_variable("y")),
		z(carl::fresh_real_variable("z"))
	{
		CARL_LOG_INFO("carl.core", "Variables " << x << ", " << y);
	}
	~LazardTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
	
	auto getRAN(std::initializer_list<Rational> coeffs, Rational lower, Rational upper) {
		carl::UnivariatePolynomial<Rational> p(y, coeffs);
		carl::Interval<Rational> i(lower, carl::BoundType::STRICT, upper, carl::BoundType::STRICT);
		return carl::IntRepRealAlgebraicNumber<Rational>::create_safe(p, i);
	}
};

TEST_F(LazardTest, Proper1) {
	auto ax = getRAN({-2, 0, 1}, 1, 2);
	auto ay = getRAN({-2, 0, 1}, 1, 2);
	auto q = (Poly(x)-y)*z;
	
	carl::ran::interval::LazardEvaluation<Rational,Poly> le(q);
	le.substitute(x, ax);
	le.substitute(y, ay);
	EXPECT_EQ(-Poly(z), le.getLiftingPoly());
}

TEST_F(LazardTest, Proper2) {
	auto ax = getRAN({6, 0, -24, 0, 16}, 1, 2);
	auto ay = getRAN({6, 0, -6, 0, 1}, 2, 3);
	auto q = (Poly(2)*x-y)*z;
	
	carl::ran::interval::LazardEvaluation<Rational,Poly> le(q);
	le.substitute(x, ax);
	le.substitute(y, ay);
	EXPECT_EQ(-Poly(z), le.getLiftingPoly());
}

TEST_F(LazardTest, Proper3) {
	auto ax = getRAN({-2, 0, 1}, 1, 2);
	auto ay = getRAN({-2, 0, 0, 0, 1}, 1, 2);
	auto q = (Poly(x)-Poly(y)*y)*z;
	
	carl::ran::interval::LazardEvaluation<Rational,Poly> le(q);
	le.substitute(x, ax);
	le.substitute(y, ay);
	EXPECT_EQ(-Poly(z), le.getLiftingPoly());
}

#endif
