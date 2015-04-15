#include "gtest/gtest.h"
#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/interval/IntervalEvaluation.h"
#include "carl/interval/Contraction.h"
#include "carl/util/platform.h"

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#elif defined(__WIN)
#include <mpirxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

using namespace carl;

template<template<typename> class Operator>
using PolynomialContraction = Contraction<Operator, MultivariatePolynomial<Rational>>;

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

    MultivariatePolynomial<Rational> e1({(Rational)1*a,(Rational)1*b,(Rational)1*c,(Rational)1*d});
    MultivariatePolynomial<Rational> e2({(Rational)1*a*b,(Rational)1*c*d});
    MultivariatePolynomial<Rational> e3({(Rational)1*a*b*c,(Rational)1*d});
    MultivariatePolynomial<Rational> e4({(Rational)1*a,(Rational)1*b,(Rational)-1*c,(Rational)-1*d});
    MultivariatePolynomial<Rational> e5({(Rational)1*a,(Rational)1*b,Term<Rational>(7)});
    MultivariatePolynomial<Rational> e6({(Rational)12*a,(Rational)3*b, (Rational)1*c*c,(Rational)-1*d*d*d});
    MultivariatePolynomial<Rational> e7({a,c});
    e7 = e7.pow(2)*b*d+a;
    
    Interval<double> resultA, resultB, overapprox;
    PolynomialContraction<SimpleNewton> e1_contractor(e1);
    PolynomialContraction<SimpleNewton> e2_contractor(e2);
    PolynomialContraction<SimpleNewton> e3_contractor(e3);
    PolynomialContraction<SimpleNewton> e4_contractor(e4);
    PolynomialContraction<SimpleNewton> e5_contractor(e5);
    PolynomialContraction<SimpleNewton> e6_contractor(e6);
    PolynomialContraction<SimpleNewton> e7_contractor(e7);
	
	MultivariatePolynomial<Rational> test((Rational)1*c*c);
    
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
    EXPECT_EQ(split, false);
    overapprox = Interval<double>(-2.0, BoundType::WEAK, -1.2, BoundType::WEAK); 
	
    EXPECT_EQ(overapprox.upper() >= resultA.upper() && overapprox.lower() <= resultA.lower(), true);
    
    split = e6_contractor(map,d,resultA,resultB);
    EXPECT_EQ(split, false);
    
    EXPECT_EQ(resultA.isEmpty(), true);
    
}
