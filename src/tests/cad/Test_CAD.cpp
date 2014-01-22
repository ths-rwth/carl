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
		// p[0] = x^2 + y^2 - 1
		this->p.push_back(new Polynomial({Term<cln::cl_RA>(x)*x, Term<cln::cl_RA>(y)*y, Term<cln::cl_RA>(-1)}));
		// p[1] = x + 1 - y
		this->p.push_back(new Polynomial({Term<cln::cl_RA>(x), -Term<cln::cl_RA>(y), Term<cln::cl_RA>(1)}));
		// p[2] = x - y
		this->p.push_back(new Polynomial({Term<cln::cl_RA>(x), -Term<cln::cl_RA>(y)}));

		// p[0] == 0
		this->c.push_back(Constraint(p[0]->toUnivariatePolynomial(x), Sign::ZERO, {x,y}));
		// p[1] == 0
		this->c.push_back(Constraint(p[1]->toUnivariatePolynomial(x), Sign::ZERO, {x,y}));
		// p[2] == 0
		this->c.push_back(Constraint(p[2]->toUnivariatePolynomial(x), Sign::ZERO, {x,y}));
	}

	virtual void TearDown() {
		for (auto pol: this->p) delete pol;
		this->p.clear();
		this->c.clear();
	}

	bool hasNRValue(const carl::RealAlgebraicNumberPtr<cln::cl_RA> n, cln::cl_RA val) {
		if (n->isNumeric()) return n->value() == val;
		return false;
	}
	bool hasValue(const carl::RealAlgebraicNumberPtr<cln::cl_RA> n, cln::cl_RA val) {
		if (n->isNumeric()) return n->value() == val;
		else {
			carl::RealAlgebraicNumberIRPtr<cln::cl_RA> ir = std::static_pointer_cast<carl::RealAlgebraicNumberIR<cln::cl_RA>>(n);
			return ir->getInterval().contains(val);
		}
	}
	bool hasSqrtValue(const carl::RealAlgebraicNumberPtr<cln::cl_RA> n, cln::cl_RA val) {
		if (n->isNumeric()) return n->value() * n->value() == val;
		else {
			carl::RealAlgebraicNumberIRPtr<cln::cl_RA> ir = std::static_pointer_cast<carl::RealAlgebraicNumberIR<cln::cl_RA>>(n);
			return (ir->getInterval() * ir->getInterval()).contains(val);
		}
	}

	carl::CAD<cln::cl_RA> cad;
	carl::Variable x;
	carl::Variable y;
	std::vector<Polynomial*> p;
	std::vector<Constraint> c;
};

TEST_F(CADTest, Check1)
{
	RealAlgebraicPoint<cln::cl_RA> r;
	std::vector<Constraint> constraints;

	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[1], {x, y});
	constraints.assign({c[0], c[1]});
	this->cad.prepareElimination();
	ASSERT_TRUE(cad.check(constraints, r));
	ASSERT_TRUE(c[0].satisfiedBy(r) && c[1].satisfiedBy(r));
	ASSERT_TRUE((hasNRValue(r[0], -1) && hasNRValue(r[1], 0)) || (hasNRValue(r[0], 0) && hasNRValue(r[1], 1)));
	ASSERT_TRUE(cad.check(constraints, r));
	ASSERT_TRUE(c[0].satisfiedBy(r) && c[1].satisfiedBy(r));
	ASSERT_TRUE((hasNRValue(r[0], -1) && hasNRValue(r[1], 0)) || (hasNRValue(r[0], 0) && hasNRValue(r[1], 1)));
}

TEST_F(CADTest, Check2)
{
	RealAlgebraicPoint<cln::cl_RA> r;
	std::vector<Constraint> constraints;
	cln::cl_RA half = cln::cl_RA(1) / 2;

	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[2], {x, y});
	this->cad.prepareElimination();
	constraints.assign({c[0], c[2]});
	ASSERT_TRUE(cad.check(constraints, r));
	ASSERT_TRUE(c[0].satisfiedBy(r) && c[2].satisfiedBy(r));
	ASSERT_TRUE((hasSqrtValue(r[0], -half) && hasSqrtValue(r[1], -half)) || (hasSqrtValue(r[0], half) && hasSqrtValue(r[1], half)));
	ASSERT_TRUE(cad.check(constraints, r));
	ASSERT_TRUE(c[0].satisfiedBy(r) && c[2].satisfiedBy(r));
}
