#include "gtest/gtest.h"
#include <map>

#include "carl/core/UnivariatePolynomial.h"
#include "carl/formula/model/ran/RealAlgebraicNumber.h"
#include "carl/formula/model/ran/RealAlgebraicPoint.h"

#include "../Common.h"

using namespace carl;


TEST(RealAlgebraicNumber, Evaluation)
{
    Variable y = freshRealVariable("skoY");
    Variable x = freshRealVariable("skoX");

	MultivariatePolynomial<Rational> mpx(x);
	MultivariatePolynomial<Rational> mpy(y);

	MultivariatePolynomial<Rational> lin(Rational(-72)*carl::pow(mpx, 6) + Rational(3024)*carl::pow(mpx, 4) + Rational(-60480)*carl::pow(mpx, 2));
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(y, {
		MultivariatePolynomial<Rational>(0),
		lin,
		MultivariatePolynomial<Rational>(0),
		MultivariatePolynomial<Rational>(60480),
		MultivariatePolynomial<Rational>(0),
		MultivariatePolynomial<Rational>(-3024),
		MultivariatePolynomial<Rational>(0),
		MultivariatePolynomial<Rational>(72),
		MultivariatePolynomial<Rational>(0),
		MultivariatePolynomial<Rational>(-1)
	});

	//UnivariatePolynomial<Rational> px(y, {-3528000, 0, 882000, 0, -74760, 0, 3444, 0, -84, 0, 1});
	UnivariatePolynomial<Rational> px(y, std::initializer_list<Rational>{-3528000, 0, 882000, 0, -74760, 0, 3444, 0, -84, 0, 1});
	Interval<Rational> ix(Rational("2927288666429")/Rational("1099511627776"), BoundType::STRICT, Rational("1463644333215")/Rational("549755813888"), BoundType::STRICT);
	UnivariatePolynomial<Rational> py(y, std::initializer_list<Rational>{Rational("-18289152000"), 0, Rational("4572288000"), 0, Rational("-387555840"), 0, 18156096, 0, -556416, 0, 11232, 0, -144, 0, 1});
	Interval<Rational> iy(Rational(-147580509)/24822226, BoundType::STRICT, Rational(-73113831)/12411113, BoundType::STRICT);

	std::vector<Variable> vars({y, x});
	RealAlgebraicPoint<Rational> point({RealAlgebraicNumber<Rational>(py, iy), RealAlgebraicNumber<Rational>(px, ix)});

	//evaluate(MultivariatePolynomial<Rational>(p), point, vars);

	/* m = {
	 * skoY : (IR ]-147580509/24822226, -73113831/12411113[, (1)*skoY^14 + (-144)*skoY^12 + (11232)*skoY^10 + (-556416)*skoY^8 + (18156096)*skoY^6 + (-387555840)*skoY^4 + (4572288000)*skoY^2 + -18289152000),
	 * skoX : (IR ]85/32, 341/128[, (1)*skoX^10 + (-84)*skoX^8 + (3444)*skoX^6 + (-74760)*skoX^4 + (882000)*skoX^2 + -3528000)
	 * }
	 */
}