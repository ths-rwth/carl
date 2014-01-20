#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <vector>
#include <cln/cln.h>

#include "../../carl/core/logging.h"
#include "../../carl/core/RealAlgebraicNumber.h"
#include "../../carl/cad/Constraint.h"

typedef carl::cad::Constraint<cln::cl_RA> Constraint;
typedef carl::MultivariatePolynomial<cln::cl_RA> MPolynomial;
typedef carl::UnivariatePolynomial<MPolynomial> Polynomial;
typedef carl::RealAlgebraicNumberNR<cln::cl_RA> RANNR;
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
		RAP tmp({new RANNR(-cln::cl_RA(1)/2), new RANNR(-1)});
		EXPECT_FALSE(Constraint(p1, carl::Sign::ZERO, {x,y}, false).satisfiedBy(tmp));
	}
}
