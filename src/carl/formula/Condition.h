/**
 * Condition.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2012-06-11
 * @version 2014-10-30
 */

#pragma once

#include <bitset>
#include <assert.h>
#include <stdio.h>
#include <iostream>

static const unsigned CONDITION_SIZE = 64;

namespace carl
{
    class Condition:
        public std::bitset<CONDITION_SIZE>
    {
        public:
            Condition():
                std::bitset<CONDITION_SIZE>()
            {}

            Condition( const std::bitset<CONDITION_SIZE>& _bisset ):
                std::bitset<CONDITION_SIZE>( _bisset )
            {}

            Condition( unsigned i ):
                std::bitset<CONDITION_SIZE>()
            {
                assert( i < CONDITION_SIZE );
                (*this)[i] = 1;
            }

            /**
             * Check whether the bits of this condition are always set if the corresponding bit
             * of the given condition is set.
             *
             * @param _condition The condition to compare with.
             * @return  true,   if all bits of this condition are always set if the corresponding bit
             *                  of the given condition is set;
             *          false,  otherwise.
             */
            bool operator <=( const Condition& _condition ) const
            {
                return (*this & (~_condition)).none();
            }
    };

    static const Condition PROP_TRUE = Condition();

    //Propositions which hold, if they hold for each sub formula of a formula including itself (0-15)
    static const Condition PROP_IS_IN_NNF                       = Condition( 0 );
    static const Condition PROP_IS_IN_CNF                       = Condition( 1 );
    static const Condition PROP_IS_PURE_CONJUNCTION             = Condition( 2 );
    static const Condition PROP_IS_A_CLAUSE                     = Condition( 3 );
    static const Condition PROP_IS_A_LITERAL                    = Condition( 4 );
    static const Condition PROP_IS_AN_ATOM                      = Condition( 5 );
    static const Condition PROP_IS_LITERAL_CONJUNCTION          = Condition( 6 );
    static const Condition STRONG_CONDITIONS                    = PROP_IS_IN_NNF | PROP_IS_IN_CNF | PROP_IS_PURE_CONJUNCTION |
                                                                  PROP_IS_A_CLAUSE | PROP_IS_A_LITERAL | PROP_IS_AN_ATOM | PROP_IS_LITERAL_CONJUNCTION;

    //Propositions which hold, if they hold in at least one sub formula (16-63)
    static const Condition PROP_CONTAINS_EQUATION                = Condition( 16 );
    static const Condition PROP_CONTAINS_INEQUALITY              = Condition( 17 );
    static const Condition PROP_CONTAINS_STRICT_INEQUALITY       = Condition( 18 );
    static const Condition PROP_CONTAINS_LINEAR_POLYNOMIAL       = Condition( 19 );
    static const Condition PROP_CONTAINS_NONLINEAR_POLYNOMIAL    = Condition( 20 );
    static const Condition PROP_CONTAINS_MULTIVARIATE_POLYNOMIAL = Condition( 21 );
    static const Condition PROP_CONTAINS_BOOLEAN                 = Condition( 22 );
    static const Condition PROP_CONTAINS_INTEGER_VALUED_VARS     = Condition( 23 );
    static const Condition PROP_CONTAINS_REAL_VALUED_VARS        = Condition( 24 );
    static const Condition PROP_CONTAINS_UNINTERPRETED_EQUATIONS = Condition( 25 );
    static const Condition PROP_CONTAINS_BITVECTOR               = Condition( 26 );
	static const Condition PROP_CONTAINS_PSEUDOBOOLEAN           = Condition( 27 );
    static const Condition PROP_VARIABLE_DEGREE_GREATER_THAN_TWO  = Condition( 28 );
    static const Condition PROP_VARIABLE_DEGREE_GREATER_THAN_THREE  = Condition( 29 );
    static const Condition PROP_VARIABLE_DEGREE_GREATER_THAN_FOUR = Condition( 30 );
    static const Condition PROP_CONTAINS_WEAK_INEQUALITY       = Condition( 31 );
    static const Condition WEAK_CONDITIONS                       = PROP_CONTAINS_EQUATION | PROP_CONTAINS_INEQUALITY | PROP_CONTAINS_STRICT_INEQUALITY
                                             | PROP_CONTAINS_LINEAR_POLYNOMIAL | PROP_CONTAINS_LINEAR_POLYNOMIAL | PROP_CONTAINS_NONLINEAR_POLYNOMIAL
                                             | PROP_CONTAINS_MULTIVARIATE_POLYNOMIAL | PROP_CONTAINS_INEQUALITY | PROP_CONTAINS_BOOLEAN
                                             | PROP_CONTAINS_REAL_VALUED_VARS | PROP_CONTAINS_INTEGER_VALUED_VARS
                                             | PROP_CONTAINS_UNINTERPRETED_EQUATIONS | PROP_CONTAINS_BITVECTOR | PROP_CONTAINS_PSEUDOBOOLEAN
                                             | PROP_VARIABLE_DEGREE_GREATER_THAN_TWO | PROP_VARIABLE_DEGREE_GREATER_THAN_THREE | PROP_VARIABLE_DEGREE_GREATER_THAN_FOUR;

}     // namespace carl
