/**
 * @file UnivariatePolynomial.h 
 * @ingroup unirp
 * @author Sebastian Junges
 */

#pragma once
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <vector>

#include "Variable.h"
#include "VariableInformation.h"
#include "Polynomial.h"
#include "DivisionResult.h"
#include "../numbers/numbers.h"
#include "../numbers/GFNumber.h"
#include "../interval/ExactInterval.h"

#include "logging.h"
#include "../util/SFINAE.h"

namespace carl
{

//
// Forward declarations
// 	
template<typename Coefficient> class UnivariatePolynomial;
	
template<typename Coefficient>
using UnivariatePolynomialPtr = std::shared_ptr<UnivariatePolynomial<Coefficient>>;

template<typename C, typename O, typename P>
class MultivariatePolynomial;

enum class PolynomialComparisonOrder : unsigned {
	CauchyBound, LowDegree, Memory, Default = Memory
};
enum class SubresultantStrategy : unsigned {
	Generic, Lazard, Ducos, Default = Lazard
};
	
/**
 * @ingroup unirp
 */
template<typename Coefficient>
class UnivariatePolynomial : public Polynomial
{
	
	template<class T> friend class UnivariatePolynomial; 
private:
	Variable mMainVar;
	std::vector<Coefficient> mCoefficients;

public:

	typedef typename UnderlyingNumberType<Coefficient>::type NumberType;
	typedef typename IntegralT<NumberType>::type IntNumberType;
	typedef Coefficient CoefficientType;

	UnivariatePolynomial(Variable::Arg mainVar);
	UnivariatePolynomial(Variable::Arg mainVar, const Coefficient& coeff, unsigned degree=0);
	UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coefficient> coefficients);

	template<typename C = Coefficient, DisableIf<std::is_same<C, typename UnderlyingNumberType<C>::type>> = dummy>
	UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<typename UnderlyingNumberType<C>::type> coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, const std::vector<Coefficient>& coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, std::vector<Coefficient>&& coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, const std::map<unsigned, Coefficient>& coefficients);
//	UnivariatePolynomial(Variable::Arg mainVar, const VariableInformation<true, Coefficient>& varinfoWithCoefficients);

	virtual ~UnivariatePolynomial();

	//Polynomial interface implementations.

	/**
	 * @see class Polynomial
	 * @return 
	 */
	virtual bool isUnivariateRepresented() const override
	{
		return true;
	}

	/**
	 * @see class Polynomial
	 * @return 
	 */
	virtual bool isMultivariateRepresented() const override
	{
		return false;
	}

	bool isZero() const
	{
		return mCoefficients.size() == 0;
	}
	
	bool isOne() const
	{
		return mCoefficients.size() == 1 && mCoefficients.back() == (Coefficient)1;
	}
	
	template<typename C=Coefficient, EnableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial one() const
	{
		return UnivariatePolynomial(mMainVar, C(1, lcoeff().gf()));
	}
	template<typename C=Coefficient, DisableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial one() const
	{
		return UnivariatePolynomial(mMainVar, (C)1);
	}
	
	/**
	 * Returns the leading coefficient.
	 * If the polynomial is empty, the return value is undefined.
	 * @return 
	 */
	const Coefficient& lcoeff() const
	{
		return this->mCoefficients.back();
	}
	/**
	 * Returns the trailing coefficient.
	 * If the polynomial is empty, the return value is undefined.
	 * @return 
	 */
	const Coefficient& tcoeff() const {
		return this->mCoefficients.front();
	}

	/**
	 * Checks whether the polynomial is constant with respect to the main variable.
	 * @return 
	 */
	bool isConstant() const
	{
		return mCoefficients.size() <= 1;
	}

	/**
	 * Checks whether the polynomial is only a number.
	 * Applies if the coefficients are numbers.
	 * @return
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	bool isNumber() const
	{
		return this->isConstant();
	}
	/**
	 * Checks whether the polynomial is only a number.
	 * Applies if the coefficients are not numbers.
	 * Calls isNumber() on the constant coefficient recursively.
	 * @return
	 */
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	bool isNumber() const
	{
		return this->isConstant() && this->lcoeff().isNumber();
	}

	/**
	 * Returns the constant part of this polynomial.
	 * Applies, if the coefficients are numbers.
	 * @return 
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	NumberType constantPart() const
	{
		return this->tcoeff();
	}
	/**
	 * Returns the constant part of this polynomial.
	 * Applies, if the coefficients are not numbers.
	 * Calls constantPart() on the trailing coefficient recursively.
	 * @return 
	 */
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	NumberType constantPart() const
	{
		if (this->isZero()) return 0;
		return this->tcoeff().constantPart();
	}

	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	bool isUnivariate() const {
		return true;
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	bool isUnivariate() const {
		for (auto c: this->coefficients()) {
			if (!c.isNumber()) return false;
		}
		return true;
	}

	/**
	 * Get the maximal exponent of the main variable.
	 * @return 
	 */
	unsigned degree() const
	{
		return mCoefficients.size() == 0 ? 0 : (unsigned)mCoefficients.size()-1;
	}
	
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	unsigned totalDegree() const {
		return this->degree();
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	unsigned totalDegree() const {
		unsigned max = 0;
		for (unsigned deg = 0; deg < this->mCoefficients.size(); deg++) {
			if (!this->mCoefficients[deg].isZero()) {
				unsigned tdeg = deg + this->mCoefficients[deg].totalDegree();
				if (tdeg > max) max = tdeg;
			}
		}
		return max;
	}

	/**
	 * Removes the leading term from the polynomial.
	 */
	void truncate() {
		this->mCoefficients.resize(this->mCoefficients.size()-1);
	}

	const std::vector<Coefficient>& coefficients() const
	{
		return mCoefficients;
	}

	const Variable& mainVar() const
	{
		return mMainVar;
	}

	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	UnivariatePolynomial switchVariable(const Variable& newVar) const {
		return MultivariatePolynomial<NumberType>(*this).toUnivariatePolynomial(newVar).toNumberCoefficients();
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial switchVariable(const Variable& newVar) const {
		return MultivariatePolynomial<NumberType>(*this).toUnivariatePolynomial(newVar);
	}
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	UnivariatePolynomial replaceVariable(const Variable& newVar) const {
		return UnivariatePolynomial<Coefficient>(newVar, this->mCoefficients);
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial replaceVariable(const Variable& newVar) const {
		return MultivariatePolynomial<NumberType>(*this).substitute(this->mainVar(), MultivariatePolynomial<NumberType>(newVar)).toUnivariatePolynomial(newVar);
	}

	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	std::set<Variable> gatherVariables() const {
		return std::set<Variable>({this->mainVar()});
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	std::set<Variable> gatherVariables() const {
		std::set<Variable> res({this->mainVar()});
		for (auto c: this->mCoefficients) {
			auto tmp = c.gatherVariables();
			res.insert(tmp.begin(), tmp.end());
		}
		return res;
	}

	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	unsigned has(Variable::Arg v) const {
		return v == this->mainVar();
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	unsigned has(Variable::Arg v) const {
		bool hasVar = v == this->mainVar();
		for (auto c: this->mCoefficients) {
			hasVar = hasVar || c.has(v);
		}
		return hasVar;
	}

	/**
	 * 
	 * @return copr
	 */
	Coefficient coprimeFactor() const;
	
	template<typename C = Coefficient, EnableIf<is_subset_of_rationals<C>> = dummy>
	UnivariatePolynomial<typename IntegralT<Coefficient>::type> coprimeCoefficients() const;

	/**
	 * Checks whether the polynomial is unit normal
	 * @see @ref GCL92, page 39
     * @return 
     */
	bool isNormal() const;
	/**
	 * The normal part of a polynomial is the polynomial divided by the unit part.
	 * @see @ref GCL92, page 42.
     * @return 
     */
	UnivariatePolynomial normalized() const;
	
	/**
	 * The unit part of a polynomial over a field is its leading coefficient for nonzero polynomials, 
	 * and one for zero polynomials.
	 * @see @ref GCL92, page 42.
     * @return The unit part of the polynomial.
     */
	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	const Coefficient& unitPart() const;
	/**
	 * The unit part of a polynomial over a ring is the sign of the polynomial for nonzero polynomials, 
	 * and one for zero polynomials.
	 * @see @ref GCL92, page 42.
     * @return 
     */
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
	Coefficient unitPart() const;
	
	/**
	 * The content of a polynomial is the gcd of the coefficients of the normal part of a polynomial.
	 * The content of zero is zero.
	 * @see @ref GCL92, page 52
     * @return 
     */
	Coefficient content() const;
	
	/**
	 * The primitive part of p is the normal part of p divided by the content of p.
	 * The primitive part of zero is zero.
	 * @see @ref GCL92, page 53
     * @return 
     */
	UnivariatePolynomial primitivePart() const;
	
	/**
	 * The n'th derivative of the polynomial in its main variable.
     * @param nth how many times the derivative should be applied.
     * @return A polynomial (d/dx)^n p(x) where p(x) is the input polynomial.
     */
	UnivariatePolynomial derivative(unsigned nth = 1) const;

	template<typename C = Coefficient, EnableIf<is_number<C>> = dummy>
	UnivariatePolynomial reduce(const UnivariatePolynomial& divisor, const Coefficient* prefactor = nullptr) const;
	template<typename C = Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial reduce(const UnivariatePolynomial& divisor, const Coefficient* prefactor = nullptr) const;
	UnivariatePolynomial prem(const UnivariatePolynomial& divisor) const;
	UnivariatePolynomial sprem(const UnivariatePolynomial& divisor) const;

	UnivariatePolynomial negateVariable() {
		UnivariatePolynomial<Coefficient> res(*this);
		for (unsigned int deg = 0; deg < res.coefficients().size(); deg++) {
			if (deg % 2 == 1) res.mCoefficients[deg] = -res.mCoefficients[deg];
		}
		return res;
	}
	
	
	/**
	 * Divides the polynomial by another polynomial.
	 * Applies if the polynomial both have integer coefficients.
	 * @param divisor
	 * @return 
	 */
	template<typename C = Coefficient, EnableIf<is_integer<C>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const UnivariatePolynomial& divisor) const;

	/**
	 * Divides the polynomial by another polynomial.
	 * Applies if the polynomial both have coefficients over a field.
	 * @param divisor
	 * @return 
	 */
	template<typename C = Coefficient, DisableIf<is_integer<C>> = dummy, EnableIf<is_field<C>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const UnivariatePolynomial& divisor) const;

	/**
	 * Divides the polynomial by a coefficient.
	 * Applies if the polynomial has coefficients from a field.
	 * @param divisor
	 * @return 
	 */
	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const Coefficient& divisor) const;

	/**
	 * Divides the polynomial by a coefficient.
	 * If the divisor divides this polynomial, quotient contains the result of the division and true is returned.
	 * Otherwise, false is returned and the content of quotient is undefined.
	 * Applies if the polynomial has coefficients that are neither numeric nor from a field.
	 * @param divisor
	 * @param quotient
	 * @return 
	 */
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy, DisableIf<is_number<C>> = dummy>
	bool divideBy(const Coefficient& divisor, UnivariatePolynomial& quotient) const;

	/**
	 * Divides the polynomial by a number.
	 * Applies if the polynomial has coefficients that are polynomials with coefficients from a field.
	 * @param divisor
	 * @return 
	 */
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy, DisableIf<is_number<C>> = dummy, EnableIf<is_field<typename UnderlyingNumberType<C>::type>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const NumberType& divisor) const;

	bool divides(const UnivariatePolynomial&) const;
	
	UnivariatePolynomial& mod(const Coefficient& modulus);
	UnivariatePolynomial mod(const Coefficient& modulus) const;
	static UnivariatePolynomial gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b);
	static UnivariatePolynomial extended_gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b,
											 UnivariatePolynomial& s, UnivariatePolynomial& t);

	template<typename C=Coefficient, EnableIf<is_subset_of_rationals<C>> = dummy>
	UnivariatePolynomial squareFreePart() const;
	template<typename C=Coefficient, DisableIf<is_subset_of_rationals<C>> = dummy>
	UnivariatePolynomial squareFreePart() const;
	
	Coefficient evaluate(const Coefficient& value) const;
	
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	void substituteIn(const Variable& var, const Coefficient& value);
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	void substituteIn(const Variable& var, const Coefficient& value);

	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	UnivariatePolynomial substitute(const Variable& var, const Coefficient& value) const;
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial substitute(const Variable& var, const Coefficient& value) const;

	/**
	 * 
     * @param value
     * @return 
     */
	carl::Sign sgn(const Coefficient& value) const {
		return carl::sgn(this->evaluate(value));
	}
	bool isRoot(const Coefficient& value) const {
		return this->sgn(value) == Sign::ZERO;
	}
	
	template<typename SubstitutionType, typename C = Coefficient, EnableIf<is_instantiation_of<MultivariatePolynomial, C>> = dummy>
	UnivariatePolynomial<typename CoefficientRing<Coefficient>::type> evaluateCoefficient(const std::map<Variable, SubstitutionType>&) const
	{
		LOG_NOTIMPLEMENTED();
	}
	template<typename SubstitutionType, typename C = Coefficient, DisableIf<is_instantiation_of<MultivariatePolynomial, C>> = dummy>
	UnivariatePolynomial<Coefficient> evaluateCoefficient(const std::map<Variable, SubstitutionType>&) const
	{
		// TODO check behaviour here. 
		return *this;
	}
	
	template<typename T = Coefficient, EnableIf<has_normalize<T>> = dummy>
	UnivariatePolynomial& normalizeCoefficients()
	{
		static_assert(std::is_same<T,Coefficient>::value, "No template parameters should be given");
		for(Coefficient& c : mCoefficients)
		{
			c.normalize();
		}
		return *this;
	}
	template<typename T = Coefficient, DisableIf<has_normalize<T>> = dummy>
	UnivariatePolynomial& normalizeCoefficients()
	{
		static_assert(std::is_same<T,Coefficient>::value, "No template parameters should be given");
		LOGMSG_WARN("carl.core", "normalize not possible");
		return *this;
	}
	/**
	 * Works only from rationals, if the numbers are already integers.
	 * @return 
	 */
	template<typename C=Coefficient, EnableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralT<Coefficient>::type> toIntegerDomain() const;
	template<typename C=Coefficient, DisableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralT<Coefficient>::type> toIntegerDomain() const;
	
	UnivariatePolynomial<GFNumber<typename IntegralT<Coefficient>::type>> toFiniteDomain(const GaloisField<typename IntegralT<Coefficient>::type>* galoisField) const;

	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial<NumberType> toNumberCoefficients() const;

	template<typename NewCoeff>
	UnivariatePolynomial<NewCoeff> convert() const;
	template<typename NewCoeff>
	UnivariatePolynomial<NewCoeff> convert(const std::function<NewCoeff(const Coefficient&)>& f) const;

	/**
	 * Notice, Cauchy bounds are only defined for polynomials over fields.
	 * 
	 * This is 
	 * @return 
	 */
	Coefficient cauchyBound() const;
	Coefficient modifiedCauchyBound() const;

	/** The maximum norm of a polynomial is the maximum absolute value of the coefficients of
	 * the corresponding integral polynomial (as calculated by coprimeCoefficients()).
	 * @return Maximum-norm of the polynomial in case it has numeric coefficients.
	 */
	template<typename C=Coefficient, EnableIf<is_subset_of_rationals<C>> = dummy>
	IntNumberType maximumNorm() const;

	/**
	 * Returns the numeric content part of the i'th coefficient.
	 *
	 * If the coefficients are numbers, this is simply the i'th coefficient.
	 * If the coefficients are polynomials, this is the numeric content part of the i'th coefficient.
	 * @param i number of the coefficient
	 * @return numeric content part of i'th coefficient.
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	NumberType numericContent(unsigned int i) const
	{
		return this->mCoefficients[i];
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	NumberType numericContent(unsigned int i) const
	{
		return this->mCoefficients[i].numericContent();
	}

	/**
	 * Returns the numeric unit part of the polynomial.
	 *
	 * If the coefficients are numbers, this is the sign of the leading coefficient.
	 * If the coefficients are polynomials, this is the unit part of the leading coefficient.s
	 * @return unit part of the polynomial.
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	NumberType numericUnit() const
	{
		return (this->lcoeff() >= 0 ? 1 : -1);
	}
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	NumberType numericUnit() const
	{
		return this->lcoeff().numericUnit();
	}

	/**
	 * Obtains the numeric content part of this polynomial.
	 *
	 * The numeric content part of a polynomial is defined as the gcd() of the numeric content parts of all coefficients.
	 * This is only possible if the underlying number type is either integral or fractional.
	 *
	 * As for fractional numbers, we consider the following definition:
	 *		gcd( a/b, c/d ) = gcd( a/b*l, c/d*l ) / l
	 * where l = lcm(b,d).
	 * @return numeric content part of the polynomial.
	 * @see UnivariatePolynomials::numericContent(unsigned int)
	 */
	template<typename N=NumberType, EnableIf<is_subset_of_rationals<N>> = dummy>
	typename UnderlyingNumberType<Coefficient>::type numericContent() const;

	/**
	 * Returns this/divisor where divisor is the numeric content of this polynomial.
	 * @return 
	 */
	UnivariatePolynomial pseudoPrimpart() const {
		auto c = this->numericContent();
		if ((c == 0) || (c == 1)) return *this;
		return this->divideBy(this->numericContent()).quotient;
	}

	/**
	 * Compute the main denominator of all numeric coefficients of this polynomial.
	 * This method only applies if the Coefficient type is a number.
	 * @return the main denominator of all coefficients of this polynomial.
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	IntNumberType mainDenom() const;
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	IntNumberType mainDenom() const;


	std::map<UnivariatePolynomial, unsigned> factorization() const;

	template<typename Integer>
	static UnivariatePolynomial excludeLinearFactors(const UnivariatePolynomial& _poly, std::map<UnivariatePolynomial, unsigned>& _linearFactors, const Integer& maxNum = 0 );

	Coefficient syntheticDivision(const Coefficient& _zeroOfDivisor);
	std::map<unsigned, UnivariatePolynomial> squareFreeFactorization() const;

	/**
	 * Checks if zero is a real root of this polynomial.
	 * @return True if zero is a root.
	 */
	bool zeroIsRoot() const {
		return this->mCoefficients[0] == 0;
	}
	/**
	 * Reduces the polynomial such that zero is not a root anymore.
	 * Is functionally equivalent to eliminateRoot(0), but much faster.
	 */
	void eliminateZeroRoots();
	/**
	 * Reduces the polynomial such that the given root is not a root anymore.
	 * The reduction is achieved by removing the linear factor (mainVar - root) from the polynomial, possibly multiple times.
	 *
	 * This method assumes that the given root is an actual real root of this polynomial.
	 * If this is not the case, i.e. <code>evaluate(root) != 0</code>, the polynomial will contain meaningless garbage.
	 * @param root Root to be eliminated.
	 */
	void eliminateRoot(const Coefficient& root);

	std::list<UnivariatePolynomial> standardSturmSequence() const;
	std::list<UnivariatePolynomial> standardSturmSequence(const UnivariatePolynomial& polynomial) const;

	/**
	 * Counts the sign variations (i.e. an upper bound for the number of real roots) via Descarte's rule of signs.
	 * This is an upper bound for countRealRoots().
	 * @param interval Count roots within this interval.
	 * @return Upper bound for number of real roots within the interval.
	 */
	unsigned int signVariations(const ExactInterval<Coefficient>& interval) const;

	/**
	 * Count the number of real roots within the given interval using Sturm sequences.
	 * @param interval Count roots within this interval.
	 * @return Number of real roots within the interval.
	 */
	int countRealRoots(const ExactInterval<Coefficient>& interval) const;

	/**
	 * Calculated the number of real roots of a polynomial within a given interval based on a sturm sequence of this polynomial.
	 * @param seq
	 * @param interval
	 * @return
	 */
	template<typename C = Coefficient, typename Number = typename UnderlyingNumberType<C>::type>
	static int countRealRoots(const std::list<UnivariatePolynomial<Coefficient>>& seq, const ExactInterval<Number>& interval);


	static const std::list<UnivariatePolynomial> subresultants(
			const UnivariatePolynomial& p,
			const UnivariatePolynomial& q,
			const SubresultantStrategy strategy = SubresultantStrategy::Default
	);

	static const std::vector<UnivariatePolynomial> principalSubresultantsCoefficients(
			const UnivariatePolynomial& p,
			const UnivariatePolynomial& q,
			const SubresultantStrategy strategy = SubresultantStrategy::Default
	);

	UnivariatePolynomial<Coefficient> resultant(
			const UnivariatePolynomial<Coefficient>& p,
			const SubresultantStrategy strategy = SubresultantStrategy::Default
	) const;

	UnivariatePolynomial<Coefficient> discriminant(const SubresultantStrategy strategy = SubresultantStrategy::Default) const;

	template<typename C>
	friend bool operator==(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const C& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs);
	template<typename C>
	friend bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator!=(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs);
	
	bool less(const UnivariatePolynomial<Coefficient>& rhs, const PolynomialComparisonOrder& order = PolynomialComparisonOrder::Default) const;
	template<typename C>
	friend bool less(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs, const PolynomialComparisonOrder& order);
	template<typename C>
	friend bool less(const UnivariatePolynomial<C>* lhs, const UnivariatePolynomial<C>* rhs, const PolynomialComparisonOrder&);
	template<typename C>
	friend bool less(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs, const PolynomialComparisonOrder&);
	template<typename C>
	friend bool operator<(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);

	UnivariatePolynomial operator-() const;
	UnivariatePolynomial& operator+=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator+=(const UnivariatePolynomial& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const C& rhs);
	
	UnivariatePolynomial& operator-=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator-=(const UnivariatePolynomial& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const C& rhs);
	
	
	UnivariatePolynomial& operator*=(const Coefficient& rhs);
	// TODO: does this make any sense?
	template<typename I = Coefficient, DisableIf<std::is_same<Coefficient, I>>...>
	UnivariatePolynomial& operator*=(const typename IntegralT<Coefficient>::type& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator*=(const UnivariatePolynomial& rhs);
	
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const C& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const typename IntegralT<C>::type& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const typename IntegralT<C>::type& rhs);
	template<typename C, typename O, typename P>
	friend UnivariatePolynomial<MultivariatePolynomial<C,O,P>> operator*(const UnivariatePolynomial<MultivariatePolynomial<C,O,P>>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend UnivariatePolynomial<MultivariatePolynomial<C,O,P>> operator*(const C& lhs, const UnivariatePolynomial<MultivariatePolynomial<C,O,P>>& rhs);
	
	

	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	UnivariatePolynomial& operator/=(const Coefficient& rhs);
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
	UnivariatePolynomial& operator/=(const Coefficient& rhs);
	
	
	template<typename C>
	friend UnivariatePolynomial<C> operator/(const UnivariatePolynomial<C>& lhs, const C& rhs);
	
	template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs);
private:
	
	/*!
	 * Reverses the order of the coefficients of this polynomial.
	 * This method is meant to be called by signVariations only.
	 * @complexity O(n)
	 */
	void reverse();

	/*!
	 * Scale the variable, i.e. apply <code>x -> factor * x</code>.
	 * This method is meant to be called by signVariations only.
	 * @param factor Factor to scale x.
	 * @complexity O(n)
	 */
	void scale(const Coefficient& factor);

	/*!
	 * Shift the variable by a, i.e. apply <code>x -> x + a</code>
	 * This method is meant to be called by signVariations only.
	 * @param a Offset to shift x.
	 * @complexity O(n^2)
	 */
	void shift(const Coefficient& a);	
		
	static UnivariatePolynomial gcd_recursive(const UnivariatePolynomial& p, const UnivariatePolynomial& q);
	void stripLeadingZeroes() 
	{
		while(!isZero() && lcoeff() == 0)
		{
			mCoefficients.pop_back();
		}
	}
};
}

#include "UnivariatePolynomial.tpp"
