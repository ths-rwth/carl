/**
 * @file MultivariatePolynomial.h 
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include <memory>
#include <vector>

#include "Polynomial.h"
#include "Term.h"
#include "DivisionResult.h"
#include "MultivariatePolynomialPolicy.h"
#include "VariableInformation.h"

namespace carl
{
// forward declaration of UnivariatePolynomials
template<typename Coeff>
class UnivariatePolynomial;

/**
 *  The general-purpose multivariate polynomial class.
 * @ingroup multirp
 */
template<typename Coeff, typename Ordering = GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class MultivariatePolynomial : public Polynomial, Policies
{
public:
	/// The ordering of the terms.
	typedef Ordering OrderedBy;
	typedef Term<Coeff> TermType;
	typedef Monomial MonomType;
	typedef Coeff CoeffType;
	typedef Policies Policy;
	typedef typename UnderlyingNumberType<Coeff>::type NumberType;
	typedef typename IntegralT<NumberType>::type IntNumberType;
protected:
	typedef std::vector<std::shared_ptr<const Term<Coeff >> > TermsType;
	
	template <bool gatherCoeff>
	using VarInfo = VariableInformation<gatherCoeff, MultivariatePolynomial>;
protected:
	/// A vector of all monomials
	TermsType mTerms;
public:
	MultivariatePolynomial() = default;
	explicit MultivariatePolynomial(int c);
	explicit MultivariatePolynomial(const Coeff& c);
	explicit MultivariatePolynomial(Variable::Arg v);
	explicit MultivariatePolynomial(const Monomial& m);
	explicit MultivariatePolynomial(const Term<Coeff>& t);
	explicit MultivariatePolynomial(std::shared_ptr<const Monomial> m);
	explicit MultivariatePolynomial(std::shared_ptr<const Term<Coeff >> t);
	explicit MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering,Policy>> &pol);
	explicit MultivariatePolynomial(const UnivariatePolynomial<Coeff>& pol);
	template<class OtherPolicy>
	explicit MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicy>&);
	template<typename InputIterator>
	MultivariatePolynomial(InputIterator begin, InputIterator end, bool duplicates, bool sorted);
	MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms);
	MultivariatePolynomial(const std::initializer_list<Variable>& terms);
	
    virtual ~MultivariatePolynomial() {}
    
	//Polynomial interface implementations.
	/**
	 * @see class Polynomial
     * @return 
     */
	virtual bool isUnivariateRepresented() const override
	{
		return false;
	}
	/**
	 * @see class Polynomial
     * @return 
     */
	virtual bool isMultivariateRepresented() const override
	{
		return true;
	}
	
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
	exponent totalDegree() const;

	bool isZero() const;
	bool isConstant() const;
	bool isLinear() const;

	bool isNumber() const
	{
		return this->isConstant();
	}
    
    Definiteness definiteness() const;

	/**
	 * Calculates the number of terms.
	 * @return the number of terms
	 */
	size_t nrTerms() const
	{
		return mTerms.size();
	}
	/**
	 * Gives the last term according to Ordering. Notice that if there is a constant part, it is always trailing.
     * @return 
     */
	std::shared_ptr<const Term<Coeff>> trailingTerm() const;
	/**
	 * 
     * @return 
     */
	bool hasConstantTerm() const;
	Coeff constantPart() const;
	
	typename TermsType::const_iterator begin() const
	{
		return mTerms.begin();
	}
	
	typename TermsType::const_iterator end() const
	{
		return mTerms.end();
	}
    
	typename TermsType::const_reverse_iterator rbegin() const
	{
		return mTerms.rbegin();
	}
	
	typename TermsType::const_reverse_iterator rend() const
	{
		return mTerms.rend();
	}

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
	
	/**
	 * Checks whether only one variable occurs.
     * @return 
	 * Notice that it might be better to use the variable information if several pieces of information are requested.
     */
	bool isUnivariate() const;

	/**
	 * Checks whether the polynomial is a trivial sum of squares.
	 * @return true if polynomial is of the form \\sum a_im_i^2 with a_i > 0 for all i.
	 */
	bool isTsos() const;
	
	/**
	 * Iterates through all terms to find variables occuring in this polynomial.
     * @param vars Holds the variables occuring in the polynomial at return.
     */
	void gatherVariables(std::set<Variable>& vars) const;
	std::set<Variable> gatherVariables() const;
    
    /**
     * @param v The variable to check for its occurrence.
     * @return true, if the variable occurs in this term.
     */
    bool has(Variable::Arg v) const;
	
	bool isReducibleIdentity() const;

	/**
	 * Divides the polynomial by the given coefficient.
	 * Applies if the coefficients are from a field.
     * @param divisor
     * @return 
     */
	template<typename C = Coeff, EnableIf<is_field<C>> = dummy>
	MultivariatePolynomial divideBy(const Coeff& divisor) const;

	/**
	 * Divides the polynomial by another polynomial.
	 * If the divisor divides this polynomial, quotient contains the result of the division and true is returned.
	 * Otherwise, false is returned and the content of quotient remains unchanged.
	 * Applies if the coefficients are from a field.
	 * Note that the quotient must not be *this.
     * @param divisor
     * @param quotient
     * @return 
     */
	template<typename C = Coeff, EnableIf<is_field<C>> = dummy>
	bool divideBy(const MultivariatePolynomial& divisor, MultivariatePolynomial& quotient) const;
	
	MultivariatePolynomial derivative(Variable::Arg v, unsigned nth=1) const;
	
	/**
     * @return The lcm of the denominators of the coefficients in p divided by the gcd of numerators 
     *         of the coefficients in p.
     */
	Coeff coprimeFactor() const;
	
	/**
     * @return p * p.coprimeFactor()
     * @see coprimeFactor()
     */
	MultivariatePolynomial coprimeCoefficients() const;
	
	/**
	 * For a polynomial p, returns p/lc(p)
     * @return 
     */
	MultivariatePolynomial normalize() const;
	
	/**
	 * Replace the given variable by the given polynomial within this multivariate polynomial.
     */
	void substituteIn(const Variable::Arg var, const MultivariatePolynomial& value);
	
	/**
	 * Replace the given variable by the given value.
     * @return A new polynomial without resulting from this substitution.
     */
	MultivariatePolynomial substitute(const Variable::Arg var, const MultivariatePolynomial& value) const;
	
	/**
	 * Replace all variables by a value given in their map.
     * @return A new polynomial without the variables in map.
     */
	MultivariatePolynomial substitute(const std::map<Variable, MultivariatePolynomial>& substitutions) const;
    
	/**
	 * Replace all variables by a value given in their map.
     * @return A new polynomial without the variables in map.
     */
	template<typename SubstitutionType = Coeff>
	MultivariatePolynomial substitute(const std::map<Variable, SubstitutionType>& substitutions) const;
    
	/**
	 * Replace all variables by a Term in which the variable does not occur.
     * @param substitutions
     * @return 
     */
	MultivariatePolynomial substitute(const std::map<Variable, Term<Coeff>>& substitutions) const;
	
	/**
	 * Like substitute, but expects substitutions for all variables.
     * @return For a polynomial p, the function value p(x_1,...,x_n).
     */
    template<typename SubstitutionType = Coeff>
	Coeff evaluate(const std::map<Variable, SubstitutionType>& substitutions) const;
	
	bool divides(const MultivariatePolynomial& b) const;
	/**
	 * Calculates the S-Polynomial.
     * @param p
     * @param q
     * @return 
     */
	static MultivariatePolynomial SPolynomial(const MultivariatePolynomial& p, const MultivariatePolynomial& q);

	unsigned hash() const;
	
	MultivariatePolynomial pow(unsigned exp) const;
    
	MultivariatePolynomial naive_pow(unsigned exp) const;
	
	std::string toString(bool infix=true, bool friendlyVarNames=true) const;
	
	MultivariatePolynomial<typename IntegralT<Coeff>::type, Ordering, Policies> toIntegerDomain() const;
	
	UnivariatePolynomial<Coeff> toUnivariatePolynomial() const;
	UnivariatePolynomial<MultivariatePolynomial> toUnivariatePolynomial(Variable::Arg mainVar) const;
	
	const std::shared_ptr<const Term<Coeff>>& operator[](unsigned) const;

	MultivariatePolynomial mod(const typename IntegralT<Coeff>::type& modulo) const;
	
	template<bool gatherCoeff>
	VariableInformation<gatherCoeff, MultivariatePolynomial> getVarInfo(const Variable& v) const;

	template<bool gatherCoeff>
	VariablesInformation<gatherCoeff, MultivariatePolynomial> getVarInfo() const;
	
	template<typename C=Coeff, EnableIf<is_number<C>> = dummy>
	typename UnderlyingNumberType<C>::type numericContent() const;
	template<typename C=Coeff, DisableIf<is_number<C>> = dummy>
	typename UnderlyingNumberType<C>::type numericContent() const;
	
	template<typename C=Coeff, EnableIf<is_number<C>> = dummy>
	IntNumberType mainDenom() const;

	
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int rhs);
	template<typename C, typename O, typename P>
	friend bool operator==(int lhs, const MultivariatePolynomial<C,O,P>& rhs);
	

	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, int rhs);
	template<typename C, typename O, typename P>
	friend bool operator!=(int lhs, const MultivariatePolynomial<C,O,P>& rhs);
	
 

	template<typename C, typename O, typename P>
	friend bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
//	template<typename C, typename O, typename P>
//	friend bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
//	template<typename C, typename O, typename P>
//	friend bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
//	template<typename C, typename O, typename P>
//	friend bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);

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


	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, Variable::Arg rhs);

	MultivariatePolynomial& operator-=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator-=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator-=(const Monomial& rhs);
	MultivariatePolynomial& operator-=(const Variable::Arg);
	MultivariatePolynomial& operator-=(const Coeff& c);


	const MultivariatePolynomial operator-() const;

	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs);

	MultivariatePolynomial& operator*=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator*=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator*=(const Monomial& rhs);
	MultivariatePolynomial& operator*=(const Variable::Arg);
	MultivariatePolynomial& operator*=(const Coeff& c);

	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	//We need sfinae to prevent C=MultivariatePolynomial
	//template<typename C, typename O, typename P>
	//friend const MultivariatePolynomial<C,O,P> operator*(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	//template<typename C, typename O, typename P>
	//friend const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator*(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs);

	MultivariatePolynomial& operator/=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator/=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator/=(const Monomial& rhs);
	MultivariatePolynomial& operator/=(const Variable::Arg);
	MultivariatePolynomial& operator/=(const Coeff& c);

	
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	
	
	template <typename C, typename O, typename P>
	friend std::ostream& operator<<(std::ostream& os, const MultivariatePolynomial<C,O,P>& rhs);

	static bool compareByLeadingTerm(const MultivariatePolynomial& p1, const MultivariatePolynomial& p2)
	{
		return Ordering::less(p1.lterm(), p2.lterm());
	}

	static bool compareByNrTerms(const MultivariatePolynomial& p1, const MultivariatePolynomial& p2)
	{
		return (p1.nrTerms() < p2.nrTerms());
	}

	
private:
	void sortTerms();
	/**
	 * Replaces the current terms by the given new terms.
	 * Takes care of trailing zero terms.
	 * @param newTerms
	 */
    void setTerms(std::vector<std::shared_ptr<const Term<Coeff>>>& newTerms);

public:
	/**
	 * Asserts that this polynomial complies with the requirements and assumptions for MultivariatePolynomial objects.
	 * 
	 * <ul>
	 * <li>All terms are actually valid and not nullptr or alike</li>
	 * <li>Only the trailing term may be constant.</li>
	 * </ul>
     */
	void checkConsistency() const;
};

} // namespace carl

/**
 * @ingroup multirp
 */
namespace std
{
    template<typename C, typename O, typename P>
    struct hash<carl::MultivariatePolynomial<C,O,P>>
    {
        size_t operator()(const carl::MultivariatePolynomial<C,O,P>& mpoly) const 
        {
            size_t result = 0;
            for(auto iter = mpoly.begin(); iter != mpoly.end(); ++iter)
                result ^= hash<carl::Term<C>>()(**iter);
            return result;
        }
    };
} // namespace std

#include "MultivariatePolynomial.tpp"
