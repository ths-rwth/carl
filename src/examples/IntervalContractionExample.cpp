/* 
 * File:   IntervalContractionExample.cpp
 * Author: stefan
 *
 * Created on September 11, 2013, 4:56 PM
 */

#include <carl-arith/interval/Interval.h>
#include <carl-arith/core/VariablePool.h>
#include <carl-arith/poly/umvpoly/MultivariatePolynomial.h>
#include <carl-arith/poly/umvpoly/functions/IntervalEvaluation.h>
#include <carl-arith/intervalcontraction/Contraction.h>
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
    
    Variable a = fresh_real_variable("a");
    Variable b = fresh_real_variable("b");
    Variable c = fresh_real_variable("c");
    Variable d = fresh_real_variable("d");
    
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

