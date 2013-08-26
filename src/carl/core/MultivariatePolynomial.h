/**
 * @file MultivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include <memory>
#include <vector>

#include "MultivariatePolynomialPolicy.h"
#include "Term.h"

namespace carl
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
public:
	typedef typename Policy::Ordering Ordering;
	typedef Term<Coeff> TermType;
	typedef Coeff CoeffType;
protected:
	typedef std::vector<std::shared_ptr<const Term<Coeff >> > TermsType;

protected:
	/// A vector of all monomials
	TermsType mTerms;
public:
	MultivariatePolynomial() = default;
	explicit MultivariatePolynomial(Variable::Arg v);
	explicit MultivariatePolynomial(const Monomial& m);
	explicit MultivariatePolynomial(const Term<Coeff>& t);
	explicit MultivariatePolynomial(std::shared_ptr<const Monomial> m);
	explicit MultivariatePolynomial(std::shared_ptr<const Term<Coeff >> t);
	explicit MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Policy >> &pol);
	explicit MultivariatePolynomial(const UnivariatePolynomial<Coeff>& pol);
	template<typename InputIterator>
	MultivariatePolynomial(InputIterator begin, InputIterator end);

	/**
	 * The leading monomial
	 * @return 
	 */
	std::shared_ptr<const Monomial> lmon() const;
	/**
	 * The leading term
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
	 * Calculates the max. degree over all monomials occurring in the polynomial.
	 * @return 
	 */
	exponent highestDegree() const;

	bool isZero() const;
	bool isConstant() const;
	bool isLinear() const;

	/**
	 * Calculates the number of terms.
	 * @return the number of terms
	 */
	size_t nrTerms() const
	{
		return mTerms.size();
	}
	std::shared_ptr<const Term<Coeff>> constantPart() const;

	/**
	 * For the polynomial p, the function calculates a polynomial p - lt(p).
	 * The function assumes the polynomial to be nonzero, otherwise, lt(p) is not defined.
	 * @return A new polynomial p - lt(p).
	 */
	MultivariatePolynomial tail() const;
	/**
	 * Drops the leading term.
	 * The function assumes the polynomial to be nonzero, otherwise the leading term is not defined.
	 * @return  A reference to this.
	 */
	MultivariatePolynomial& stripLT();
	
	const std::shared_ptr<const Term<Coeff>>& operator[](int) const;

	/**
	 * Checks whether the polynomial is a trivial sum of squares.
	 * @return true if polynomial is of the form \sum a_im_i^2 with a_i > 0 for all i.
	 */
	bool isTsos() const;

	MultivariatePolynomial derivative(Variable::Arg v) const;
	UnivariatePolynomial<MultivariatePolynomial<Coeff, Policy>> coeffRepresentation(Variable::Arg v) const;

	unsigned hash() const;

	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename P>
	friend bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, const Term<C>& rhs);
	template<typename C, typename P>
	friend bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, const Monomial& rhs);
	template<typename C, typename P>
	friend bool operator==(const Monomial& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, const C& rhs);
	template<typename C, typename P>
	friend bool operator==(const C& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator==(const MultivariatePolynomial<C, P>& lhs, Variable::Arg rhs);
	template<typename C, typename P>
	friend bool operator==(Variable::Arg lhs, const MultivariatePolynomial<C, P>& rhs);

	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, const Term<C>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const Term<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, const Monomial& rhs);
	template<typename C, typename P>
	friend bool operator!=(const Monomial& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, const C& rhs);
	template<typename C, typename P>
	friend bool operator!=(const C& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator!=(const MultivariatePolynomial<C, P>& lhs, Variable::Arg rhs);
	template<typename C, typename P>
	friend bool operator!=(Variable::Arg lhs, const MultivariatePolynomial<C, P>& rhs);


	template<typename C, typename P>
	friend bool operator<(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend bool operator<=(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);

	/**
	 * Notice that when adding a polynomial which consists of just one term, it will be faster to just add the pointer to this term! 
	 * @param rhs
	 * @return 
	 */
	MultivariatePolynomial& operator+=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator+=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator+=(const Monomial& rhs);
	MultivariatePolynomial& operator+=(const Variable::Arg);
	MultivariatePolynomial& operator+=(const Coeff& c);


	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, const Term<C>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const Term<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, const Monomial& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const Monomial& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, const C& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const C& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(const MultivariatePolynomial<C, P>& lhs, Variable::Arg rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(Variable::Arg lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator+(Variable::Arg lhs, Variable::Arg rhs);

	MultivariatePolynomial& operator-=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator-=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator-=(const Monomial& rhs);
	MultivariatePolynomial& operator-=(const Variable::Arg);
	MultivariatePolynomial& operator-=(const Coeff& c);


	const MultivariatePolynomial operator-() const;

	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, const Term<C>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const Term<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, const Monomial& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const Monomial& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, const C& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const C& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(const MultivariatePolynomial<C, P>& lhs, Variable::Arg rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator-(Variable::Arg lhs, const MultivariatePolynomial<C, P>& rhs);

	MultivariatePolynomial& operator*=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator*=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator*=(const Monomial& rhs);
	MultivariatePolynomial& operator*=(const Variable::Arg);
	MultivariatePolynomial& operator*=(const Coeff& c);

	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, const Term<C>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const Term<C>& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, const Monomial& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const Monomial& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, const C& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const C& lhs, const MultivariatePolynomial<C, P>& rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(const MultivariatePolynomial<C, P>& lhs, Variable::Arg rhs);
	template<typename C, typename P>
	friend const MultivariatePolynomial<C, P> operator*(Variable::Arg lhs, const MultivariatePolynomial<C, P>& rhs);

	template <typename C, typename P>
	friend std::ostream& operator<<(std::ostream& os, const MultivariatePolynomial<C, P>& rhs);

	template<typename C, typename P>
	static bool compareByLeadingTerm(const MultivariatePolynomial<C, P>& p1, const MultivariatePolynomial<C, P>& p2)
	{
		return Ordering::less(p1.lterm(), p2.lterm());
	}

	template<typename C, typename P>
	static bool compareByNrTerms(const MultivariatePolynomial<C, P>& p1, const MultivariatePolynomial<C, P>& p2)
	{
		return (p1.nrOfTerms() < p2.nrOfTerms());
	}

private:
	void sortTerms();

};


}

#include "MultivariatePolynomial.tpp"
