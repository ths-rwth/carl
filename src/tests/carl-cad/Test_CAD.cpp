#include "gtest/gtest.h"

#include <memory>
#include <list>
#include <vector>

#include "carl/core/logging.h"
#include <carl-cad/CAD.h>
#include <carl-cad/Constraint.h>
#include "carl/util/platform.h"


#include "../Common.h"

using namespace carl;


typedef carl::cad::Constraint<Rational> CadConstraint;

class CADTest : public ::testing::Test {
protected:
	typedef carl::CAD<Rational>::MPolynomial Polynomial;

	CADTest() :
		x(freshRealVariable("x")),
		y(freshRealVariable("y")),
		z(freshRealVariable("z")),
		w(freshRealVariable("w"))
	{
		CARL_LOG_INFO("carl.core", "Variables " << x << ", " << y);
	}
	~CADTest() {}

	virtual void SetUp() {
		// p[0] = x^2 + y^2 - 1
		this->p.push_back(Polynomial({Term<Rational>(x)*x, Term<Rational>(y)*y, Term<Rational>(-1)}));
		// p[1] = x + 1 - y
		this->p.push_back(Polynomial({Term<Rational>(x), -Term<Rational>(y), Term<Rational>(1)}));
		// p[2] = x - y
		this->p.push_back(Polynomial({Term<Rational>(x), -Term<Rational>(y)}));
		// p[3] = x^2 + y^2 + z^2 - 1
		this->p.push_back(Polynomial({Term<Rational>(x)*x, Term<Rational>(y)*y, Term<Rational>(z)*z, Term<Rational>(-1)}));
		// p[4] = x^2 + y^2
		this->p.push_back(Polynomial({Term<Rational>(x)*x, Term<Rational>(y)*y}));
		// p[5] = z^3 - 1/2
		this->p.push_back(Polynomial({Term<Rational>(z)*z*z, Term<Rational>(Rational(-1)/2)}));
		// p[6] = xy - x - y + 1
		this->p.push_back(Polynomial({Term<Rational>(x)*y, Term<Rational>(-1)*x, Term<Rational>(-1)*y, Term<Rational>(1)}));
		// p[7] = x^2 + y^2
		this->p.push_back(Polynomial({Term<Rational>(x)*x, Term<Rational>(y)*y}));
		// p[8] = x^3 + y^3 + z^3 - 1
		this->p.push_back(Polynomial({Term<Rational>(x)*x*x, Term<Rational>(y)*y*y, Term<Rational>(z)*z*z, Term<Rational>(-1)}));
		
		// p[9] = x^2 - 2
		this->p.push_back(Polynomial({Term<Rational>(x)*x, Term<Rational>(Rational(-2))}));
		// p[10] = y^2 - 2
		this->p.push_back(Polynomial({Term<Rational>(y)*y, Term<Rational>(Rational(-2))}));
		// p[11] = z^2 - 2
		this->p.push_back(Polynomial({Term<Rational>(z)*z, Term<Rational>(Rational(-2))}));
		// p[12] = w^2 - 2
		this->p.push_back(Polynomial({Term<Rational>(w)*w, Term<Rational>(Rational(-2))}));
	}

	virtual void TearDown() {
		//for (auto pol: this->p) delete pol;
		this->p.clear();
		this->bounds.clear();
	}

	bool hasNRValue(const carl::RealAlgebraicNumber<Rational> n, Rational val) {
		if (n.isNumeric()) return n.value() == val;
		return false;
	}
	bool hasValue(const carl::RealAlgebraicNumber<Rational> n, Rational val) {
		if (n.isNumeric()) return n.value() == val;
		else {
			return n.getInterval().contains(val);
		}
	}
	bool hasSqrtValue(const carl::RealAlgebraicNumber<Rational> n, Rational val) {
		if (n.isNumeric()) return n.value() * n.value() == val;
		else if(n.isInterval()){
			return (n.getInterval() * n.getInterval()).contains(val);
		}
		else {
		    // can't check this for a thom encoding
		    return true;
		}
	}

	carl::CAD<Rational> cad;
	carl::Variable x, y, z, w;
	std::vector<Polynomial> p;
	carl::CAD<Rational>::BoundMap bounds;
};

TEST_F(CADTest, Check1)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;
	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[1], {x, y});
	cons.assign({
		CadConstraint(this->p[0], Sign::ZERO, {x,y}),
		CadConstraint(this->p[1], Sign::ZERO, {x,y})
	});
	this->cad.prepareElimination();
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
	EXPECT_TRUE((hasNRValue(r[0], -1) && hasNRValue(r[1], 0)) || (hasNRValue(r[0], 0) && hasNRValue(r[1], 1)));
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
	EXPECT_TRUE((hasNRValue(r[0], -1) && hasNRValue(r[1], 0)) || (hasNRValue(r[0], 0) && hasNRValue(r[1], 1)));
}

TEST_F(CADTest, Check2)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;
	Rational half = Rational(1) / 2;

	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[2], {x, y});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[0], Sign::ZERO, {x,y}),
		CadConstraint(this->p[2], Sign::ZERO, {x,y})
	});
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
	EXPECT_TRUE((hasSqrtValue(r[0], -half) && hasSqrtValue(r[1], -half)) || (hasSqrtValue(r[0], half) && hasSqrtValue(r[1], half)));
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check3)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;

	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[2], {x, y});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[0], Sign::POSITIVE, {x,y}),
		CadConstraint(this->p[2], Sign::NEGATIVE, {x,y})
	});
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check4)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;

	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[2], {x, y});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[0], Sign::NEGATIVE, {x,y}),
		CadConstraint(this->p[2], Sign::POSITIVE, {x,y})
	});
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check5)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;

	this->cad.addPolynomial(this->p[0], {x, y});
	this->cad.addPolynomial(this->p[2], {x, y});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[0], Sign::ZERO, {x,y}),
		CadConstraint(this->p[2], Sign::POSITIVE, {x,y})
	});
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check6)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;

	this->cad.addPolynomial(this->p[3], {x, y, z});
	this->cad.addPolynomial(this->p[4], {x, y, z});
	this->cad.addPolynomial(this->p[5], {x, y, z});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[3], Sign::NEGATIVE, {x,y,z}),
		CadConstraint(this->p[4], Sign::POSITIVE, {x,y,z}),
		CadConstraint(this->p[5], Sign::POSITIVE, {x,y,z})
	});
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check7)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;
	
	this->cad.addPolynomial(this->p[7], {x, y});
	this->cad.addPolynomial(this->p[6], {x, y});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[7], Sign::ZERO, {x,y}),
		CadConstraint(this->p[6], Sign::ZERO, {x,y})
	});
	EXPECT_EQ(carl::cad::Answer::False, cad.check(cons, r, this->bounds));
	//for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check8)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;
	
	this->cad.addPolynomial(this->p[7], {x, y, z});
	this->cad.addPolynomial(this->p[8], {x, y, z});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[7], Sign::NEGATIVE, {x,y, z}),
		CadConstraint(this->p[8], Sign::ZERO, {x,y, z})
	});
	EXPECT_EQ(carl::cad::Answer::False, cad.check(cons, r, this->bounds));
	//for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, Check9)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;
	
	this->cad.addPolynomial(this->p[9], {x, y, z, w});
	this->cad.addPolynomial(this->p[10], {x, y, z, w});
	this->cad.addPolynomial(this->p[11], {x, y, z, w});
	this->cad.addPolynomial(this->p[12], {x, y, z, w});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(this->p[9], Sign::NEGATIVE, {x,y,z,w}),
		CadConstraint(this->p[10], Sign::ZERO, {x,y,z,w}),
		CadConstraint(this->p[11], Sign::POSITIVE, {x,y,z,w}),
		CadConstraint(this->p[12], Sign::ZERO, {x,y,z,w})
	});
	EXPECT_EQ(carl::cad::Answer::True, cad.check(cons, r, this->bounds));
	for (auto c: cons) EXPECT_TRUE(c.satisfiedBy(r, cad.getVariables()));
}

TEST_F(CADTest, CheckInt)
{
	RealAlgebraicPoint<Rational> r;
	std::vector<CadConstraint> cons;
	carl::Variable i = carl::freshIntegerVariable("i");

	this->cad.addPolynomial(Polynomial(i), {i});
	this->cad.addPolynomial(Polynomial(Rational(1)-i), {i});
	this->cad.prepareElimination();
	cons.assign({
		CadConstraint(Polynomial(i), Sign::ZERO, {i}),
		CadConstraint(Polynomial(Rational(1)-i), Sign::ZERO, {i})
	});
	//EXPECT_EQ(carl::cad::Answer::False, cad.check(cons, r, this->bounds));
	//std::cout << r << std::endl;
}

template<typename T>
inline carl::RealAlgebraicNumber<Rational> NR(T t, bool b) {
	return carl::RealAlgebraicNumber<Rational>(t, b);
}

TEST(CAD, Samples)
{
	std::list<carl::RealAlgebraicNumber<Rational>> roots({ NR(-1, true), NR(1, true) });

	carl::cad::SampleSet<Rational> currentSamples;
	currentSamples.insert(NR(-1, false));
	currentSamples.insert(NR(0, true));
	currentSamples.insert(NR(1, false));

	std::forward_list<carl::RealAlgebraicNumber<Rational>> replacedSamples;

	carl::Interval<Rational> bounds(0, carl::BoundType::STRICT, 1, carl::BoundType::INFTY);

	carl::CAD<Rational> cad;

	//carl::cad::SampleSet<Rational> res = cad.samples(0, roots, currentSamples, replacedSamples, bounds);

	//std::cout << res << std::endl;
	//EXPECT_TRUE(!res.empty());
}
