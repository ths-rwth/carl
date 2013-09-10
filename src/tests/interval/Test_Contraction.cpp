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
    
    DoubleInterval resultA, resultB;
    PolynomialContraction<SimpleNewton> e1_contractor(e1);
    PolynomialContraction<SimpleNewton> e2_contractor(e2);
    PolynomialContraction<SimpleNewton> e3_contractor(e3);
    PolynomialContraction<SimpleNewton> e4_contractor(e4);
    PolynomialContraction<SimpleNewton> e5_contractor(e5);
    PolynomialContraction<SimpleNewton> e6_contractor(e6);
    PolynomialContraction<SimpleNewton> e7_contractor(e7);
    
    e1_contractor(map,a,resultA,resultB);
    EXPECT_EQ();
    e1_contractor(map,b,resultA,resultB);
    EXPECT_EQ();
    e1_contractor(map,c,resultA,resultB);
    EXPECT_EQ();
    e1_contractor(map,d,resultA,resultB);
    EXPECT_EQ();
    
    EXPECT_EQ( DoubleInterval( -159, 494 ), result );
}