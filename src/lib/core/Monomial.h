/**
 * @file Monomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include "Variable.h"
#include "VarExpPair.h"

namespace arithmetic
{   
    /**
     *  The general-purpose multivariate polynomial class.
     */
    template<typename Coefficient>
    class Monomial
    {
        protected:
            /// The coefficient of the monomial..
            Coefficient mCoefficient;
            /// A vector of variable-exponent vars (v_i^e_i) with nonzero exponents. 
            std::vector<VarExpPair> mExponents;
        public:
            /**
             */
            Monomial();
            /**
             * Generate a constant
             * @param c The value of the constant.
             */
            Monomial(const Coefficient& c);
            /**
             * A monomial over one variable.
             * @param c The coefficient for the monomial.
             * @param v The variable
             * @param e The exponent of this variable
             */
            Monomial(const Coefficient& c, variable v, exponent e=1);
            
            Monomial(const Coefficient& c, const VarExpPair& varexp);

            
    };
}

#include "Monomial.tpp"
