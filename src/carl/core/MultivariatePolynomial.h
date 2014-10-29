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
 * The general-purpose multivariate polynomial class.
 *
 * It is represented as a sum of terms, being a coefficient and a monomial.
 * @ingroup multirp
 */
template<typename Coeff, typename Ordering = GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class MultivariatePolynomial : public Polynomial, public Policies
{
public:
	/// The ordering of the terms.
	typedef Ordering OrderedBy;
	/// Type of the terms.
	typedef Term<Coeff> TermType;
	/// Type of the monomials within the terms.
	typedef Monomial MonomType;
	/// Type of the coefficients.
	typedef Coeff CoeffType;
	/// Policies for this monomial.
	typedef Policies Policy;
	/// Number type within the coefficients.
	typedef typename UnderlyingNumberType<Coeff>::type NumberType;
	/// Integer type associated with the number type.
	typedef typename IntegralType<NumberType>::type IntNumberType;
protected:
	/// Type our terms vector.
	typedef std::vector<std::shared_ptr<const Term<Coeff >> > TermsType;
	
	template <bool gatherCoeff>
	using VarInfo = VariableInformation<gatherCoeff, MultivariatePolynomial>;
protected:
	/// A vector of all terms.
	TermsType mTerms;
public:
	enum ConstructorOperation : unsigned { ADD, SUB, MUL, DIV };
	
	/// @name Constructors
	/// @{
	MultivariatePolynomial() = default;
	explicit MultivariatePolynomial(int c);
	explicit MultivariatePolynomial(unsigned c);
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
	explicit MultivariatePolynomial(InputIterator begin, InputIterator end, bool duplicates, bool sorted);
	explicit MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms);
	explicit MultivariatePolynomial(const std::initializer_list<Variable>& terms);
	explicit MultivariatePolynomial(const std::pair<ConstructorOperation, std::vector<MultivariatePolynomial>>& p);
	/// @}
	
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
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 48
	 * @return Total degree.
	 */
	exponent totalDegree() const;

	/**
	 * Checks if the polynomial is zero.
	 * @return If this is zero.
	 */
	bool isZero() const;
	/**
	 * 
     * @return 
     */
	bool isOne() const;
	/**
	 * Checks if the polynomial is constant.
	 * @return If this is constant.
	 */
	bool isConstant() const;
	/**
	 * Checks if the polynomial is linear.
	 * @return If this is linear.
	 */
	bool isLinear() const;
	/**
	 * Checks if the polynomial is a number.
	 * This is basically the same as being a constant.
	 * @return If this is a number.
	 */
	bool isNumber() const
	{
		return this->isConstant();
	}
	
	/**
	 * Retrieves information about the definiteness of the polynomial.
	 * @return Definiteness of this.
	 */
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
	 * Checks if the polynomial has a constant term that is not zero.
	 * @return If there is a constant term unequal to zero.
	 */
	bool hasConstantTerm() const;
	/**
	 * Retrieves the constant term of this polynomial or zero, if there is no constant term.
	 * @return Constant term.
	 */
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

	typename TermsType::iterator eraseTerm(typename TermsType::iterator pos) {
		return mTerms.erase(pos);
	}
	typename TermsType::const_iterator eraseTerm(typename TermsType::const_iterator pos) {
		return mTerms.erase(pos);
	}
	TermsType& getTerms() {
		return mTerms;
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
	
	/**
	 * Calculating the quotient and the remainder, such that for a given polynomial p we have
	 * p = divisor * quotient + remainder.
	 * @param divisor Another polynomial
	 * @return A divisionresult, holding the quotient and the remainder.
	 * @see
	 * @note Division is only defined on fields
	 */
	DivisionResult<MultivariatePolynomial> divideBy(const MultivariatePolynomial& divisor) const;
	/**
	 * Calculates the quotient
	 * @param divisor
	 * @return 
	 * @see MultivariatePolynomial::divideBy
	 */
	MultivariatePolynomial quotient(const MultivariatePolynomial& divisor) const;
	/**
	 * Calculates the remainder
	 * @param divisor
	 * @return 
	 * @see MultivariatePolynomial::divideBy
	 */
	MultivariatePolynomial remainder(const MultivariatePolynomial& divisor) const;
	MultivariatePolynomial prem(const MultivariatePolynomial& divisor) const;
	
	
	MultivariatePolynomial derivative(Variable::Arg v, unsigned nth=1) const;
	
	/**
	 * @return The lcm of the denominators of the coefficients in p divided by the gcd of numerators 
	 *		 of the coefficients in p.
	 */
	template<typename C = Coeff, EnableIf<is_subset_of_rationals<C>> = dummy>
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
	void substituteIn(Variable::Arg var, const MultivariatePolynomial& value);
	
	/**
	 * Replace the given variable by the given value.
	 * @return A new polynomial without resulting from this substitution.
	 */
	MultivariatePolynomial substitute(Variable::Arg var, const MultivariatePolynomial& value) const;
	
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

	MultivariatePolynomial pow(unsigned exp) const;
	
	MultivariatePolynomial naive_pow(unsigned exp) const;
	
	std::string toString(bool infix=true, bool friendlyVarNames=true) const;
	
	MultivariatePolynomial<typename IntegralType<Coeff>::type, Ordering, Policies> toIntegerDomain() const;
	
	UnivariatePolynomial<Coeff> toUnivariatePolynomial() const;
	UnivariatePolynomial<MultivariatePolynomial> toUnivariatePolynomial(Variable::Arg mainVar) const;
	
	const std::shared_ptr<const Term<Coeff>>& operator[](unsigned) const;

	MultivariatePolynomial mod(const typename IntegralType<Coeff>::type& modulo) const;
	
	template<bool gatherCoeff>
	VariableInformation<gatherCoeff, MultivariatePolynomial> getVarInfo(Variable::Arg v) const;

	template<bool gatherCoeff>
	VariablesInformation<gatherCoeff, MultivariatePolynomial> getVarInfo() const;
	
	template<typename C=Coeff, EnableIf<is_number<C>> = dummy>
	typename UnderlyingNumberType<C>::type numericContent() const;
	template<typename C=Coeff, DisableIf<is_number<C>> = dummy>
	typename UnderlyingNumberType<C>::type numericContent() const;
	
	template<typename C=Coeff, EnableIf<is_number<C>> = dummy>
	IntNumberType mainDenom() const;
	
	/// @name In-place addition operators
	/// @{
	/**
	 * Add something to this polynomial and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	MultivariatePolynomial& operator+=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator+=(const TermType& rhs);
	MultivariatePolynomial& operator+=(const std::shared_ptr<const TermType>& rhs);
	MultivariatePolynomial& operator+=(const Monomial& rhs);
	MultivariatePolynomial& operator+=(Variable::Arg rhs);
	MultivariatePolynomial& operator+=(const Coeff& rhs);
	/// @}

	/// @name In-place subtraction operators
	/// @{
	/**
	 * Subtract something from this polynomial and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	MultivariatePolynomial& operator-=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator-=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator-=(const Monomial& rhs);
	MultivariatePolynomial& operator-=(Variable::Arg);
	MultivariatePolynomial& operator-=(const Coeff& c);
	/// @}


	const MultivariatePolynomial operator-() const;

	/// @name In-place multiplication operators
	/// @{
	/**
	 * Multiply this polynomial with something and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	MultivariatePolynomial& operator*=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator*=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator*=(const Monomial& rhs);
	MultivariatePolynomial& operator*=(Variable::Arg);
	MultivariatePolynomial& operator*=(const Coeff& c);
	/// @}

	/// @name In-place division operators
	/// @{
	/**
	 * Divide this polynomial by something and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	MultivariatePolynomial& operator/=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator/=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator/=(const Monomial& rhs);
	MultivariatePolynomial& operator/=(Variable::Arg);
	MultivariatePolynomial& operator/=(const Coeff& c);
	/// @}
	
	/// @name Division operators
	/// @{
	/**
	 * Perform a division involving a polynomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs / rhs`
	 */
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	friend const MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, unsigned long rhs);
	/// @}
	
	/**
	 * Streaming operator for multivariate polynomials.
	 * @param os Output stream.
	 * @param rhs Polynomial.
	 * @return `os`.
	 */
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
	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> quotient(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q)
	{
		return p.quotient(q);
	}

	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> pow(const MultivariatePolynomial<C,O,P>& p, unsigned exp)
	{
		return p.pow(exp);
	}
	
	/// @name Equality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are equal.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs == rhs`
	 */
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P, DisableIf<std::is_integral<C>> = dummy>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int i);
	template<typename C, typename O, typename P>
	inline bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs == lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator==(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs == lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator==(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs == lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator==(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs == lhs;
	}

	template<typename C, typename O, typename P>
	bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	/// @}

	/// @name Inequality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are not equal.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs != rhs`
	 */
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}

	template<typename C, typename O, typename P>
	inline bool operator!=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(lhs == rhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs) {
		return !(lhs == rhs);
	}
	/// @}

	/// @name Less than comparison operators
	/// @{
	/**
	 * Checks if the first arguments is less than the second.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs < rhs`
	 */
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator<(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	/// @}

	/// @name Less or equal comparison operators
	/// @{
	/**
	 * Checks if the first arguments is less or equal than the second.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs <= rhs`
	 */
	template<typename C, typename O, typename P>
	inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(rhs < lhs);
	}
	template<typename C, typename O, typename P>
	inline bool operator<=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return !(rhs < lhs);
	}
	/// @}

	/// @name Greater than comparison operators
	/// @{
	/**
	 * Checks if the first arguments is greater than the second.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs > rhs`
	 */
	template<typename C, typename O, typename P>
	inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs < lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs < lhs;
	}
	/// @}

	/// @name Greater or equal comparison operators
	/// @{
	/**
	 * Checks if the first arguments is greater or equal than the second.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs >= rhs`
	 */
	template<typename C, typename O, typename P>
	inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs <= lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator>=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs <= lhs;
	}
	/// @}
	
	/// @name Addition operators
	/// @{
	/**
	 * Performs an addition involving a polynomial.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs + rhs`
	 */
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const Term<C>& lhs, const Term<C>& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res += rhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const Term<C>& lhs, const Monomial& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res += rhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const Term<C>& lhs, Variable::Arg rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res += rhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const Term<C>& lhs, const C& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const Monomial& lhs, const Term<C>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const Monomial& lhs, const C& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(Variable::Arg lhs, const Term<C>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(Variable::Arg lhs, const C& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res += rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator+(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const C& lhs, const Term<C>& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const C& lhs, const Monomial& rhs) {
		return rhs + lhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator+(const C& lhs, Variable::Arg rhs) {
		return rhs + lhs;
	}

	template<typename C, typename O, typename P>
	const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	const MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	const MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename O, typename P>
	const MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs);
	/// @}
	
	/// @name Subtraction operators
	/// @{
	/**
	 * Performs an subtraction involving a polynomial.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs - rhs`
	 */
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const Term<C>& lhs, const Term<C>& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res -= rhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const Term<C>& lhs, const Monomial& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res -= rhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const Term<C>& lhs, Variable::Arg rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res -= rhs;
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const Term<C>& lhs, const C& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const Monomial& lhs, const Term<C>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const Monomial& lhs, const C& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(Variable::Arg lhs, const Term<C>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(Variable::Arg lhs, const C& rhs) {
		MultivariatePolynomial<C> res(lhs);
		return res -= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator-(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const C& lhs, const Term<C>& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const C& lhs, const Monomial& rhs) {
		return -(rhs - lhs);
	}
	template<typename C>
	inline const MultivariatePolynomial<C> operator-(const C& lhs, Variable::Arg rhs) {
		return -(rhs - lhs);
	}
	/// @}
	
	/// @name Multiplication operators
	/// @{
	/**
	 * Perform a multiplication involving a polynomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs * rhs`
	 */
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res *= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res *= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Monomial& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res *= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res *= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		MultivariatePolynomial<C,O,P> res(lhs);
		return res *= rhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs * lhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const Monomial& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs * lhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs * lhs;
	}
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs * lhs;
	}

	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	/// @}
	
} // namespace carl

/**
 * @ingroup multirp
 */
namespace std
{
	/**
	 * Specialization of `std::hash` for MultivariatePolynomial.
	 */
	template<typename C, typename O, typename P>
	struct hash<carl::MultivariatePolynomial<C,O,P>>
	{
		/**
		 * Calculates the hash of a MultivariatePolynomial.
		 * @param mpoly MultivariatePolynomial.
		 * @return Hash of mpoly.
		 */
		size_t operator()(const carl::MultivariatePolynomial<C,O,P>& mpoly) const 
		{
			size_t result = 0;
			std::hash<carl::Term<C>> h;
			for(auto iter = mpoly.begin(); iter != mpoly.end(); ++iter)
                        {
                            result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
                            result ^= h(**iter);
			}
                        return result;
		}
	};
} // namespace std

#include "MultivariatePolynomial.tpp"
