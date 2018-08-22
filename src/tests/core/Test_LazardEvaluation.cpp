#include <gtest/gtest.h>


#include "carl/core/polynomialfunctions/LazardEvaluation.h"
#include "carl/core/UnivariatePolynomial.h"

#include "../Common.h"

#ifdef USE_COCOA
TEST(LazardEvaluation, Test)
{
	using Poly = carl::MultivariatePolynomial<Rational>;
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");
	carl::Variable z = carl::freshRealVariable("z");
	
	Poly p = (Poly(x)-Poly(y))*z;
	
	carl::LazardEvaluation<Rational,Poly> le(p);
	
	{
		carl::UnivariatePolynomial<Rational> p(x, std::initializer_list<Rational>{-2, 0, 1});
		carl::Interval<Rational> i(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
		carl::RealAlgebraicNumber<Rational> ran(p, i);
		
		le.substitute(x, ran);
	}
	
	{
		carl::UnivariatePolynomial<Rational> p(y, std::initializer_list<Rational>{-2, 0, 1});
		carl::Interval<Rational> i(Rational(1), carl::BoundType::STRICT, Rational(2), carl::BoundType::STRICT);
		carl::RealAlgebraicNumber<Rational> ran(p, i);
		
		le.substitute(y, ran);
	}
	
	EXPECT_EQ(-Poly(z), le.getLiftingPoly());
}
#endif
