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
        Monomial(Variable v, exponent e=1) :
            mExponents(1, VarExpPair(v,e)),
            mTotalDegree(e)
        {

        }

        Monomial& operator =(const Monomial& rhs)
        {
            // Check for self-assignment.
            if(this == &rhs) return *this;
            mExponents = rhs.mExponents;
            mTotalDegree = rhs.mTotalDegree;
            return *this;
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

        exponent highestExponent(Variable::Arg v) const
        {
            assert("NOT IMPLEMENTED");
        }

        ///////////////////////////
        // Orderings
        ///////////////////////////

        static CompareResult compareLexical(const Monomial& lhs, const Monomial& rhs)
        {
            return lexicalCompare(lhs,rhs);
        }


        static CompareResult compareGradedLexical(const Monomial& lhs, const Monomial& rhs)
        {
            if(lhs.mTotalDegree < rhs.mTotalDegree) return CompareResult::LESS;
            if(lhs.mTotalDegree > rhs.mTotalDegree) return CompareResult::GREATER;
            return lexicalCompare(lhs, rhs);
        }

        // 
        // Operators
        //
        
        friend bool operator==(const Monomial& lhs, const Monomial& rhs)
        {
            if(lhs.mTotalDegree != rhs.mTotalDegree) return false;
            else return lhs.mExponents == rhs.mExponents;
        }

        friend bool operator==(const Monomial& lhs, Variable::Arg rhs)
        {
            // TODO think about reordering based on what is most probable.
            if(lhs.mTotalDegree != 1) return false;
            if(lhs.mExponents[0].var == rhs) return true;
            else return false;
        }

        friend bool operator==(const Variable& lhs, const Monomial& rhs)
        {
            return rhs == lhs;
        }

        friend bool operator!=(const Monomial& lhs, const Monomial& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator!=(const Monomial& lhs, const Variable& rhs)
        {
            return !(lhs == rhs);
        }
        friend bool operator!=(const Variable& lhs, const Monomial& rhs)
        {
            return !(lhs == rhs);
        }

        Monomial& operator*=(Variable::Arg v)
        {
            ++mTotalDegree;
            // Linear, as we expect small monomials.
            for(exponents_it it = mExponents.begin(); it != mExponents.end(); ++it)
            {
                // Variable is present
                if(*it == v)
                {
                    ++(it->exp);
                    return *this;
                }
                // Variable is not present, we have to insert v.
                if(*it > v)
                {
                    mExponents.emplace(it,v);
                }
            }
            // Variable was not inserted, insert at end.
            mExponents.emplace_back(v);
            return *this;
        }

        Monomial& operator*=(const Monomial& rhs)
        {
            if(rhs.mTotalDegree == 0) return *this;
            mTotalDegree += rhs.mTotalDegree;

            // Linear, as we expect small monomials.
            exponents_cIt itright = rhs.mExponents.begin();
            for(exponents_it itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
            {
                // Everything is inserted.
                if(itright == rhs.mExponents.end())
                {
                    return *this;
                }
                // Variable is present in both monomials.
                if(itleft->var == itright->var)
                {
                    itleft->exp += itright->exp;
                    ++itright;
                }
                // Variable is not present in lhs, we have to insert var-exp pair from rhs.
                else if(itleft->var > itright->var) 
                {
                    mExponents.insert(itleft,*itright);
                    ++itright;
                }        
            }
            // Insert remainder of rhs.
            mExponents.insert(mExponents.end(), itright, rhs.mExponents.end());
            return *this;
        }

        friend const Monomial operator*(const Monomial& lhs, Variable::Arg rhs )
        {
            // Note that this implementation is not optimized yet!
            Monomial result(lhs);
            result *= rhs;
            return result;
        }

        friend const Monomial operator*(Variable::Arg lhs, const Monomial& rhs )
        {
            return rhs * lhs;
        }

        friend const Monomial operator*(const Monomial& lhs, const Monomial& rhs )
        {
            // Note that this implementation is not optimized yet!
            Monomial result(lhs);
            result *= rhs;
            return result;
        }

        friend std::ostream& operator <<( std::ostream& os, const Monomial& rhs )
        {
            for( VarExpPair vp : rhs.mExponents )
            {
                os << vp;
            }
            return os;
        }

    private:
        
        static CompareResult lexicalCompare(const Monomial& lhs, const Monomial& rhs)
        {
            exponents_cIt lhsit = lhs.mExponents.begin( );
            exponents_cIt rhsit = rhs.mExponents.begin( );
            exponents_cIt lhsend = lhs.mExponents.end( );
            exponents_cIt rhsend = rhs.mExponents.end( );

            while( lhsit != lhsend )
            {
                if( rhsit == rhsend )
                    return CompareResult::GREATER;
                //which variable occurs first
                if( lhsit->var == rhsit->var )
                {
                    //equal variables
                    if( lhsit->exp < rhsit->exp )
                        return CompareResult::LESS;
                    if( lhsit->exp > rhsit->exp )
                        return CompareResult::GREATER;
                }
                else
                {
                    return (lhsit->var < rhsit->var ) ? CompareResult::GREATER : CompareResult::LESS;
                }
                ++lhsit;
                ++rhsit;
            }
            if( rhsit == rhsend )
                return CompareResult::EQUAL;
            return CompareResult::LESS;
        }

    };
}
