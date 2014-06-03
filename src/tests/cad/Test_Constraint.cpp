#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <vector>
#include <cln/cln.h>

#include "../../carl/core/logging.h"
#include "../../carl/core/RealAlgebraicNumber.h"
#include "../../carl/cad/Constraint.h"
#include "../../carl/interval/Interval.h"

typedef carl::cad::Constraint<cln::cl_RA> Constraint;
typedef carl::MultivariatePolynomial<cln::cl_RA> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> Polynomial;
typedef carl::RealAlgebraicNumberNR<cln::cl_RA> RANNR;
typedef carl::RealAlgebraicNumberIR<cln::cl_RA> RANIR;
typedef carl::RealAlgebraicPoint<cln::cl_RA> RAP;

TEST(Constraint, satisfiedBy)
{
	carl::Variable x = carl::VariablePool::getInstance().getFreshVariable("x");
	carl::Variable y = carl::VariablePool::getInstance().getFreshVariable("y");
	
	// p1 = x^2 + y^2 - 1
	Polynomial p1(x, {MPolynomial({carl::Term<cln::cl_RA>(y)*y, carl::Term<cln::cl_RA>(-1)}), MPolynomial(0), MPolynomial(1)});

	{
		// x^2 + y^2 - 1 @ x=-0.5, y=-1
		// Should be 1/4 != 0
		RAP tmp({RANNR::create(-cln::cl_RA(1)/2), RANNR::create(-1)});
		///@todo Change to MultivariatePolynomial
		//EXPECT_FALSE(Constraint(p1, carl::Sign::ZERO, {x,y}, false).satisfiedBy(tmp));
	}
}

TEST(Constraint, satisfiedBySqrt)
{
	carl::Variable x = carl::VariablePool::getInstance().getFreshVariable("x");
	carl::Variable y = carl::VariablePool::getInstance().getFreshVariable("y");
	
	// p1 = x^2 + y^2 - 1
	Polynomial p1(x, {MPolynomial({carl::Term<cln::cl_RA>(y)*y, carl::Term<cln::cl_RA>(-1)}), MPolynomial(0), MPolynomial(1)});

	carl::UnivariatePolynomial<cln::cl_RA> px(x, {-1, 0, 2});
	carl::UnivariatePolynomial<cln::cl_RA> py(y, {-1, 0, 2});
	
	carl::Interval<cln::cl_RA> i(cln::cl_RA(11)/16, carl::BoundType::STRICT, cln::cl_RA(3)/4, carl::BoundType::STRICT);
	
	{
		RAP r({ RANIR::create(px, i), RANIR::create(py, i) });
		///@todo Change to MultivariatePolynomial
		//EXPECT_TRUE(Constraint(p1, carl::Sign::ZERO, {x,y}, false).satisfiedBy(r));
	}
}

