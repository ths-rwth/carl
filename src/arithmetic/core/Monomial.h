/**
 * @file Monomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include "Variable.h"
#include "CompareResult.h"
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
            //
            // Constructors
            //
            /**
             */
            Monomial();
            /**
             * Generate a constant
             * @param c The value of the constant.
             */
            explicit Monomial(const Coefficient& c);
            /**
             * Generate a monomial from a variable, with constant 1 (one).
             * @param v The variable.
             * @param e The exponent.
             */
            explicit Monomial(Variable v, exponent e=1);
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
            
            const Coefficiet& coeff() const;
            exponent tdeg() const;
            
            bool isConstant() const;
            bool isLinear() const;
            unsigned nrVariables() const;
            
            
            ///////////////////////////
            // Orderings
            ///////////////////////////
            
            static CompareResult compareLexical();
            static CompareResult compareGradedLexical();
            static CompareResult compareGradedReverseLexical();
            
            ///////////////////////////
            // Operators
            ///////////////////////////
            
            // Compare operators
            template<typename C1, typename C2>
            friend operator==(const Monomial<C1>& lhs, const Monomial<C2>& rhs);
            template<typename C>
            friend operator==(const Monomial<C>& lhs, const C& rhs);
            template<typename C>
            friend operator==(const C& lhs, const Monomial& rhs);
            template<typename C>
            friend operator==(const Monomial<C>& lhs, const Variable& rhs);
            template<typename C>
            friend operator==(const Variable& lhs, const Monomial& rhs);
            
            template<typename C1, typename C2>
            friend operator!=(const Monomial<C1>& lhs, const Monomial<C2>& rhs);
            template<typename C>
            friend operator!=(const Monomial<C>& lhs, const C& rhs);
            template<typename C>
            friend operator!=(const C& lhs, const Monomial& rhs);
            template<typename C>
            friend operator!=(const Monomial<C>& lhs, const Variable& rhs);
            template<typename C>
            friend operator!=(const Variable& lhs, const Monomial& rhs);
            
            // For assignment, we provide constructors.
            
            // Addition and substraction make only very limited sense, we do not support that.            
            // Multiplication and dividing
            
            
            // Output
            template <typename C>
            friend std::ostream& operator <<( std::ostream& os, const Monomial<C>& rhs );
    };
}

#include "Monomial.tpp"
