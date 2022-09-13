#include "gtest/gtest.h"
#include <map>

#include "../Common.h"

#include <carl-arith/poly/umvpoly/UnivariatePolynomial.h>
#include <carl-arith/ran/ran.h>

#include <carl-arith/ran/interval/ran_interval_extra.h>


using namespace carl;
using RAN = IntRepRealAlgebraicNumber<Rational>;
using ConstraintT = BasicConstraint<MultivariatePolynomial<Rational>>;

TEST(IsolationAndEvaluation, Comparison) {
	Variable a = fresh_real_variable("a");
	Variable b = fresh_real_variable("b");
	Variable c = fresh_real_variable("c");
	Variable d = fresh_real_variable("d");

	MultivariatePolynomial<Rational> poly(Rational(-11) + Rational(49) * b + Rational(10) * d * a + carl::pow(c, 2) + Rational(-49) * c * b);

	Rational d_value = Rational(0);

	Interval<Rational> i1(Rational("-4") / Rational("1"), BoundType::STRICT, -Rational("-2") / Rational("1"), BoundType::STRICT);
	UnivariatePolynomial<Rational> p1(fresh_real_variable("__r1"), std::initializer_list<Rational>{-11, 1, 1});
	IntRepRealAlgebraicNumber<Rational> c_value = IntRepRealAlgebraicNumber<Rational>::create_safe(p1, i1);

	Interval<Rational> i2(Rational("-1245") / Rational("76832"), BoundType::STRICT, -Rational("-22409") / Rational("1382976"), BoundType::STRICT);
	UnivariatePolynomial<Rational> p2(fresh_real_variable("__r2"), std::initializer_list<Rational>{-11, -1029, -21609});
	IntRepRealAlgebraicNumber<Rational> b_value = IntRepRealAlgebraicNumber<Rational>::create_safe(p2, i2);

	Rational a_value = Rational(-1);


	std::vector<Variable> variableOrder{d, c, b, a};
	std::map<carl::Variable, RAN> varToRANMap;
	varToRANMap[d] = d_value;
	varToRANMap[c] = c_value;
	varToRANMap[b] = b_value;
	varToRANMap[a] = a_value;

	varToRANMap.erase(a);

	bool is_nullified = carl::real_roots(carl::to_univariate_polynomial(poly, a), varToRANMap).is_nullified();
	EXPECT_TRUE(is_nullified);
}
