#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include "carl/interval/IntervalEvaluation.h"

using namespace carl;

TEST(IntervalEvaluation, Monomial)
{
    Interval<cln::cl_RA> ia( 1, 4 );
    Interval<cln::cl_RA> ib( 2, 5 );
    Interval<cln::cl_RA> ic( -2, 3 );
    Interval<cln::cl_RA> id( 0, 2 );

    std::map<Variable, Interval<cln::cl_RA>> map;
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

    MultivariatePolynomial<cln::cl_RA> e1({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)1*c,(cln::cl_RA)1*d});
    MultivariatePolynomial<cln::cl_RA> e2({(cln::cl_RA)1*a*b,(cln::cl_RA)1*c*d});
    MultivariatePolynomial<cln::cl_RA> e3({(cln::cl_RA)1*a*b*c,(cln::cl_RA)1*d});
    MultivariatePolynomial<cln::cl_RA> e4({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)-1*c,(cln::cl_RA)-1*d});
    MultivariatePolynomial<cln::cl_RA> e5({(cln::cl_RA)1*a,(cln::cl_RA)1*b,Term<cln::cl_RA>(7)});
    MultivariatePolynomial<cln::cl_RA> e6({(cln::cl_RA)12*a,(cln::cl_RA)3*b, (cln::cl_RA)1*c*c,(cln::cl_RA)-1*d*d*d});
//    MultivariatePolynomial<cln::cl_RA> e7({(cln::cl_RA)1*a,(cln::cl_RA)1*b*, Monomial(c,2),(cln::cl_RA)-1*Monomial(d,3)});
    MultivariatePolynomial<cln::cl_RA> e7({a,c});
    e7 = e7.pow(2)*b*d+a;
    
//    e7     = a + b * pow( c + a, 2 ) * d;
    
    Interval<cln::cl_RA> result = IntervalEvaluation::evaluate( e1, map );
    EXPECT_EQ( Interval<cln::cl_RA>( 1, 14 ), result );

    result = IntervalEvaluation::evaluate( e2, map );
    EXPECT_EQ( Interval<cln::cl_RA>( -2, 26 ), result );

    result = IntervalEvaluation::evaluate( e3, map );
    EXPECT_EQ( Interval<cln::cl_RA>( -40, 62 ), result );

    result = IntervalEvaluation::evaluate( e4, map );
    EXPECT_EQ( Interval<cln::cl_RA>( -2, 11 ), result );

    result = IntervalEvaluation::evaluate( e5, map );
    EXPECT_EQ( Interval<cln::cl_RA>( 10, 16 ), result );

    result = IntervalEvaluation::evaluate( e6, map );
    EXPECT_EQ( Interval<cln::cl_RA>( 10,72 ), result );

    result = IntervalEvaluation::evaluate( e7, map );
    EXPECT_EQ( Interval<cln::cl_RA>( -159, 494 ), result );
}


TEST(IntervalEvaluation, Term)
{
}


TEST(IntervalEvaluation, MultivariatePolynomial)
{
}
