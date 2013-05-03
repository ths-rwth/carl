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
     *  The general-purpose monomials. Notice that we aim to keep this object as small as possbible, 
     * while also limiting the use of expensive language features such as RTTI, exceptions and even 
     * polymorphism.
     */
    template<typename Coefficient>
    class Monomial
    {
        protected:
            /// The coefficient of the monomial..
            Coefficient mCoefficient;
            /// A vector of variable-exponent vars (v_i^e_i) with nonzero exponents. 
            std::vector<VarExpPair> mExponents;
            /// Some applications performance depends on getting the degree of monomials very fast
            exponent mTotalDegree;
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
            Monomial(const Coefficient& c, Variable v, exponent e=1);
            /**
             * 
             * @param c
             * @param varexp
             */
            Monomial(const Coefficient& c, const VarExpPair& varexp);
            
            /*
             * Basic operations.
             */
            
            
            /**
             *
             */
            template <typename C>
            friend std::ostream& operator <<( std::ostream& os, const Monomial<C>& rhs );
    };
}

#include "Monomial.tpp"
