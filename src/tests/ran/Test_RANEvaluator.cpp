#include "gtest/gtest.h"
#include <map>

#include <carl/poly/umvpoly/UnivariatePolynomial.h>
#include <carl/ran/ran.h>
#include <carl/ran/interval/ran_interval_extra.h>


#include "../Common.h"

using namespace carl;

using Poly = MultivariatePolynomial<Rational>;
using UPoly = UnivariatePolynomial<Rational>;
using RAN = RealAlgebraicNumber<Rational>;

#ifdef RAN_USE_INTERVAL

TEST(RealAlgebraicNumber, RANEvaluator)
{
	Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    
	Poly p = Poly(x)*Poly(x) - Poly(y) * Poly(y);
	RAN sqrt2 = RAN::create_safe(UPoly(x, {-2,0,1}), Interval<Rational>(0, BoundType::STRICT, 2, BoundType::STRICT));

	ran::RANMap<Rational> map;
	map.emplace(x,sqrt2);
	map.emplace(y,sqrt2);

	ran::interval::ran_evaluator<Rational> eval(p);
	bool res = eval.assign(map);
	EXPECT_TRUE(res);
	EXPECT_EQ(eval.value(), Rational(0));




	ran::interval::ran_evaluator<Rational> eval1(p);
	res = eval1.assign(x, sqrt2);
	EXPECT_FALSE(res);

	auto eval2 = eval1;
	res = eval2.assign(y, sqrt2);
	EXPECT_TRUE(res);
	EXPECT_EQ(eval2.value(), Rational(0));
	EXPECT_FALSE(eval1.has_value());


}

#endif