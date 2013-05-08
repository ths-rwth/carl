/**
 * @file Monomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include <algorithm>
#include <list>

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
    class Monomial
    {
    private:
        /**
         * Set if we want to use binary search instead of linear search.
         */
        static const bool PREFERBINARYSEARCH = false;

    protected:
        /// A vector of variable-exponent vars (v_i^e_i) with nonzero exponents. 
        std::vector<VarExpPair> mExponents;
        /// Some applications performance depends on getting the degree of monomials very fast
        exponent mTotalDegree = 0;
        
        typedef std::vector<VarExpPair>::iterator exponents_it;
        typedef std::vector<VarExpPair>::const_iterator exponents_cIt;
    public:
        //
        // Constructors
        //
        /**
         */
        Monomial() = default;
        /**
         * Generate a monomial from a variable, with constant 1 (one).
         * @param v The variable.
         * @param e The exponent.
         */
        explicit Monomial(Variable v, exponent e=1);
        
        Monomial& operator=(const Monomial& rhs);
        /**
         * Gives the total degree, i.e. the sum of all exponents.
         * @return 
         */
        exponent tdeg() const
        {
            return mTotalDegree;
        }
        /**
         * Checks whether the monomial is a constant.
         * @return 
         */
        bool isConstant() const
        {
            return mTotalDegree == 0;
        }
        /**
         * Checks whether the monomial has exactly the degree one.
         * @return 
         */
        bool isLinear() const
        {
            return mTotalDegree == 1;
        }
        bool isAtMostLinear() const
        {
            return mTotalDegree <= 1;
        }
        size_t nrVariables() const
        {
            return mExponents.size();
        }

        /**
         * TODO: write code if binary search is preferred.
         * @param v
         * @return 
         */
        exponent exponentOfVariable(Variable::Arg v) const
        {
            // Linear implementation, as we expect very small monomials.
            exponents_cIt it;
            if((it = std::find(mExponents.cbegin(), mExponents.cend(), v)) == mExponents.cend())
            {
                return 0;
            }
            else
            {
                return it->exp;
            }
        }


        ///////////////////////////
        // Orderings
        ///////////////////////////

        static CompareResult compareLexical(const Monomial& lhs, const Monomial& rhs);
        static CompareResult compareGradedLexical(const Monomial& lhs, const Monomial& rhs);
        static CompareResult compareGradedReverseLexical(const Monomial& lhs, const Monomial& rhs);

        ///////////////////////////
        // Operators
        ///////////////////////////

        // Compare operators
        friend bool operator==(const Monomial& lhs, const Monomial& rhs);
        friend bool operator==(const Monomial& lhs, Variable::Arg rhs);
        friend bool operator==(Variable::Arg lhs, const Monomial& rhs);

        friend bool operator!=(const Monomial& lhs, const Monomial& rhs);
        friend bool operator!=(const Monomial& lhs, Variable::Arg rhs);
        friend bool operator!=(Variable::Arg lhs, const Monomial& rhs);

        // Addition and substraction make only very limited sense, we do not support that.            
        // Multiplication and dividing
        Monomial& operator*=(Variable::Arg rhs);
        Monomial& operator*=(const Monomial& rhs);
        
        friend const Monomial operator*(const Monomial& lhs, Variable::Arg rhs );
        friend const Monomial operator*(Variable::Arg lhs, const Monomial& rhs );
        friend const Monomial operator*(const Monomial& lhs, const Monomial& rhs );
        
        // Output
        friend std::ostream& operator <<( std::ostream& os, const Monomial& rhs );
        
    private:
        // Ordering helpers.
        /**
         * @param lhs 
         * @param rhs
         * @return true if lhs < rhs.
         */
        static CompareResult lexicalCompare(const Monomial& lhs, const Monomial& rhs);
    };
}

#include "Monomial.hpp"
