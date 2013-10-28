#include "gtest/gtest.h"
#include "carl/interval/DoubleInterval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include <cln/cln.h>
#include "carl/interval/IntervalEvaluation.h"
#include "carl/interval/Contraction.h"

using namespace carl;

template<template<typename> class Operator>
using PolynomialContraction = Contraction<Operator, MultivariatePolynomial<cln::cl_RA>>;

TEST(Contraction, SimpleNewton)
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
    MultivariatePolynomial<cln::cl_RA> e7({a,c});
    e7 = e7.pow(2)*b*d+a;
    
    DoubleInterval resultA, resultB, overapprox;
    PolynomialContraction<SimpleNewton> e1_contractor(e1);
    PolynomialContraction<SimpleNewton> e2_contractor(e2);
    PolynomialContraction<SimpleNewton> e3_contractor(e3);
    PolynomialContraction<SimpleNewton> e4_contractor(e4);
    PolynomialContraction<SimpleNewton> e5_contractor(e5);
    PolynomialContraction<SimpleNewton> e6_contractor(e6);
    PolynomialContraction<SimpleNewton> e7_contractor(e7);
    
    bool split;
    
    split = e1_contractor(map,a,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(DoubleInterval(-10,WEAK_BOUND, 0, WEAK_BOUND), resultA);
    
    split = e1_contractor(map,b,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(DoubleInterval(-9,WEAK_BOUND, 1,WEAK_BOUND), resultA);
    
    split = e1_contractor(map,c,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(DoubleInterval(-11,WEAK_BOUND, -3, WEAK_BOUND), resultA);
    
    split = e1_contractor(map,d,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(DoubleInterval(-12,WEAK_BOUND, -1, WEAK_BOUND), resultA);
    
    split = e6_contractor(map,a,resultA,resultB);
    EXPECT_EQ(split, false);
    overapprox = DoubleInterval(-2.00000000000000177635683940025, WEAK_BOUND, 0.166666666666667406815349750104, WEAK_BOUND);
    EXPECT_EQ(overapprox.isLessOrEqual(resultA) && overapprox.isGreaterOrEqual(resultA), true);
    
    split = e6_contractor(map,b,resultA,resultB);
    EXPECT_EQ(split, false);
    overapprox = DoubleInterval(-19.000000000000007105427357601, WEAK_BOUND, -1.33333333333333214909544039983, WEAK_BOUND);
    EXPECT_EQ(overapprox.isLessOrEqual(resultA) && overapprox.isGreaterOrEqual(resultA), true);
    
    split = e6_contractor(map,c,resultA,resultB);
    EXPECT_EQ(split, true);
    overapprox = DoubleInterval(-1, INFINITY_BOUND, -1.2, WEAK_BOUND);
    EXPECT_EQ(overapprox.isLessOrEqual(resultA) && overapprox.isGreaterOrEqual(resultA), true);
    overapprox = DoubleInterval(3.0625, WEAK_BOUND, 1, INFINITY_BOUND);
    EXPECT_EQ(overapprox.isLessOrEqual(resultB) && overapprox.isGreaterOrEqual(resultB), true);
    
    split = e6_contractor(map,d,resultA,resultB);
    EXPECT_EQ(split, false);
    overapprox = DoubleInterval(2.41666666666666651863693004998, WEAK_BOUND, 1, INFINITY_BOUND);
    EXPECT_EQ(overapprox.isLessOrEqual(resultA) && overapprox.isGreaterOrEqual(resultA), true);
}