#include "gtest/gtest.h"
#include <map>

#include "../Common.h"

#include <carl/poly/umvpoly/UnivariatePolynomial.h>
#include <carl/ran/ran.h>
#include <carl/ran/real_roots.h>
#include <carl/ran/interval/ran_interval_extra.h>


using namespace carl;
using RAN = RealAlgebraicNumber<Rational>;
using ConstraintT = BasicConstraint<MultivariatePolynomial<Rational>>;

TEST(IsolationAndEvaluation, Comparison) {
	Variable a = freshRealVariable("a");
	Variable b = freshRealVariable("b");
	Variable c = freshRealVariable("c");
	Variable d = freshRealVariable("d");

	MultivariatePolynomial<Rational> poly(Rational(-11) + Rational(49) * b + Rational(10) * d * a + carl::pow(c, 2) + Rational(-49) * c * b);

	Rational d_value = Rational(0);

	Interval<Rational> i1(Rational("-4") / Rational("1"), BoundType::STRICT, -Rational("-2") / Rational("1"), BoundType::STRICT);
	UnivariatePolynomial<Rational> p1(freshRealVariable("__r1"), std::initializer_list<Rational>{-11, 1, 1});
	RealAlgebraicNumber<Rational> c_value = RealAlgebraicNumber<Rational>::create_safe(p1, i1);

	Interval<Rational> i2(Rational("-1245") / Rational("76832"), BoundType::STRICT, -Rational("-22409") / Rational("1382976"), BoundType::STRICT);
	UnivariatePolynomial<Rational> p2(freshRealVariable("__r2"), std::initializer_list<Rational>{-11, -1029, -21609});
	RealAlgebraicNumber<Rational> b_value = RealAlgebraicNumber<Rational>::create_safe(p2, i2);

	Rational a_value = Rational(-1);


	std::vector<Variable> variableOrder{d, c, b, a};
	std::map<carl::Variable, RAN> varToRANMap;
	varToRANMap[d] = d_value;
	varToRANMap[c] = c_value;
	varToRANMap[b] = b_value;
	varToRANMap[a] = a_value;

/*
	bool b1 = carl::evaluate(ConstraintT(poly, carl::Relation::EQ), varToRANMap);
	bool b3 = carl::evaluate(poly, varToRANMap).is_zero();
	EXPECT_EQ(b1, b3);
*/

/*
	std::cout << "test if poly is not nullified" << std::endl;
	varToRANMap[variableOrder[3]] = Rational(123);
	bool b4= carl::evaluate(ConstraintT(poly, carl::Relation::EQ), varToRANMap);
	EXPECT_FALSE(b4);
*/

	varToRANMap.erase(a);

	bool is_nullified = carl::ran::real_roots(carl::to_univariate_polynomial(poly, a), varToRANMap).is_nullified();
	EXPECT_TRUE(is_nullified);

	/*
	auto isolatedRoots = carl::real_roots(carl::to_univariate_polynomial(poly, variableOrder[3]), varToRANMap);
	std::size_t rootIdx = 0;
	bool b2 = false;
	for (const auto& root : isolatedRoots) {
		rootIdx++;
		if (root == a_value) {
			b2 = true;
		}
	}

	//EXPECT_TRUE(b1);
	//EXPECT_TRUE(b2);
	
	EXPECT_EQ(b1, b2);
	*/
}
