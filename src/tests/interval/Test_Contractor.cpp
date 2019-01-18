#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/interval/IntervalEvaluation.h"
#include "carl/interval/Contractor.h"
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;

TEST(Evaluation, Basic)
{
    Interval<double> ia = Interval<double>( 1, 4 );
    Interval<double> ib = Interval<double>( 2, 5 );
    Interval<double> ic = Interval<double>( -2, 3 );
    Interval<double> id = Interval<double>( 0, 2 );

    Interval<double>::evalintervalmap map;
	Variable a = freshRealVariable("a");
	Variable b = freshRealVariable("b");
	Variable c = freshRealVariable("c");
	Variable d = freshRealVariable("d");
    
    map[a] = ia;
    map[b] = ib;
    map[c] = ic;
    map[d] = id;

    MultivariatePolynomial<Rational> e1({(Rational)1*a,(Rational)1*b,(Rational)1*c,(Rational)1*d});
    MultivariatePolynomial<Rational> e2({(Rational)1*a*b,(Rational)1*c*d});
    MultivariatePolynomial<Rational> e3({(Rational)1*a*b*c,(Rational)1*d});
    MultivariatePolynomial<Rational> e4({(Rational)1*a,(Rational)1*b,(Rational)-1*c,(Rational)-1*d});
    MultivariatePolynomial<Rational> e5({(Rational)1*a,(Rational)1*b,Term<Rational>(7)});
    MultivariatePolynomial<Rational> e6({(Rational)12*a,(Rational)3*b, (Rational)1*c*c,(Rational)-1*d*d*d});
    MultivariatePolynomial<Rational> e7({a,c});

	using Evaluation = contractor::Evaluation<MultivariatePolynomial<Rational>>;
	using V = std::vector<Interval<double>>;

	EXPECT_EQ(Evaluation(e1, a).evaluate(map), V({ Interval<double>(-10, 0) }));
	EXPECT_EQ(Evaluation(e1, b).evaluate(map), V({ Interval<double>(-9, 1) }));
	EXPECT_EQ(Evaluation(e1, c).evaluate(map), V({ Interval<double>(-11, -3) }));
	EXPECT_EQ(Evaluation(e1, d).evaluate(map), V({ Interval<double>(-12, -1) }));

	EXPECT_EQ(Evaluation(e2, a).evaluate(map), V({ Interval<double>(-3, 2) }));
	EXPECT_EQ(Evaluation(e2, b).evaluate(map), V({ Interval<double>(-6, 4) }));
	EXPECT_EQ(Evaluation(e2, c).evaluate(map), V({ Interval<double>(0, BoundType::INFTY, -1, BoundType::WEAK) }));
	//EXPECT_EQ(Evaluation(e2, d).evaluate(map), V({ Interval<double>(-10, 0) }));

	EXPECT_EQ(Evaluation(e3, a).evaluate(map), V({ Interval<double>(0, BoundType::INFTY, 0, BoundType::INFTY) }));
	EXPECT_EQ(Evaluation(e3, b).evaluate(map), V({ Interval<double>(0, BoundType::INFTY, 0, BoundType::INFTY) }));
	EXPECT_EQ(Evaluation(e3, c).evaluate(map), V({ Interval<double>(-1, 0) }));
	EXPECT_EQ(Evaluation(e3, d).evaluate(map), V({ Interval<double>(-60, 40) }));

	EXPECT_EQ(Evaluation(e4, a).evaluate(map), V({ Interval<double>(-7, 3) }));
	EXPECT_EQ(Evaluation(e4, b).evaluate(map), V({ Interval<double>(-6, 4) }));
	EXPECT_EQ(Evaluation(e4, c).evaluate(map), V({ Interval<double>(1, 9) }));
	EXPECT_EQ(Evaluation(e4, d).evaluate(map), V({ Interval<double>(0, 11) }));

	EXPECT_EQ(Evaluation(e5, a).evaluate(map), V({ Interval<double>(-12, -9) }));
	EXPECT_EQ(Evaluation(e5, b).evaluate(map), V({ Interval<double>(-11, -8) }));

	//EXPECT_EQ(Evaluation(e6, a).evaluate(map), V({ Interval<double>(-10, 0) }));
	//EXPECT_EQ(Evaluation(e6, b).evaluate(map), V({ Interval<double>(-10, 0) }));
	EXPECT_EQ(Evaluation(e6, c).evaluate(map), V());
	//EXPECT_EQ(Evaluation(e6, d).evaluate(map), V({ Interval<double>(-10, 0) }));
}
