/* 
 * File:   IntervalContractionExample.cpp
 * Author: stefan
 *
 * Created on September 11, 2013, 4:56 PM
 */

#include "carl/interval/Interval.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/polynomialfunctions/IntervalEvaluation.h"
#include "carl/intervalcontraction/Contraction.h"
#include <chrono>
#include <set>

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif


using namespace carl;

template<template<typename> class Operator>
using PolynomialContraction = Contraction<Operator, MultivariatePolynomial<Rational>>;

using DoubleInterval = Interval<double>;

/*
 * 
 */
int main(int argc, char** argv) {

    DoubleInterval ia = DoubleInterval( 1, 4 );
    DoubleInterval ib = DoubleInterval( 2, 5 );
    DoubleInterval ic = DoubleInterval( -2, 3 );
    DoubleInterval id = DoubleInterval( 0, 2 );
    DoubleInterval resA, resB;

    DoubleInterval::evalintervalmap map;
    
    Variable a = freshRealVariable("a");
    Variable b = freshRealVariable("b");
    Variable c = freshRealVariable("c");
    Variable d = freshRealVariable("d");
    
    map[a] = ia;
    map[b] = ib;
    map[c] = ic;
    map[d] = id;

    MultivariatePolynomial<Rational> e6({(Rational)12*a,(Rational)3*b, (Rational)1*createMonomial(c,2),(Rational)-1*createMonomial(d,3)});
    PolynomialContraction<SimpleNewton> contractor(e6);
    
    std::set<Variable> variables = {a,b,c,d};
    
    typedef std::chrono::high_resolution_clock clock;
    ///
    typedef std::chrono::microseconds timeunit;

    clock::time_point start = clock::now();
    
    for( auto variableIt = variables.begin(); variableIt != variables.end(); ++variableIt )
    {
        int count = 0;
        while (count < 100000)
        {
            contractor(map,*variableIt,resA,resB);
            ++count;
        }
    }
    
    std::cout << "Total time: " << std::chrono::duration_cast<timeunit>( clock::now() - start ).count()/1000 << std::endl;
    
    
    return 0;
}

