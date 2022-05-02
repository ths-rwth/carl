#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/polynomialfunctions/IntervalEvaluation.h"
#include <carl-common/meta/platform.h>

#include "../Common.h"

using namespace carl;

TEST(IntervalEvaluation, Monomial)
{
    Interval<Rational> ia( 1, 4 );
    Interval<Rational> ib( 2, 5 );
    Interval<Rational> ic( -2, 3 );
    Interval<Rational> id( 0, 2 );

    std::map<Variable, Interval<Rational>> map;
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
//    MultivariatePolynomial<Rational> e7({(Rational)1*a,(Rational)1*b*, Monomial(c,2),(Rational)-1*Monomial(d,3)});
    MultivariatePolynomial<Rational> e7({a,c});
    e7 = carl::pow(e7, 2)*b*d+a;

//    e7     = a + b * pow( c + a, 2 ) * d;

    Interval<Rational> result = carl::evaluate( e1, map );
    EXPECT_EQ( Interval<Rational>( 1, 14 ), result );

    result = carl::evaluate( e2, map );
    EXPECT_EQ( Interval<Rational>( -2, 26 ), result );

    result = carl::evaluate( e3, map );
    EXPECT_EQ( Interval<Rational>( -40, 62 ), result );

    result = carl::evaluate( e4, map );
    EXPECT_EQ( Interval<Rational>( -2, 11 ), result );

    result = carl::evaluate( e5, map );
    EXPECT_EQ( Interval<Rational>( 10, 16 ), result );

    result = carl::evaluate( e6, map );
    EXPECT_EQ( Interval<Rational>( 10,72 ), result );

    result = carl::evaluate( e7, map );
    EXPECT_EQ( Interval<Rational>( -159, 494 ), result );
}


TEST(IntervalEvaluation, Term)
{
    std::map<Variable, Interval<Rational>> map;
    Variable a = freshRealVariable("a");
    Variable b = freshRealVariable("b");
	//MultivariatePolynomial<Rational> p({(Rational)1*a*a*b*b});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> p(b,
		{MultivariatePolynomial<Rational>(), MultivariatePolynomial<Rational>(),
		MultivariatePolynomial<Rational>(a*a)});
	Interval<Rational> i1(Rational(0), carl::BoundType::INFTY, Rational(0),
		carl::BoundType::INFTY);
	Interval<Rational> i2(Rational(0), carl::BoundType::INFTY, Rational(0),
		carl::BoundType::INFTY);
	map[a] = i1;
	map[b] = i2;
	auto res = carl::evaluate( p, map );
	
	std::cout << p << " on " << map << " = " << res << std::endl;
}


TEST(IntervalEvaluation, MultivariatePolynomial)
{
}
