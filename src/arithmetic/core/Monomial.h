/**
 * @file Monomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include <algorithm>

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
    private:
        /**
         * Set if we want to use binary search instead of linear search.
         */
        static const bool PREFERBINARYSEARCH = false;

    protected:
        /// The coefficient of the monomial..
        Coefficient mCoefficient = 0;
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

        Monomial& operator=(const Monomial& rhs);
        /**
         * Gives the coefficient, i.e. the constant factor.
         * @return 
         */
        const Coefficient& coeff() const
        {
            return mCoefficient;
        }
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

        static CompareResult compareLexical();
        static CompareResult compareGradedLexical();
        static CompareResult compareGradedReverseLexical();

        ///////////////////////////
        // Operators
        ///////////////////////////

        // Compare operators
        template<typename C1, typename C2>
        friend bool operator==(const Monomial<C1>& lhs, const Monomial<C2>& rhs);
        template<typename C>
        friend bool operator==(const Monomial<C>& lhs, const C& rhs);
        template<typename C>
        friend bool operator==(const C& lhs, const Monomial<C>& rhs);
        template<typename C>
        friend bool operator==(const Monomial<C>& lhs, Variable::Arg rhs);
        template<typename C>
        friend bool operator==(Variable::Arg lhs, const Monomial<C>& rhs);

        template<typename C1, typename C2>
        friend bool operator!=(const Monomial<C1>& lhs, const Monomial<C2>& rhs);
        template<typename C>
        friend bool operator!=(const Monomial<C>& lhs, const C& rhs);
        template<typename C>
        friend bool operator!=(const C& lhs, const Monomial<C>& rhs);
        template<typename C>
        friend bool operator!=(const Monomial<C>& lhs, Variable::Arg rhs);
        template<typename C>
        friend bool operator!=(Variable::Arg lhs, const Monomial<C>& rhs);

        // Addition and substraction make only very limited sense, we do not support that.            
        // Multiplication and dividing
        Monomial& operator*=(Variable::Arg rhs);
        Monomial& operator*=(const Coefficient& rhs);
        Monomial& operator*=(const Monomial& rhs);
        
        template<typename C>
        friend const Monomial<C> operator*(const C& lhs, Variable::Arg rhs);
        template<typename C>
        friend const Monomial<C> operator*(Variable::Arg lhs, const C& rhs);
        template<typename C>
        friend const Monomial<C> operator*(const Monomial<C>& lhs, Variable::Arg rhs );
        template<typename C>
        friend const Monomial<C> operator*(Variable::Arg lhs, const Monomial& rhs );
        template<typename C>
        friend const Monomial<C> operator*(const Monomial<C>& lhs, const C& rhs );
        template<typename C>
        friend const Monomial<C> operator*(const C& lhs, const Monomial<C>& rhs);
        template<typename C>
        friend const Monomial<C> operator*(const Monomial<C>& lhs, const Monomial<C>& rhs );
        
        // Output
        template <typename C>
        friend std::ostream& operator <<( std::ostream& os, const Monomial<C>& rhs );
    };
}

#include "Monomial.tpp"
