/**
 * @file Term.h 
 * @author Sebastian Junges
 */

#pragma once

#include <memory>
#include "Monomial.h"

namespace arithmetic
{

template<typename Coefficient>
class Term
{
    private:
        Coefficient                     mCoeff = 0;
        std::shared_ptr<const Monomial> mMonomial;
    
    public:
        Term() = default;
        explicit Term(const Coefficient& c);
        explicit Term(Variable::Arg v);
        explicit Term(const Monomial& m);
        explicit Term(std::shared_ptr<const Monomial> m);
        Term(const Coefficient& c, const Monomial& m);
        Term(const Coefficient& c, std::shared_ptr<const Monomial> m);
        
        /**
         * Get the coefficient.
         * @return 
         */
        const Coefficient& coeff() const
        {
            return mCoeff;
        }
        
        std::shared_ptr<const Monomial> monomial() const
        {
            return mMonomial;
        }
        /**
         * Gives the total degree, i.e. the sum of all exponents.
         * @return 
         */
        exponent tdeg() const
        {
            if(!mMonomial) return 0;
            return mMonomial->tdeg();
        }
        /**
         * Checks whether the monomial is a constant.
         * @return 
         */
        bool isConstant() const
        {
            return !mMonomial;
        }
        /**
         * Checks whether the monomial has exactly the degree one.
         * @return 
         */
        bool isLinear() const
        {
            if(!mMonomial) return false;
            return mMonomial->isLinear();
        }
        bool isAtMostLinear() const
        {
            if(!mMonomial) return true;
            return mMonomial->isAtMostLinear();
        }
        size_t nrVariables() const
        {
            if(!mMonomial) return 0;
            return mMonomial->nrVariables();
        }
        
        template<typename Coeff>
        friend bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend bool operator==(const Term<Coeff>& lhs, const Coeff& rhs);
        template<typename Coeff>
        friend bool operator==(const Coeff& lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend bool operator==(const Term<Coeff>& lhs, Variable::Arg rhs);
        template<typename Coeff>
        friend bool operator==(Variable::Arg lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend bool operator==(const Term<Coeff>& lhs, const Monomial& rhs);
        template<typename Coeff>
        friend bool operator==(const Monomial& lhs, const Term<Coeff>& rhs);
        
        template<typename Coeff>
        friend bool operator!=(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs);
        template<typename Coeff>
        friend bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend bool operator!=(const Term<Coeff>& lhs, Variable::Arg rhs);
        template<typename Coeff>
        friend bool operator!=(Variable::Arg lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend bool operator!=(const Term<Coeff>& lhs, const Monomial& rhs);
        template<typename Coeff>
        friend bool operator!=(const Monomial& lhs, const Term<Coeff>& rhs);
        
        Term& operator *=(const Coefficient& rhs);
        Term& operator *=(Variable::Arg rhs);
        Term& operator *=(const Term& rhs);
        
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Term<Coeff>& lhs, const Coeff& rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Term<Coeff>& lhs, Variable::Arg rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(Variable::Arg, const Term<Coeff>& rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Coeff& lhs, Variable::Arg rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(Variable::Arg lhs, const Coeff& rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Coeff& lhs, const Monomial& rhs);
        template<typename Coeff>
        friend const Term<Coeff> operator*(const Monomial& lhs, const Coeff& rhs);
        
        template<typename Coeff>
        friend std::ostream& operator<<(std::ostream& lhs, const Term<Coeff>& rhs);
        
};

}

#include "Term.tpp"
