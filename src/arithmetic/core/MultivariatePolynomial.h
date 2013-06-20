/**
 * @file MultivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include <memory>
#include <vector>

#include "MultivariatePolynomialPolicy.h"
#include "Term.h"



namespace arithmetic
{
    // forward declaration of UnivariatePolynomials
    template<typename Coeff>
    class UnivariatePolynomial;
    /**
     *  The general-purpose multivariate polynomial class.
     */
    template<typename Coeff, typename Policy = StdMultivariatePolynomialPolicy>
    class MultivariatePolynomial
    {
    protected:
        typedef std::vector<std::shared_ptr<const Term<Coeff>>> TermsType;
        /// A vector of all monomials
        TermsType mTerms;
    public:
        MultivariatePolynomial() = default;
        explicit MultivariatePolynomial(Variable::Arg v);
        explicit MultivariatePolynomial(const Monomial& m);
        explicit MultivariatePolynomial(const Term<Coeff>& t);
        explicit MultivariatePolynomial(std::shared_ptr<const Monomial> m);
        explicit MultivariatePolynomial(std::shared_ptr<const Term<Coeff>> t);
        explicit MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Policy>>& pol);
        explicit MultivariatePolynomial(const UnivariatePolynomial<Coeff>& pol);
        
        /**
         * 
         * @return 
         */
        std::shared_ptr<const Monomial> lmon() const;
        /**
         * 
         * @return 
         */
        std::shared_ptr<const Term<Coeff>> lterm() const;
        /**
         * Returns the coefficient of the leading term.
         * Notice that this is not defined for zero polynomials. 
         * @return 
         */
        Coeff lcoeff() const;
        /**
         * 
         * @return 
         */
        exponent highestDegree() const;
        
        bool isZero() const;
        bool isConstant() const;
        bool isLinear() const;
        
        size_t nrTerms() const { return mTerms.size(); }
        std::shared_ptr<const Term<Coeff>> constantPart() const;
        
        /**
         * Checks whether the polynomial is a trivial sum of squares.
         * @return true if polynomial is of the form \sum a_im_i^2 with a_i > 0 for all i.
         */
        bool isTsos() const;
        
        MultivariatePolynomial derivative(Variable::Arg v) const;
        UnivariatePolynomial<MultivariatePolynomial<Coeff, Policy>> coeffRepresentation(Variable::Arg v) const;
        
        /**
         * Multiplies the polynomial with minus one.
         * @return reference to the object.
         */
        MultivariatePolynomial& negate();
        
        template<typename C, typename P>
        friend bool operator==( const MultivariatePolynomial<C,P>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator==(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<C>& rhs);
        template<typename C, typename P>
        friend bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator==(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs);
        template<typename C, typename P>
        friend bool operator==(const MultivariatePolynomial<C,P>& lhs, const Term<C>& rhs);
        template<typename C, typename P>
        friend bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator==(const MultivariatePolynomial<C,P>& lhs, const Monomial& rhs);
        template<typename C, typename P>
        friend bool operator==(const Monomial& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator==(const MultivariatePolynomial<C,P>& lhs, const C& rhs);
        template<typename C, typename P>
        friend bool operator==(const C& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator==(const MultivariatePolynomial<C,P>& lhs, Variable::Arg rhs);
        template<typename C, typename P>
        friend bool operator==(Variable::Arg lhs, const MultivariatePolynomial<C,P>& rhs);
        
        template<typename C, typename P>
        friend bool operator!=( const MultivariatePolynomial<C,P>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<C>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const MultivariatePolynomial<C,P>& lhs, const Term<C>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const Term<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const MultivariatePolynomial<C,P>& lhs, const Monomial& rhs);
        template<typename C, typename P>
        friend bool operator!=(const Monomial& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const MultivariatePolynomial<C,P>& lhs, const C& rhs);
        template<typename C, typename P>
        friend bool operator!=(const C& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend bool operator!=(const MultivariatePolynomial<C,P>& lhs, Variable::Arg rhs);
        template<typename C, typename P>
        friend bool operator!=(Variable::Arg lhs, const MultivariatePolynomial<C,P>& rhs);
        
        MultivariatePolynomial& operator+=(const MultivariatePolynomial& rhs);
        MultivariatePolynomial& operator+=(const Term<Coeff>& rhs);
        MultivariatePolynomial& operator+=(const Monomial& rhs);
        MultivariatePolynomial& operator+=(const Variable::Arg);
        MultivariatePolynomial& operator+=(const Coeff& c);
        
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+( const MultivariatePolynomial<C,P>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<C>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const MultivariatePolynomial<C,P>& lhs, const Term<C>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const Term<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const MultivariatePolynomial<C,P>& lhs, const Monomial& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const Monomial& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const MultivariatePolynomial<C,P>& lhs, const C& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const C& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(const MultivariatePolynomial<C,P>& lhs, Variable::Arg rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator+(Variable::Arg lhs, const MultivariatePolynomial<C,P>& rhs);
        
        MultivariatePolynomial& operator-=(const MultivariatePolynomial& rhs);
        MultivariatePolynomial& operator-=(const Term<Coeff>& rhs);
        MultivariatePolynomial& operator-=(const Monomial& rhs);
        MultivariatePolynomial& operator-=(const Variable::Arg);
        MultivariatePolynomial& operator-=(const Coeff& c);
        
        /**
         * Consider using negate instead, which does not copy the current polynomial.
         * @return 
         */
        const MultivariatePolynomial operator-() const;
        
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-( const MultivariatePolynomial<C,P>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<C>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const MultivariatePolynomial<C,P>& lhs, const Term<C>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const Term<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const MultivariatePolynomial<C,P>& lhs, const Monomial& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const Monomial& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const MultivariatePolynomial<C,P>& lhs, const C& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const C& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(const MultivariatePolynomial<C,P>& lhs, Variable::Arg rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator-(Variable::Arg lhs, const MultivariatePolynomial<C,P>& rhs);
        
        MultivariatePolynomial& operator*=(const MultivariatePolynomial& rhs);
        MultivariatePolynomial& operator*=(const Term<Coeff>& rhs);
        MultivariatePolynomial& operator*=(const Monomial& rhs);
        MultivariatePolynomial& operator*=(const Variable::Arg);
        MultivariatePolynomial& operator*=(const Coeff& c);
        
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*( const MultivariatePolynomial<C,P>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<C>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const MultivariatePolynomial<C,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const MultivariatePolynomial<C,P>& lhs, const Term<C>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const Term<C>& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const MultivariatePolynomial<C,P>& lhs, const Monomial& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const Monomial& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const MultivariatePolynomial<C,P>& lhs, const C& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const C& lhs, const MultivariatePolynomial<C,P>& rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(const MultivariatePolynomial<C,P>& lhs, Variable::Arg rhs);
        template<typename C, typename P>
        friend const MultivariatePolynomial<C,P> operator*(Variable::Arg lhs, const MultivariatePolynomial<C,P>& rhs);
        
        template <typename C, typename P>
        friend std::ostream& operator<<( std::ostream& os, const MultivariatePolynomial<C,P>& rhs );
    };
}

#include "MultivariatePolynomial.tpp"
