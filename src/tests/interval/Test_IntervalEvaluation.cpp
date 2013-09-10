#include "gtest/gtest.h"
#include "carl/interval/DoubleInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include "carl/interval/IntervalEvaluation.h"

using namespace carl;

TEST(IntervalEvaluation, Monomial)
{
    DoubleInterval ia = DoubleInterval( 1, 4 );
    DoubleInterval ib = DoubleInterval( 2, 5 );
    DoubleInterval ic = DoubleInterval( -2, 3 );
    DoubleInterval id = DoubleInterval( 0, 2 );

    DoubleInterval::evaldoubleintervalmap map;
    VariablePool& vpool = VariablePool::getInstance();
    Variable a = vpool.getFreshVariable();
    vpool.setVariableName(a, "a");
    Variable b = vpool.getFreshVariable();
    vpool.setVariableName(b, "b");
    Variable c = vpool.getFreshVariable();
    vpool.setVariableName(c, "c");
    Variable d = vpool.getFreshVariable();
    vpool.setVariableName(d, "d");
    
    map[a] = ia;
    map[b] = ib;
    map[c] = ic;
    map[d] = id;

    MultivariatePolynomial<cln::cl_RA> e1({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)1*c,(cln::cl_RA)1*d});
    MultivariatePolynomial<cln::cl_RA> e2({(cln::cl_RA)1*a*b,(cln::cl_RA)1*c*d});
    MultivariatePolynomial<cln::cl_RA> e3({(cln::cl_RA)1*a*b*c,(cln::cl_RA)1*d});
    MultivariatePolynomial<cln::cl_RA> e4({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)-1*c,(cln::cl_RA)-1*d});
    MultivariatePolynomial<cln::cl_RA> e5({(cln::cl_RA)1*a,(cln::cl_RA)1*b,Term<cln::cl_RA>(7)});
    MultivariatePolynomial<cln::cl_RA> e6({(cln::cl_RA)12*a,(cln::cl_RA)3*b, (cln::cl_RA)1*Monomial(c,2),(cln::cl_RA)-1*Monomial(d,3)});
//    MultivariatePolynomial<cln::cl_RA> e7({(cln::cl_RA)1*a,(cln::cl_RA)1*b*, Monomial(c,2),(cln::cl_RA)-1*Monomial(d,3)});
    MultivariatePolynomial<cln::cl_RA> e7({a,c});
    e7 = e7.pow(2)*b*d+a;
    
//    e7     = a + b * pow( c + a, 2 ) * d;
    
    DoubleInterval result = IntervalEvaluation::evaluate( e1, map );
    EXPECT_EQ( DoubleInterval( 1, 14 ), result );

    result = IntervalEvaluation::evaluate( e2, map );
    EXPECT_EQ( DoubleInterval( -2, 26 ), result );

    result = IntervalEvaluation::evaluate( e3, map );
    EXPECT_EQ( DoubleInterval( -40, 62 ), result );

    result = IntervalEvaluation::evaluate( e4, map );
    EXPECT_EQ( DoubleInterval( -2, 11 ), result );

    result = IntervalEvaluation::evaluate( e5, map );
    EXPECT_EQ( DoubleInterval( 10, 16 ), result );

    result = IntervalEvaluation::evaluate( e6, map );
    EXPECT_EQ( DoubleInterval( 10,72 ), result );

    result = IntervalEvaluation::evaluate( e7, map );
    EXPECT_EQ( DoubleInterval( -159, 494 ), result );
}


TEST(IntervalEvaluation, Term)
{
}


TEST(IntervalEvaluation, MultivariatePolynomial)
{
}
