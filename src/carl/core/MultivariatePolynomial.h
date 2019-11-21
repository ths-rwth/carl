/**
 * @file MultivariatePolynomial.h 
 * @ingroup multirp
 * @author Sebastian Junges
 * @author Florian Corzilius
 */

#pragma once

#include <algorithm>
#include <numeric>
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
    
	template <bool gatherCoeff>
	using VarInfo = VariableInformation<gatherCoeff, MultivariatePolynomial>;
private:
	/// A vector of all terms.
	mutable TermsType mTerms;
	/// Flag that indicates if the terms are ordered.
	mutable bool mOrdered;
public:
    ///
    static TermAdditionManager<MultivariatePolynomial,Ordering> mTermAdditionManager;
    
	enum class ConstructorOperation { ADD, SUB, MUL, DIV };
    friend std::ostream& operator<<(std::ostream& os, ConstructorOperation op) {
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
	MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, Policies>& p);
	MultivariatePolynomial(MultivariatePolynomial<Coeff, Ordering, Policies>&& p);
	MultivariatePolynomial& operator=(const MultivariatePolynomial& p);
	MultivariatePolynomial& operator=(MultivariatePolynomial&& p) noexcept;
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
	explicit MultivariatePolynomial(const UnivariatePolynomial<Coeff>& p);
	template<class OtherPolicies, DisableIf<std::is_same<Policies,OtherPolicies>> = dummy>
	explicit MultivariatePolynomial(const MultivariatePolynomial<Coeff, Ordering, OtherPolicies>& p);
	explicit MultivariatePolynomial(TermsType&& terms, bool duplicates = true, bool ordered = false);
	explicit MultivariatePolynomial(const TermsType& terms, bool duplicates = true, bool ordered = false);
	MultivariatePolynomial(const std::initializer_list<Term<Coeff>>& terms);
	MultivariatePolynomial(const std::initializer_list<Variable>& terms);
	explicit MultivariatePolynomial(const std::pair<ConstructorOperation, std::vector<MultivariatePolynomial>>& p);
    explicit MultivariatePolynomial(ConstructorOperation op, const std::vector<MultivariatePolynomial>& operands);
	/// @}
	
	~MultivariatePolynomial() noexcept override = default;
	
	//Polynomial interface implementations.
	/**
	 * @see class Polynomial
	 * @return 
	 */
	bool isUnivariateRepresented() const override {
		return false;
	}
	/**
	 * @see class Polynomial
	 * @return 
	 */
	bool isMultivariateRepresented() const override	{
		return true;
	}
	
	/**
	 * Check if the terms are ordered.
     * @return If terms are ordered.
     */
	bool isOrdered() const {
		return mOrdered;
	}
	void reset_ordered() const {
		mOrdered = false;
	}
	/**
	 * Ensure that the terms are ordered.
     */
	void makeOrdered() const {
		if (isOrdered()) return;
		std::sort(begin(), end(),
			[](const auto& lhs, const auto& rhs){ return Ordering::less(lhs, rhs); }
		);
		mOrdered = true;
        assert(this->isConsistent());
	}
	/**
	 * The leading term
	 * @return leading term.
	 */
	const Term<Coeff>& lterm() const {
		CARL_LOG_ASSERT("carl.core", nrTerms() > 0, "Leading term undefined on zero polynomials.");
		return mTerms.back();
	}
	Term<Coeff>& lterm(){
		CARL_LOG_ASSERT("carl.core", nrTerms() > 0, "Leading term undefined on zero polynomials.");
		return mTerms.back();
	}
	/**
	 * Returns the coefficient of the leading term.
	 * Notice that this is not defined for zero polynomials. 
	 * @return Leading coefficient.
	 */
	const Coeff& lcoeff() const {
		return lterm().coeff();
	}
	/**
	 * The leading monomial
	 * @return monomial of leading term.
	 */
	const Monomial::Arg& lmon() const {
		return lterm().monomial();
	}
	/**
	 * Returns the leading coefficient with respect to the given variable.
	 * @param var Variable.
	 * @return Leading coefficient.
	 */
	MultivariatePolynomial lcoeff(Variable::Arg var) const {
		return coeff(var, degree(var));
	}

	/**
	 * Give the last term according to Ordering. Notice that if there is a constant part, it is always trailing.
	 */
	const Term<Coeff>& trailingTerm() const {
		CARL_LOG_ASSERT("carl.core", nrTerms() > 0, "Trailing term undefined on zero polynomials.");
		return mTerms.front();
	}
	Term<Coeff>& trailingTerm() {
		CARL_LOG_ASSERT("carl.core", nrTerms() > 0, "Trailing term undefined on zero polynomials.");
		return mTerms.front();
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
	 * Check if the polynomial is zero.
	 */
	[[deprecated("use carl::isZero(p) instead.")]]
	bool isZero() const {
		return mTerms.empty();
	}
	/**
	 * 
     * @return 
     */
	[[deprecated("use carl::isOne(p) instead.")]]
	bool isOne() const {
		return (nrTerms() == 1) && carl::isOne(lterm());
	}
	/**
	 * Check if the polynomial is constant.
	 */
	bool isConstant() const {
		return (nrTerms() == 0) || ((nrTerms() == 1) && lterm().isConstant());
	}
	/**
	 * Check if the polynomial is a number, i.e., a constant.
	 */
	bool isNumber() const {
		return isConstant();
	}
	/**
	 * @return true, if this polynomial consists just of one variable (with coefficient 1). 
	 */
	bool isVariable() const {
		return (nrTerms() == 1) && carl::isOne(lcoeff()) && lterm().isSingleVariable();
	}
	/**
	 * Check if the polynomial is linear.
	 */
	bool isLinear() const;

	/**
	 * Calculate the number of terms.
	 */
	std::size_t nrTerms() const {
		return mTerms.size();
	}
    
    /**
     * @return A rough estimation of the size of this polynomial being the number of its terms.
     *         (Note, that this method is required, as it is provided of other polynomials not necessarily being straightforward.)
     */
    std::size_t size() const {
        return mTerms.size();
    }

	/**
	 * Check if the polynomial has a constant term that is not zero.
	 */
	bool hasConstantTerm() const {
		return (nrTerms() > 0) && trailingTerm().isConstant();
	}
    
    /**
     * @return true, if the image of this polynomial is integer-valued.
     */
    bool integerValued() const {
		return std::all_of(begin(), end(),
			[](const auto& t){ return t.integerValued(); }
		);
	}
    
	/**
	 * Retrieve the constant term of this polynomial or zero, if there is no constant term.
	 */
	const Coeff& constantPart() const;
	
	auto begin() const {
		return mTerms.begin();
	}
	auto end() const {
		return mTerms.end();
	}
	auto rbegin() const {
		return mTerms.rbegin();
	}
	auto rend() const {
		return mTerms.rend();
	}

	auto eraseTerm(typename TermsType::iterator pos) {
		///@todo find new lterm or constant term
		assert(false);
		return mTerms.erase(pos);
	}
    
	const TermsType& getTerms() const {
		return mTerms;
	}
	TermsType& getTerms() {
		return mTerms;
	}

	/**
	 * For the polynomial p, the function calculates a polynomial p - lt(p).
	 * The function assumes the polynomial to be nonzero, otherwise, lt(p) is not defined.
	 * @return A new polynomial p - lt(p).
	 */
	MultivariatePolynomial tail(bool makeFullyOrdered = false) const;
	/**
	 * Drops the leading term.
	 * The function assumes the polynomial to be nonzero, otherwise the leading term is not defined.
	 * @return  A reference to this.
	 */
	///@todo find new lterm
	MultivariatePolynomial& stripLT();
    
	bool hasSingleVariable() const {
		return gatherVariables().size() == 1;
	}
    
    /**
     * For terms with exactly one variable, get this variable.
     * @return The only variable occuring in the term.
     */
    Variable getSingleVariable() const {
        return lterm().getSingleVariable();
    }
    
    /**
     * @return Coefficient of the polynomial (this makes only sense for polynomials storing the gcd of all coefficients separately)
     */
    const CoeffType& coefficient() const {
        return constant_one<CoeffType>::get();
    }
    
    /**
     * @return The coprimeCoefficients of this polyomial, if this is stored internally, otherwise this polynomial.
     */
    const PolyType& polynomial() const {
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
	bool isTsos() const {
		return std::all_of(begin(), end(),
			[](const auto& t){ return t.isSquare(); }
		);
	}
	
	/**
	 * Iterates through all terms to find variables occuring in this polynomial.
	 * @param vars Holds the variables occuring in the polynomial at return.
	 */
	[[deprecated("Use carl::variables() instead.")]]
	void gatherVariables(std::set<Variable>& vars) const;
	[[deprecated("Use carl::variables() instead.")]]
	void gatherVariables(carlVariables& vars) const;
	[[deprecated("Use carl::variables() instead.")]]
	std::set<Variable> gatherVariables() const;
	
	/**
	 * @param v The variable to check for its occurrence.
	 * @return true, if the variable occurs in this term.
	 */
	bool has(Variable v) const {
		return std::any_of(begin(), end(),
			[v](const auto& t){ return t.has(v); }
		);
	}
	
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
	 * Calculating the quotient and the remainder, such that for a given polynomial p we have
	 * p = divisor * quotient + remainder.
	 * @param divisor Another polynomial
	 * @return A divisionresult, holding the quotient and the remainder.
	 * @see
	 * @note Division is only defined on fields
	 */
	[[deprecated("Use carl::divide() instead.")]]
	DivisionResult<MultivariatePolynomial> divideBy(const MultivariatePolynomial& divisor) const;

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
	 * @return p * |p.coprimeFactor()|
	 * @see coprimeCoefficients()
	 */
	MultivariatePolynomial coprimeCoefficientsSignPreserving() const;
	
	/**
	 * For a polynomial p, returns p/lc(p)
	 * @return 
	 */
	MultivariatePolynomial normalize() const;
	
	bool divides(const MultivariatePolynomial& b) const;

	MultivariatePolynomial<typename IntegralType<Coeff>::type, Ordering, Policies> toIntegerDomain() const;

	const Term<Coeff>& operator[](std::size_t index) const {
		assert(index < mTerms.size());
		return mTerms[index];
	}

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
	MultivariatePolynomial& operator+=(Variable rhs);
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
};

	template<typename C, typename O, typename P>
	bool isOne(const MultivariatePolynomial<C,O,P>& p) {
		return (p.nrTerms() == 1) && carl::isOne(p.lterm());
	}
	template<typename C, typename O, typename P>
	bool isZero(const MultivariatePolynomial<C,O,P>& p) {
		return p.nrTerms() == 0;
	}

	//template<typename C, typename O, typename P>
	//MultivariatePolynomial<C,O,P> pow(const MultivariatePolynomial<C,O,P>& p, std::size_t exp)
	//{
	//	return p.pow(exp);
	//}

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
        return MultivariatePolynomial<C,O,P>(lhs) /= rhs;
    }
	/// @}
    template<typename C, typename O, typename P>
    std::pair<MultivariatePolynomial<C,O,P>,MultivariatePolynomial<C,O,P>> lazyDiv( const MultivariatePolynomial<C,O,P>& _polyA, const MultivariatePolynomial<C,O,P>& _polyB )
    {
        MultivariatePolynomial<C,O,P> g = gcd( _polyA, _polyB );
        return std::make_pair( _polyA/g, _polyB/g );
    }
	
	/**
	 * Streaming operator for multivariate polynomials.
	 * @param os Output stream.
	 * @param rhs Polynomial.
	 * @return `os`.
	 */
	template<typename C, typename O, typename P>
	inline std::ostream& operator<<(std::ostream& os, const MultivariatePolynomial<C,O,P>& rhs) {
		if (isZero(rhs)) return os << "0";
		return os << carl::stream_joined(" + ", rhs);
	}

	/// Add the variables of the given polynomial to the variables.
	template<typename Coeff, typename Ordering, typename Policies>
	void variables(const MultivariatePolynomial<Coeff,Ordering,Policies>& p, carlVariables& vars) {
		for (const auto& t : p) {
			variables(t, vars);
		}
	}
} // namespace carl

/**
 * @ingroup multirp
 */
namespace std {
	/**
	 * Specialization of `std::hash` for MultivariatePolynomial.
	 */
	template<typename C, typename O, typename P>
	struct hash<carl::MultivariatePolynomial<C,O,P>> {
		/**
		 * Calculates the hash of a MultivariatePolynomial.
		 * @param mpoly MultivariatePolynomial.
		 * @return Hash of mpoly.
		 */

		std::size_t operator()(const carl::MultivariatePolynomial<C,O,P>& mpoly) const {
			assert(mpoly.isConsistent());
#ifdef true
			if (mpoly.nrTerms() == 0) return 0;
			if (mpoly.nrTerms() == 1) return carl::hash_all(mpoly[0]);
			std::size_t seed = 0;
			carl::hash_add(seed, mpoly[0]);
			for (std::size_t i = 1; i < mpoly.nrTerms() - 1; ++i) {
				seed = seed | carl::hash_all(mpoly[i]);
			}
			carl::hash_add(seed, mpoly[mpoly.nrTerms()-1]);
			return seed;
#else
			mpoly.makeOrdered();
			return std::accumulate(mpoly.begin(), mpoly.end(), static_cast<std::size_t>(0),
				[](std::size_t seed, const auto& t){ carl::hash_add(seed, t); return seed; }
			);
#endif
		}
	};
} // namespace std

#include "MultivariatePolynomial_operators.h"
#include "MultivariatePolynomial.tpp"
