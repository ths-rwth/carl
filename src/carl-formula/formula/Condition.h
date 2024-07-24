/**
 * Condition.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @since 2012-06-11
 * @version 2014-10-30
 */

#pragma once

#include <bitset>
#include <cassert>
#include <cstdio>
#include <iostream>


namespace carl {
    static constexpr std::size_t CONDITION_SIZE = 64;

    class Condition: public std::bitset<CONDITION_SIZE>
    {
        public:
            constexpr Condition():
                std::bitset<CONDITION_SIZE>()
            {}

            constexpr Condition(std::bitset<CONDITION_SIZE> _bitset):
                std::bitset<CONDITION_SIZE>(_bitset)
            {}

            explicit constexpr Condition(std::size_t i):
                std::bitset<CONDITION_SIZE>(static_cast<std::size_t>(1) << i)
            {}
    };
	
	/**
	 * Check whether the bits of one condition are always set if the corresponding bit
	 * of another condition is set. Essentially checks for an implication.
	 *
	 * @param lhs The first condition.
	 * @param rhs The second condition.
	 * @return  true,   if all bits of lhs are set if the corresponding bit
	 *                  of rhs are set;
	 *          false,  otherwise.
	 */
	inline bool operator<=(const Condition& lhs, const Condition& rhs) {
		return (lhs & (~rhs)).none();
	}

    static constexpr Condition PROP_TRUE = Condition();

    //Propositions which hold, if they hold for each sub formula of a formula including itself (0-15)
    static constexpr Condition PROP_IS_IN_NNF                       = Condition( 0 );
    static constexpr Condition PROP_IS_IN_CNF                       = Condition( 1 );
    static constexpr Condition PROP_IS_PURE_CONJUNCTION             = Condition( 2 );
    static constexpr Condition PROP_IS_A_CLAUSE                     = Condition( 3 );
    static constexpr Condition PROP_IS_A_LITERAL                    = Condition( 4 );
    static constexpr Condition PROP_IS_AN_ATOM                      = Condition( 5 );
    static constexpr Condition PROP_IS_LITERAL_CONJUNCTION          = Condition( 6 );
    static constexpr Condition PROP_IS_IN_PNF                       = Condition( 7 );
    static const Condition STRONG_CONDITIONS                    = PROP_IS_IN_NNF | PROP_IS_IN_CNF | PROP_IS_PURE_CONJUNCTION |
                                                                  PROP_IS_A_CLAUSE | PROP_IS_A_LITERAL | PROP_IS_AN_ATOM | PROP_IS_LITERAL_CONJUNCTION |
                                                                  PROP_IS_IN_PNF;

    //Propositions which hold, if they hold in at least one sub formula (16-63)
    static constexpr Condition PROP_CONTAINS_EQUATION                   = Condition( 16 );
    static constexpr Condition PROP_CONTAINS_INEQUALITY                 = Condition( 17 );
    static constexpr Condition PROP_CONTAINS_STRICT_INEQUALITY          = Condition( 18 );
    static constexpr Condition PROP_CONTAINS_LINEAR_POLYNOMIAL          = Condition( 19 );
    static constexpr Condition PROP_CONTAINS_NONLINEAR_POLYNOMIAL       = Condition( 20 );
    static constexpr Condition PROP_CONTAINS_MULTIVARIATE_POLYNOMIAL    = Condition( 21 );
    static constexpr Condition PROP_CONTAINS_BOOLEAN                    = Condition( 22 );
    static constexpr Condition PROP_CONTAINS_INTEGER_VALUED_VARS        = Condition( 23 );
    static constexpr Condition PROP_CONTAINS_REAL_VALUED_VARS           = Condition( 24 );
    static constexpr Condition PROP_CONTAINS_UNINTERPRETED_EQUATIONS    = Condition( 25 );
    static constexpr Condition PROP_CONTAINS_BITVECTOR                  = Condition( 26 );
	static constexpr Condition PROP_CONTAINS_PSEUDOBOOLEAN              = Condition( 27 );
    static constexpr Condition PROP_VARIABLE_DEGREE_GREATER_THAN_TWO    = Condition( 28 );
    static constexpr Condition PROP_VARIABLE_DEGREE_GREATER_THAN_THREE  = Condition( 29 );
    static constexpr Condition PROP_VARIABLE_DEGREE_GREATER_THAN_FOUR   = Condition( 30 );
    static constexpr Condition PROP_CONTAINS_WEAK_INEQUALITY            = Condition( 31 );
    static constexpr Condition PROP_CONTAINS_QUANTIFIER_EXISTS          = Condition( 32 );
    static constexpr Condition PROP_CONTAINS_QUANTIFIER_FORALL          = Condition( 33 );
    static constexpr Condition PROP_CONTAINS_ROOT_EXPRESSION            = Condition( 34 );


    static const Condition WEAK_CONDITIONS                       = PROP_CONTAINS_EQUATION | PROP_CONTAINS_INEQUALITY | PROP_CONTAINS_STRICT_INEQUALITY
                                             | PROP_CONTAINS_LINEAR_POLYNOMIAL | PROP_CONTAINS_LINEAR_POLYNOMIAL | PROP_CONTAINS_NONLINEAR_POLYNOMIAL
                                             | PROP_CONTAINS_MULTIVARIATE_POLYNOMIAL | PROP_CONTAINS_INEQUALITY | PROP_CONTAINS_BOOLEAN
                                             | PROP_CONTAINS_REAL_VALUED_VARS | PROP_CONTAINS_INTEGER_VALUED_VARS
                                             | PROP_CONTAINS_UNINTERPRETED_EQUATIONS | PROP_CONTAINS_BITVECTOR | PROP_CONTAINS_PSEUDOBOOLEAN
                                             | PROP_VARIABLE_DEGREE_GREATER_THAN_TWO | PROP_VARIABLE_DEGREE_GREATER_THAN_THREE | PROP_VARIABLE_DEGREE_GREATER_THAN_FOUR | PROP_CONTAINS_QUANTIFIER_EXISTS | PROP_CONTAINS_QUANTIFIER_FORALL | PROP_CONTAINS_ROOT_EXPRESSION;

}     // namespace carl
