#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <vector>

#include "../../carl/core/logging.h"
#include "../../carl/cad/Constraint.h"
#include "../../carl/interval/Interval.h"

#include "../Common.h"

typedef carl::cad::Constraint<Rational> Constraint;
typedef carl::MultivariatePolynomial<Rational> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> Polynomial;
typedef carl::RealAlgebraicNumberNR<Rational> RANNR;
typedef carl::RealAlgebraicNumberIR<Rational> RANIR;
typedef carl::RealAlgebraicPoint<Rational> RAP;

TEST(Constraint, satisfiedBy)
{
	carl::Variable x = carl::VariablePool::getInstance().getFreshVariable("x");
	carl::Variable y = carl::VariablePool::getInstance().getFreshVariable("y");

	// p1 = x^2 + y^2 - 1
	Polynomial p1(x, {MPolynomial({carl::Term<Rational>(y)*y, carl::Term<Rational>(-1)}), MPolynomial(0), MPolynomial(1)});

	{
		// x^2 + y^2 - 1 @ x=-0.5, y=-1
		// Should be 1/4 != 0
		RAP tmp({RANNR::create(-Rational(1)/2), RANNR::create(-1)});
		///@todo Change to MultivariatePolynomial
		//EXPECT_FALSE(Constraint(p1, carl::Sign::ZERO, {x,y}, false).satisfiedBy(tmp));
	}
}

TEST(Constraint, satisfiedBySqrt)
{
	carl::Variable x = carl::VariablePool::getInstance().getFreshVariable("x");
	carl::Variable y = carl::VariablePool::getInstance().getFreshVariable("y");

	// p1 = x^2 + y^2 - 1
	Polynomial p1(x, {MPolynomial({carl::Term<Rational>(y)*y, carl::Term<Rational>(-1)}), MPolynomial(0), MPolynomial(1)});

	carl::UnivariatePolynomial<Rational> px(x, std::initializer_list<Rational>{-1, 0, 2});
	carl::UnivariatePolynomial<Rational> py(y, std::initializer_list<Rational>{-1, 0, 2});

	carl::Interval<Rational> i(Rational(11)/16, carl::BoundType::STRICT, Rational(3)/4, carl::BoundType::STRICT);

	{
		RAP r({ RANIR::create(px, i), RANIR::create(py, i) });
		///@todo Change to MultivariatePolynomial
		//EXPECT_TRUE(Constraint(p1, carl::Sign::ZERO, {x,y}, false).satisfiedBy(r));
	}
}
