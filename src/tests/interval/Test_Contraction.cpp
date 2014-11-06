#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
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
    Interval<double> ia = Interval<double>( 1, 4 );
    Interval<double> ib = Interval<double>( 2, 5 );
    Interval<double> ic = Interval<double>( -2, 3 );
    Interval<double> id = Interval<double>( 0, 2 );

    Interval<double>::evalintervalmap map;
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
    MultivariatePolynomial<cln::cl_RA> e7({a,c});
    e7 = e7.pow(2)*b*d+a;
    
    Interval<double> resultA, resultB, overapprox;
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
    EXPECT_EQ(Interval<double>::emptyInterval(), resultA);
    
    split = e1_contractor(map,b,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(Interval<double>::emptyInterval(), resultA);
    
    split = e1_contractor(map,c,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(Interval<double>::emptyInterval(), resultA);
    
    split = e1_contractor(map,d,resultA,resultB);
    EXPECT_EQ(split, false);
    EXPECT_EQ(Interval<double>::emptyInterval(), resultA);
    
    split = e6_contractor(map,a,resultA,resultB);
    EXPECT_EQ(split, false);
    overapprox = Interval<double>(-2.00000000000000177635683940025, BoundType::WEAK, 0.166666666666667406815349750104, BoundType::WEAK);
    EXPECT_EQ(overapprox.lower() <= resultA.lower() && overapprox.upper() >= resultA.upper(), true);
    
    /*
    split = e6_contractor(map,b,resultA,resultB);
    EXPECT_EQ(split, false);
    overapprox = Interval<double>(-19.000000000000007105427357601, BoundType::WEAK, -1.33333333333333214909544039983, BoundType::WEAK);
    EXPECT_EQ(overapprox.lower() <= resultA.lower() && overapprox.upper() >= resultA.upper(), true);
    */
    
    split = e6_contractor(map,c,resultA,resultB);
    EXPECT_EQ(split, true);
    overapprox = Interval<double>(-1.0, BoundType::INFTY, -1.2, BoundType::WEAK);
    
    EXPECT_EQ(overapprox.upper() >= resultA.upper(), true);
    overapprox = Interval<double>(3.0625, BoundType::WEAK, 1.0, BoundType::INFTY);
    EXPECT_EQ(overapprox.lower() <= resultB.lower() && overapprox.upper() >= resultB.upper(), true);
    
    split = e6_contractor(map,d,resultA,resultB);
    EXPECT_EQ(split, false);
    overapprox = Interval<double>(2.41666666666666651863693004998, BoundType::WEAK, 1.0, BoundType::INFTY);
    EXPECT_EQ(overapprox.lower() <= resultA.lower() && overapprox.upper() >= resultA.upper(), true);
    
}