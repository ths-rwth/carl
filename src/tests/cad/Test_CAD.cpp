#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <vector>
#include <cln/cln.h>

#include "carl/core/logging.h"
#include "carl/cad/CAD.h"
#include "carl/cad/Constraint.h"

using namespace carl;

typedef carl::cad::Constraint<cln::cl_RA> Constraint;

class CADTest : public ::testing::Test {
protected:
	typedef carl::CAD<cln::cl_RA>::MPolynomial Polynomial;
	
	CADTest() : 
		x(carl::VariablePool::getInstance().getFreshVariable("x")),
		y(carl::VariablePool::getInstance().getFreshVariable("y"))
	{
		LOGMSG_INFO("carl.core", "Variables " << x << ", " << y);
	}
	~CADTest() {}
	
	virtual void SetUp() {
		this->p1 = new Polynomial({Term<cln::cl_RA>(x)*x, Term<cln::cl_RA>(y)*y, Term<cln::cl_RA>(-1)});
		this->p2 = new Polynomial({Term<cln::cl_RA>(x), -Term<cln::cl_RA>(y)});
		this->cad.clear();
		this->cad.addPolynomial(this->p1, {x, y});
		this->cad.addPolynomial(this->p2, {x, y});
		this->cad.prepareElimination();
	}
	
	virtual void TearDown() {
		delete this->p1;
		delete this->p2;
	}
	
	carl::CAD<cln::cl_RA> cad;
	carl::Variable x;
	carl::Variable y;
	Polynomial* p1;
	Polynomial* p2;
};

TEST_F(CADTest, Check)
{
	RealAlgebraicPoint<cln::cl_RA> r;
	std::vector<Constraint> constraints;
	constraints.push_back(Constraint(p1->toUnivariatePolynomial(x), Sign::ZERO, cad.getVariables()));
	constraints.push_back(Constraint(p2->toUnivariatePolynomial(x), Sign::ZERO, cad.getVariables()));
	ASSERT_TRUE(cad.check(constraints, r));
	ASSERT_TRUE(cad.check(constraints, r));
}
