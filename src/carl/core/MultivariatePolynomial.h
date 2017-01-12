/**
 * @file MultivariatePolynomial.h 
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "DivisionResult.h"
#include "MultivariatePolynomialPolicy.h"
#include "Polynomial.h"
#include "Term.h"
#include "VariableInformation.h"
#include "../numbers/numbers.h"
#include "../util/TermAdditionManager.h"


namespace carl
{
// forward declaration of UnivariatePolynomials
template<typename Coeff>
class UnivariatePolynomial;

/**
 * The general-purpose multivariate polynomial class.
 *
 * It is represented as a sum of terms, being a coefficient and a monomial.
 *
 * A polynomial is always *minimally ordered*.
 * By that, we mean that the leading term and the constant term (if there is any) are at the correct positions.
 * For some operations, the terms may be *fully ordered*.
 * `isOrdered()` checks if the polynomial is *fully ordered* while `makeOrdered()` makes the polynomial *fully ordered*.
 * 
 * @ingroup multirp
 */
template<typename Coeff, typename Ordering = GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class MultivariatePolynomial : public Polynomial, public Policies
{
    template<typename Polynomial, typename Order>
    friend class TermAdditionManager;
public:
	/// The ordering of the terms.
	using OrderedBy = Ordering;
	/// Type of the terms.
	using TermType = Term<Coeff>;
	/// Type of the monomials within the terms.
	using MonomType = Monomial;
	/// Type of the coefficients. 	
	using CoeffType = Coeff;
	/// Policies for this monomial.
	using Policy = Policies;
	/// Number type within the coefficients.
	using NumberType = typename UnderlyingNumberType<Coeff>::type;
	/// Integer type associated with the number type.
	using IntNumberType = typename IntegralType<NumberType>::type;
    ///
    using PolyType = MultivariatePolynomial<Coeff, Ordering, Policies>;
    /// The type of the cache. Multivariate polynomials do not need a cache, we set it to something.
    using CACHE = std::vector<int>;
	/// Type our terms vector.f
	using TermsType = std::vector<Term<Coeff>>;
	
	template<typename C, typename T>
	using EnableIfNotSame = typename std::enable_if<!std::is_same<C,T>::value,T>::type;
    
protected:
    
	template <bool gatherCoeff>
	using VarInfo = VariableInformation<gatherCoeff, MultivariatePolynomial>;
protected:
	/// A vector of all terms.
	mutable TermsType mTerms;
	/// Flag that indicates if the terms are ordered.
	mutable bool mOrdered;
public:
    ///
    static TermAdditionManager<MultivariatePolynomial,Ordering> mTermAdditionManager;
    
	enum ConstructorOperation { ADD, SUB, MUL, DIV };
    friend inline std::ostream& operator<<(std::ostream& os, ConstructorOperation op) {
        switch (op) {
            case ConstructorOperation::ADD: return os << "+";
            case ConstructorOperation::SUB: return os << "-";
            case ConstructorOperation::MUL: return os << "*";
            case ConstructorOperation::DIV: return os << "/";
        }
        return os << "?";
    }
	
	/// @name Constructors
	/// @{
	MultivariatePolynomial();
	MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, Policies>&);
	MultivariatePolynomial(MultivariatePolynomial<Coeff, Ordering, Policies>&&);
	MultivariatePolynomial& operator=(const MultivariatePolynomial& p);
	MultivariatePolynomial& operator=(MultivariatePolynomial&& p);
	explicit MultivariatePolynomial(int c): MultivariatePolynomial(sint(c)) {}
	template<typename C = Coeff>
	explicit MultivariatePolynomial(EnableIfNotSame<C,sint> c);
	template<typename C = Coeff>
	explicit MultivariatePolynomial(EnableIfNotSame<C,uint> c);
	explicit MultivariatePolynomial(const Coeff& c);
	explicit MultivariatePolynomial(Variable::Arg v);
	explicit MultivariatePolynomial(const Term<Coeff>& t);
	explicit MultivariatePolynomial(const std::shared_ptr<const Monomial>& m);
	explicit MultivariatePolynomial(const UnivariatePolynomial<MultivariatePolynomial<Coeff, Ordering,Policy>> &pol);
	explicit MultivariatePolynomial(const UnivariatePolynomial<Coeff>& pol);
	template<class OtherPolicies, DisableIf<std::is_same<Policies,OtherPolicies>> = dummy>
	explicit MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicies>&);
	explicit MultivariatePolynomial(TermsType&& terms, bool duplicates = true, bool ordered = false);
	explicit MultivariatePolynomial(const TermsType& terms, bool duplicates = true, bool ordered = false);
	explicit MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms);
	explicit MultivariatePolynomial(const std::initializer_list<Variable>& terms);
	explicit MultivariatePolynomial(const std::pair<ConstructorOperation, std::vector<MultivariatePolynomial>>& p);
    explicit MultivariatePolynomial(ConstructorOperation op, const std::vector<MultivariatePolynomial>& operands);
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
	 * Check if the terms are ordered.
     * @return If terms are ordered.
     */
	bool isOrdered() const {
		return mOrdered;
	}
	/**
	 * Ensure that the terms are ordered.
     */
	inline void makeOrdered() const {
		if (isOrdered()) return;
		std::sort(mTerms.begin(), mTerms.end(), (bool (&)(Term<Coeff> const&, Term<Coeff> const&))Ordering::less);
		mOrdered = true;
        assert(this->isConsistent());
	}
	
	/**
	 * The leading monomial
	 * @return 
	 */
	const Monomial::Arg& lmon() const;
	/**
	 * The leading term
	 * @return 
	 */
	const Term<Coeff>& lterm() const;
	Term<Coeff>& lterm();
	/**
	 * Returns the coefficient of the leading term.
	 * Notice that this is not defined for zero polynomials. 
	 * @return Leading coefficient.
	 */
	const Coeff& lcoeff() const;
	/**
	 * Returns the leading coefficient with respect to the given variable.
	 * @param var Variable.
	 * @return Leading coefficient.
	 */
	MultivariatePolynomial lcoeff(Variable::Arg var) const {
		return coeff(var, degree(var));
	}
	/**
	 * Calculates the max. degree over all monomials occurring in the polynomial.
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 48
	 * @return Total degree.
	 */
	std::size_t totalDegree() const;

	/**
	 * Calculates the degree of this polynomial with respect to the given variable.
	 * @param var Variable.
	 * @return Degree w.r.t. var.
	 */
	std::size_t degree(Variable::Arg var) const {
		std::size_t max = 0;
		for (const auto& t: mTerms) {
			if (t.monomial() == nullptr) continue;
			std::size_t c = t.monomial()->exponentOfVariable(var);
			if (c > max) max = c;
		}
		return max;
	}

	/**
	 * Calculates the coefficient of var^exp.
	 * @param var Variable.
	 * @param exp Exponent.
	 * @return Coefficient of var^exp.
	 */
	MultivariatePolynomial coeff(Variable::Arg var, std::size_t exp) const {
		MultivariatePolynomial res;
		for (const auto& t: mTerms) {
			if (t.monomial() == nullptr) {
				if (exp == 0) res.mTerms.push_back(t);
			} else if (t.monomial()->exponentOfVariable(var) == exp) {
				Monomial::Arg newMon = t.monomial()->dropVariable(var);
				res.mTerms.push_back(TermType(t.coeff(), newMon));
			}
		}
		res.mOrdered = false;
		res.makeMinimallyOrdered<true,true>();
		return res;
	}

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
	Definiteness definiteness( bool _fullEffort = true ) const;
    
    /**
     * @param _notTrivial If this flag is true, this polynomial is not yet in form of a sos (the method than avoids checking this).
     * @return The sum-of-squares (sos) decomposition ((q1,p1), .., (qn,pn)) with this = q1*p1^2+..+qn*pn^2, qi being positive rational numbers and pi being polynomials.
     *          If the result is empty, no sos could be found (which does not mean, that there exists no one).
     */
    std::vector<std::pair<Coeff,MultivariatePolynomial<Coeff,Ordering,Policies>>> sosDecomposition( bool _notTrivial = false ) const;

	/**
	 * Calculates the number of terms.
	 * @return the number of terms
	 */
	size_t nrTerms() const
	{
		return mTerms.size();
	}
    
    /**
     * @return A rough estimation of the size of this polynomial being the number of its terms.
     *         (Note, that this method is required, as it is provided of other polynomials not necessarily being straightforward.)
     */
    size_t size() const
    {
        return mTerms.size();
    }
    
    /**
     * @return An approximation of the complexity of this polynomial.
     */
    size_t complexity() const
    {
        size_t result = 0;
        for(const auto& term : mTerms)
            result += term.complexity();
        return result;
    }
    
	/**
	 * Gives the last term according to Ordering. Notice that if there is a constant part, it is always trailing.
	 * @return 
	 */
	const Term<Coeff>& trailingTerm() const;
	Term<Coeff>& trailingTerm();
	/**
	 * Checks if the polynomial has a constant term that is not zero.
	 * @return If there is a constant term unequal to zero.
	 */
	bool hasConstantTerm() const;
    
    /**
     * @return true, if the image of this polynomial is integer-valued.
     */
    bool integerValued() const;
    
	/**
	 * Retrieves the constant term of this polynomial or zero, if there is no constant term.
	 * @return Constant term.
	 */
	const Coeff& constantPart() const;
	
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
		///@todo find new lterm or constant term
		assert(false);
		return mTerms.erase(pos);
	}
    
    //TODO: This cannot be compiled with gcc 4.8.1
//	typename TermsType::const_iterator eraseTerm(typename TermsType::const_iterator pos) {
//		///@todo find new lterm or constant term
//		assert(false);
//		return mTerms.erase(pos);
//	}
	TermsType& getTerms() {
		return mTerms;
	}

	/**
	 * For the polynomial p, the function calculates a polynomial p - lt(p).
	 * The function assumes the polynomial to be nonzero, otherwise, lt(p) is not defined.
	 * @return A new polynomial p - lt(p).
	 */
	MultivariatePolynomial tail(bool fullOrdered = false) const;
	/**
	 * Drops the leading term.
	 * The function assumes the polynomial to be nonzero, otherwise the leading term is not defined.
	 * @return  A reference to this.
	 */
	///@todo find new lterm
	MultivariatePolynomial& stripLT();
    
    /**
     * @return true, if this polynomial consists just of one variable (with coefficient 1). 
     */
    bool isVariable() const
    {
        return !mTerms.empty() && totalDegree() == 1 && nrTerms() == 1 && lcoeff() == constant_one<CoeffType>::get();
    }
    
    /**
     * For terms with exactly one variable, get this variable.
     * @return The only variable occuring in the term.
     */
    Variable::Arg getSingleVariable() const
    {
        assert( !isConstant() );
        return lterm().getSingleVariable();
    }
    
    /**
     * @return Coefficient of the polynomial (this makes only sense for polynomials storing the gcd of all coefficients separately)
     */
    const CoeffType& coefficient() const
    {
        return constant_one<CoeffType>::get();
    }
    
    /**
     * @return The coprimeCoefficients of this polyomial, if this is stored internally, otherwise this polynomial.
     */
    const PolyType& polynomial() const
    {
        return *this;
    }
	
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
	 * Subtract a term times a polynomial from this polynomial.
	 * @param factor Term.
	 * @param p Polynomial.
	 */
	void subtractProduct(const Term<Coeff>& factor, const MultivariatePolynomial& p);
	
	/**
	 * Adds a single term without using a TermAdditionManager or changing the ordering status.
	 * @param term Term.
	 */
	void addTerm(const Term<Coeff>& term);

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
	MultivariatePolynomial prem(const MultivariatePolynomial& divisor, Variable::Arg var) const;
	
	
	MultivariatePolynomial derivative(Variable::Arg v, unsigned nth=1) const;
    
    /**
     * Calculates the square of this multivariate polynomial if it is a square.
     * @param res Used to store the result in.
     * @return true, if this multivariate polynomial is a square; false, otherwise.
     */
    bool sqrt(MultivariatePolynomial& res) const;
	
	/**
	 * @return The lcm of the denominators of the coefficients in p divided by the gcd of numerators 
	 *		 of the coefficients in p.
	 */
	template<typename C = Coeff, EnableIf<is_subset_of_rationals<C>> = dummy>
	Coeff coprimeFactor() const;
    
    /**
	 * @return The lcm of the denominators of the coefficients (without the constant one) in p divided by the gcd of numerators 
	 *		 of the coefficients in p.
	 */
	template<typename C = Coeff, EnableIf<is_subset_of_rationals<C>> = dummy>
	Coeff coprimeFactorWithoutConstant() const;
	
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
	///@todo find new lterm
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
	SubstitutionType evaluate(const std::map<Variable, SubstitutionType>& substitutions) const;
	
	bool divides(const MultivariatePolynomial& b) const;
	/**
	 * Calculates the S-Polynomial.
	 * @param p
	 * @param q
	 * @return 
	 */
	static MultivariatePolynomial SPolynomial(const MultivariatePolynomial& p, const MultivariatePolynomial& q);

	void square();

	MultivariatePolynomial pow(std::size_t exp) const;
	
	MultivariatePolynomial naive_pow(unsigned exp) const;
	
	std::string toString(bool infix=true, bool friendlyVarNames=true) const;
	
	MultivariatePolynomial<typename IntegralType<Coeff>::type, Ordering, Policies> toIntegerDomain() const;
	
	UnivariatePolynomial<Coeff> toUnivariatePolynomial() const;
	UnivariatePolynomial<MultivariatePolynomial> toUnivariatePolynomial(Variable::Arg mainVar) const;
	
	const Term<Coeff>& operator[](unsigned) const;

	MultivariatePolynomial mod(const typename IntegralType<Coeff>::type& modulo) const;
	
	template<bool gatherCoeff>
	VariableInformation<gatherCoeff, MultivariatePolynomial> getVarInfo(Variable::Arg v) const;

	template<bool gatherCoeff>
	VariablesInformation<gatherCoeff, MultivariatePolynomial> getVarInfo() const;
	
	template<typename C=Coeff, EnableIf<is_number<C>> = dummy>
	Coeff numericContent() const;
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
	MultivariatePolynomial& operator+=(const Monomial::Arg& rhs);
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
	MultivariatePolynomial& operator-=(const Monomial::Arg& rhs);
	MultivariatePolynomial& operator-=(Variable::Arg rhs);
	MultivariatePolynomial& operator-=(const Coeff& rhs);
	/// @}


	MultivariatePolynomial operator-() const;

	/// @name In-place multiplication operators
	/// @{
	/**
	 * Multiply this polynomial with something and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	MultivariatePolynomial& operator*=(const MultivariatePolynomial& rhs);
	MultivariatePolynomial& operator*=(const Term<Coeff>& rhs);
	MultivariatePolynomial& operator*=(const Monomial::Arg& rhs);
	MultivariatePolynomial& operator*=(Variable::Arg rhs);
	MultivariatePolynomial& operator*=(const Coeff& rhs);
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
	MultivariatePolynomial& operator/=(const Monomial::Arg& rhs);
	MultivariatePolynomial& operator/=(Variable::Arg rhs);
	MultivariatePolynomial& operator/=(const Coeff& rhs);
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
	friend MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	
	template<typename C, typename O, typename P>
	friend MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, unsigned long rhs);
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

	
	/**
	 * Make sure that the terms are at least minimally ordered.
	 */
	template<bool findConstantTerm=true, bool findLeadingTerm=true> 
	void makeMinimallyOrdered() const;
private:
	/**
	 * Make sure that the terms are at least minimally ordered.
	 * Iterators to the important terms are given as arguments, so that we don't need to scan the whole vector.
	 * @param lterm Iterator to leading term.
	 * @param cterm Iterator to constant term.
	 */
	void makeMinimallyOrdered(typename TermsType::iterator& lterm, typename TermsType::iterator& cterm) const;

public:
	/**
	 * Asserts that this polynomial complies with the requirements and assumptions for MultivariatePolynomial objects.
	 * 
	 * <ul>
	 * <li>All terms are actually valid and not nullptr or alike</li>
	 * <li>Only the trailing term may be constant.</li>
	 * </ul>
	 */
	bool isConsistent() const;
	
	template<typename C, typename O, typename P>
	friend bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
};

#define MPOpTemplate typename C, typename O = GrLexOrdering, typename P = StdMultivariatePolynomialPolicies<>

	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> quotient(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q)
	{
		return p.quotient(q);
	}

	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> pow(const MultivariatePolynomial<C,O,P>& p, std::size_t exp)
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
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P, DisableIf<std::is_integral<C>> = dummy>
	bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int rhs);
	template<typename C, typename O, typename P>
	inline bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return rhs == lhs;
	}
	template<typename C, typename O, typename P>
	inline bool operator==(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
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
	inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
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
	inline bool operator!=(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
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
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs);
	template<typename C, typename O, typename P>
	bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	bool operator<(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs);
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
	inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
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
	inline bool operator<=(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
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
	inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
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
	inline bool operator>(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
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
	inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
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
	inline bool operator>=(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
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
	inline MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) += rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) += rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) += rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) += rhs);
	}
	template<typename C, typename O, typename P, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) += rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator+(const Term<C>& lhs, const C& rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const Monomial::Arg& lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const Monomial::Arg& lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator+(const Monomial::Arg& lhs, const C& rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C>(lhs) += rhs);
	}
    template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
    inline MultivariatePolynomial<C,O,P> operator+(Variable::Arg lhs, const C& rhs) {
        return std::move(MultivariatePolynomial<C>(lhs) += rhs);
    }
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator+(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const C& lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C>(rhs) += lhs);
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator+(const C& lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C>(rhs) += lhs);
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator+(const C& lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C>(rhs) += lhs);
	}

	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs);
	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> operator+(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C >> &rhs);
	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> operator+(const UnivariatePolynomial<MultivariatePolynomial<C >> &lhs, const MultivariatePolynomial<C,O,P>& rhs);
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
	inline MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) -= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) -= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) -= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) -= rhs);
	}
	template<typename C, typename O, typename P, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) -= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move( -(rhs - lhs) );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const Term<C>& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const Monomial::Arg& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, Variable::Arg rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const Term<C>& lhs, const C& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator-(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move( -(rhs - lhs) );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(const Monomial::Arg& lhs, const Term<C>& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(const Monomial::Arg& lhs, const Monomial::Arg& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(const Monomial::Arg& lhs, Variable::Arg rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const Monomial::Arg& lhs, const C& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move( -(rhs - lhs) );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const Term<C>& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const Monomial::Arg& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate>
	inline MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, Variable::Arg rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(Variable::Arg lhs, const C& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<typename C, typename O, typename P, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move( -(rhs - lhs) );
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const C& lhs, const Term<C>& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const C& lhs, const Monomial::Arg& rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
	}
	template<MPOpTemplate, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator-(const C& lhs, Variable::Arg rhs) {
		return std::move( MultivariatePolynomial<C>(lhs) -= rhs );
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
	inline MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, Variable::Arg rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator*(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator*(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator*(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P>
	inline MultivariatePolynomial<C,O,P> operator*(Variable::Arg lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(lhs) *= rhs);
	}
	template<typename C, typename O, typename P, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator*(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
		return std::move(MultivariatePolynomial<C,O,P>(rhs) *= lhs);
	}

	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C, O, P> operator*(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C, O, P>& rhs);
	template<typename C, typename O, typename P>
	inline const MultivariatePolynomial<C, O, P> operator*(const MultivariatePolynomial<C, O, P>& lhs, const UnivariatePolynomial<C>& rhs);
	/// @}
    
    /// @name Division operators
	/// @{
	/**
	 * Perform a division involving a polynomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs / rhs`
	 */
	template<typename C, typename O, typename P, EnableIf<carl::is_number<C>> = dummy>
	inline MultivariatePolynomial<C,O,P> operator/(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
        return std::move(MultivariatePolynomial<C,O,P>(lhs) /= rhs);
    }
	/// @}
    template<typename C, typename O, typename P>
    std::pair<MultivariatePolynomial<C,O,P>,MultivariatePolynomial<C,O,P>> lazyDiv( const MultivariatePolynomial<C,O,P>& _polyA, const MultivariatePolynomial<C,O,P>& _polyB )
    {
        MultivariatePolynomial<C,O,P> g = gcd( _polyA, _polyB );
        return std::make_pair( _polyA/g, _polyB/g );
    }
	
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
			assert(mpoly.isConsistent());
			std::hash<carl::Term<C>> h;
			size_t result = 0;
			for(auto it = mpoly.begin(); it != mpoly.end(); ++it) {
				if (it == (mpoly.end()-1)) {
					// Shift before leading term
					result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
				}
				result ^= h(*it);
				if (it == mpoly.begin() && it->isConstant()) {
					// Shift after constant term
					result = (result << 5) | (result >> (sizeof(size_t)*8 - 5));
				}
			}
			return result;
		}
	};
} // namespace std

#include "MultivariateGCD.h"
#include "MultivariateFactor.h"
#include "MultivariatePolynomial.tpp"
