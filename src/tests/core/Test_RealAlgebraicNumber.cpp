#include "gtest/gtest.h"

#include <cln/cln.h>
#include <map>

#include "carl/core/RealAlgebraicNumber.h"
#include "carl/core/RealAlgebraicNumberNR.h"
#include "carl/core/RealAlgebraicNumberIR.h"
#include "carl/core/RealAlgebraicNumberEvaluation.h"
#include "carl/core/RealAlgebraicPoint.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/MultivariatePolynomial.h"

using namespace carl;

TEST(RealAlgebraicNumber, Evaluation)
{
	VariablePool& vpool = VariablePool::getInstance();
    Variable y = vpool.getFreshVariable("skoY");
    Variable x = vpool.getFreshVariable("skoX");
	
	MultivariatePolynomial<cln::cl_RA> mpx(x);
	MultivariatePolynomial<cln::cl_RA> mpy(y);
	
	MultivariatePolynomial<cln::cl_RA> lin(cln::cl_RA(-72)*mpx.pow(6) + cln::cl_RA(3024)*mpx.pow(4) + cln::cl_RA(-60480)*mpx.pow(2));
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> p(y, {
		MultivariatePolynomial<cln::cl_RA>(0), 
		lin, 
		MultivariatePolynomial<cln::cl_RA>(0), 
		MultivariatePolynomial<cln::cl_RA>(60480), 
		MultivariatePolynomial<cln::cl_RA>(0), 
		MultivariatePolynomial<cln::cl_RA>(-3024), 
		MultivariatePolynomial<cln::cl_RA>(0), 
		MultivariatePolynomial<cln::cl_RA>(72), 
		MultivariatePolynomial<cln::cl_RA>(0), 
		MultivariatePolynomial<cln::cl_RA>(-1)
	});
	
	//UnivariatePolynomial<cln::cl_RA> px(y, {-3528000, 0, 882000, 0, -74760, 0, 3444, 0, -84, 0, 1});
	UnivariatePolynomial<cln::cl_RA> px(y, {-3528000, 0, 882000, 0, -74760, 0, 3444, 0, -84, 0, 1});
	Interval<cln::cl_RA> ix(cln::cl_RA(2927288666429)/1099511627776, BoundType::STRICT, cln::cl_RA(1463644333215)/549755813888, BoundType::STRICT);
	UnivariatePolynomial<cln::cl_RA> py(y, {-18289152000, 0, 4572288000, 0, -387555840, 0, 18156096, 0, -556416, 0, 11232, 0, -144, 0, 1});
	Interval<cln::cl_RA> iy(cln::cl_RA(-147580509)/24822226, BoundType::STRICT, cln::cl_RA(-73113831)/12411113, BoundType::STRICT);
	
	std::vector<Variable> vars({y, x});
	RealAlgebraicPoint<cln::cl_RA> point({RealAlgebraicNumberIR<cln::cl_RA>::create(py, iy), RealAlgebraicNumberIR<cln::cl_RA>::create(px, ix)});
	
	//RealAlgebraicNumberEvaluation::evaluate(MultivariatePolynomial<cln::cl_RA>(p), point, vars);
	
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
	
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> mp(t, {
		MultivariatePolynomial<cln::cl_RA>(-3), 
		MultivariatePolynomial<cln::cl_RA>(0), 
		MultivariatePolynomial<cln::cl_RA>(1)
	});
	UnivariatePolynomial<cln::cl_RA> p(t, {-3, 0, 1});
	std::vector<Variable> vars({t});
	Interval<cln::cl_RA> i(cln::cl_RA(13)/8, BoundType::STRICT, cln::cl_RA(7)/4, BoundType::STRICT);
	RealAlgebraicPoint<cln::cl_RA> point({RealAlgebraicNumberIR<cln::cl_RA>::create(p, i)});
	auto res = RealAlgebraicNumberEvaluation::evaluate(MultivariatePolynomial<cln::cl_RA>(mp), point, vars);
	std::cerr << res << std::endl;
}