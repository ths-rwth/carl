#include "gtest/gtest.h"
#include "carl/interval/DoubleInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include "carl/interval/IntervalEvaluation.h"

using namespace carl;

TEST(IntervalEvaluation, Monomial)
{
    DoubleInterval i0  = DoubleInterval( 0.0, DoubleInterval::STRICT_BOUND, 0.0, DoubleInterval::STRICT_BOUND );
    DoubleInterval i1  = DoubleInterval();
    DoubleInterval i2  = DoubleInterval( 0.0, DoubleInterval::INFINITY_BOUND, 0.0, DoubleInterval::INFINITY_BOUND );
    DoubleInterval i3  = DoubleInterval( 0.0, DoubleInterval::INFINITY_BOUND, 10.0, DoubleInterval::WEAK_BOUND );

    DoubleInterval a0  = DoubleInterval( -2, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a1  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a2  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND );
    DoubleInterval a3  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval a4  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a5  = DoubleInterval( -2, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval a6  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval a7  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval a8  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval b0  = DoubleInterval( -2, DoubleInterval::WEAK_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b1  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 2, DoubleInterval::WEAK_BOUND );
    DoubleInterval b2  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, -1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b3  = DoubleInterval( 1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );

    DoubleInterval b4  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval b5  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval b6  = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b7  = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b8  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 0, DoubleInterval::WEAK_BOUND );
    DoubleInterval b9  = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::WEAK_BOUND );
    DoubleInterval b10 = DoubleInterval( -1, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval b11 = DoubleInterval( 0, DoubleInterval::WEAK_BOUND, 1, DoubleInterval::INFINITY_BOUND );
    DoubleInterval b12 = DoubleInterval( -1, DoubleInterval::INFINITY_BOUND, 1, DoubleInterval::INFINITY_BOUND );

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

//    MultivariatePolynomial<cln::cl_RA> e1({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)1*c,(cln::cl_RA)1*d});
//    MultivariatePolynomial<cln::cl_RA> e2({(cln::cl_RA)1*a*b,(cln::cl_RA)1*c*d});
//    MultivariatePolynomial<cln::cl_RA> e3({(cln::cl_RA)1*a*b*c,(cln::cl_RA)1*d});
//    MultivariatePolynomial<cln::cl_RA> e4({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)-1*c,(cln::cl_RA)-1*d});
//    MultivariatePolynomial<cln::cl_RA> e5({(cln::cl_RA)1*a,(cln::cl_RA)1*b,(cln::cl_RA)3,(cln::cl_RA)4});
//    MultivariatePolynomial<cln::cl_RA> e6({(cln::cl_RA)12*a,(cln::cl_RA)3*b, Monomial(c,2),(cln::cl_RA)-1*Monomial(d,3)});
//    MultivariatePolynomial<cln::cl_RA> e7({(cln::cl_RA)1*a,(cln::cl_RA)1*b*, Monomial(c,2),(cln::cl_RA)-1*Monomial(d,3)});
      MultivariatePolynomial<cln::cl_RA> e7({a,c});
        e7 = e7.pow(2)*b*d+a;
//    e7     = a + b * pow( c + a, 2 ) * d;
    
    
    
    
    
    
}


TEST(IntervalEvaluation, Term)
{
}


TEST(IntervalEvaluation, MultivariatePolynomial)
{
}
