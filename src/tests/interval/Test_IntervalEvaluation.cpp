#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/interval/IntervalEvaluation.h"
#include "carl/util/platform.h"

#include "../Common.h"

using namespace carl;

TEST(IntervalEvaluation, Monomial)
{
    Interval<Rational> ia( 1, 4 );
    Interval<Rational> ib( 2, 5 );
    Interval<Rational> ic( -2, 3 );
    Interval<Rational> id( 0, 2 );

    std::map<Variable, Interval<Rational>> map;
    VariablePool& vpool = VariablePool::getInstance();
    Variable a = vpool.getFreshVariable();
    vpool.setName(a, "a");
    Variable b = vpool.getFreshVariable();
    vpool.setName(b, "b");
    Variable c = vpool.getFreshVariable();
    vpool.setName(c, "c");
    Variable d = vpool.getFreshVariable();
    vpool.setName(d, "d");

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
    e7 = e7.pow(2)*b*d+a;

//    e7     = a + b * pow( c + a, 2 ) * d;

    Interval<Rational> result = IntervalEvaluation::evaluate( e1, map );
    EXPECT_EQ( Interval<Rational>( 1, 14 ), result );

    result = IntervalEvaluation::evaluate( e2, map );
    EXPECT_EQ( Interval<Rational>( -2, 26 ), result );

    result = IntervalEvaluation::evaluate( e3, map );
    EXPECT_EQ( Interval<Rational>( -40, 62 ), result );

    result = IntervalEvaluation::evaluate( e4, map );
    EXPECT_EQ( Interval<Rational>( -2, 11 ), result );

    result = IntervalEvaluation::evaluate( e5, map );
    EXPECT_EQ( Interval<Rational>( 10, 16 ), result );

    result = IntervalEvaluation::evaluate( e6, map );
    EXPECT_EQ( Interval<Rational>( 10,72 ), result );

    result = IntervalEvaluation::evaluate( e7, map );
    EXPECT_EQ( Interval<Rational>( -159, 494 ), result );
}


TEST(IntervalEvaluation, Term)
{
}


TEST(IntervalEvaluation, MultivariatePolynomial)
{
}
