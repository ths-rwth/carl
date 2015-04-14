#include "gtest/gtest.h"
#include <map>

#include "carl/core/RealAlgebraicNumber.h"
#include "carl/core/RealAlgebraicNumberNR.h"
#include "carl/core/RealAlgebraicNumberIR.h"
#include "carl/core/RealAlgebraicNumberEvaluation.h"
#include "carl/core/RealAlgebraicPoint.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/util/platform.h"

using namespace carl;

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#elif defined(__VS)
#include <mpirxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

TEST(RealAlgebraicNumber, Evaluation)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable y = vpool.getFreshVariable("skoY");
    Variable x = vpool.getFreshVariable("skoX");
	
	MultivariatePolynomial<Rational> mpx(x);
	MultivariatePolynomial<Rational> mpy(y);
	
	MultivariatePolynomial<Rational> lin(Rational(-72)*mpx.pow(6) + Rational(3024)*mpx.pow(4) + Rational(-60480)*mpx.pow(2));
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
	UnivariatePolynomial<Rational> px(y, {-3528000, 0, 882000, 0, -74760, 0, 3444, 0, -84, 0, 1});
	Interval<Rational> ix(Rational(2927288666429)/1099511627776, BoundType::STRICT, Rational(1463644333215)/549755813888, BoundType::STRICT);
	UnivariatePolynomial<Rational> py(y, {-18289152000, 0, 4572288000, 0, -387555840, 0, 18156096, 0, -556416, 0, 11232, 0, -144, 0, 1});
	Interval<Rational> iy(Rational(-147580509)/24822226, BoundType::STRICT, Rational(-73113831)/12411113, BoundType::STRICT);
	
	std::vector<Variable> vars({y, x});
	RealAlgebraicPoint<Rational> point({RealAlgebraicNumberIR<Rational>::create(py, iy), RealAlgebraicNumberIR<Rational>::create(px, ix)});
	
	//RealAlgebraicNumberEvaluation::evaluate(MultivariatePolynomial<Rational>(p), point, vars);
	
	/* m = {
	 * skoY : (IR ]-147580509/24822226, -73113831/12411113[, (1)*skoY^14 + (-144)*skoY^12 + (11232)*skoY^10 + (-556416)*skoY^8 + (18156096)*skoY^6 + (-387555840)*skoY^4 + (4572288000)*skoY^2 + -18289152000), 
	 * skoX : (IR ]85/32, 341/128[, (1)*skoX^10 + (-84)*skoX^8 + (3444)*skoX^6 + (-74760)*skoX^4 + (882000)*skoX^2 + -3528000)
	 * }
	 */
}

TEST(RealAlgebraicNumber, Evaluation2)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable t = vpool.getFreshVariable("t");
	
	UnivariatePolynomial<MultivariatePolynomial<Rational>> mp(t, {
		MultivariatePolynomial<Rational>(-3), 
		MultivariatePolynomial<Rational>(0), 
		MultivariatePolynomial<Rational>(1)
	});
	UnivariatePolynomial<Rational> p(t, {-3, 0, 1});
	std::vector<Variable> vars({t});
	Interval<Rational> i(Rational(13)/8, BoundType::STRICT, Rational(7)/4, BoundType::STRICT);
	RealAlgebraicPoint<Rational> point({RealAlgebraicNumberIR<Rational>::create(p, i)});
	auto res = RealAlgebraicNumberEvaluation::evaluate(MultivariatePolynomial<Rational>(mp), point, vars);
	std::cerr << res << std::endl;
}