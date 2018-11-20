/**
 * @file UnivariatePolynomial.h 
 * @ingroup unirp
 * @author Sebastian Junges
 */

#pragma once

#include "../interval/Interval.h"
#include "../numbers/numbers.h"
#include "../util/SFINAE.h"
#include "Polynomial.h"
#include "Sign.h"
#include "Variable.h"
#include "VariableInformation.h"

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <vector>

namespace carl {
//
// Forward declarations
//
template<typename Coefficient> class UnivariatePolynomial;

template<typename Coefficient>
using UnivariatePolynomialPtr = std::shared_ptr<UnivariatePolynomial<Coefficient>>;

template<typename Coefficient>
using FactorMap = std::map<UnivariatePolynomial<Coefficient>, uint>;
}

#include "DivisionResult.h"
#include "MultivariatePolynomial.h"

#include "logging.h"

namespace carl
{

enum class PolynomialComparisonOrder {
	CauchyBound, LowDegree, Memory, Default = LowDegree
};
	
/**
 * This class represents a univariate polynomial with coefficients of an arbitrary type.
 *
 * A univariate polynomial is defined by a variable (the _main variable_) and the coefficients.
 * The coefficients may be of any type. The intention is to use a numbers or polynomials as coefficients.
 * If polynomials are used as coefficients, this can be seen as a multivariate polynomial with a distinguished main variable.
 *
 * Most methods are specifically adapted for polynomial coefficients, if necessary.
 * @ingroup unirp
 */
template<typename Coefficient>
class UnivariatePolynomial : public Polynomial
{
	/**
	 * Declare all instantiations of univariate polynomials as friends.
	 */
	template<class T> friend class UnivariatePolynomial; 
private:
	/// The main variable.
	Variable mMainVar;
	/// The coefficients.
	std::vector<Coefficient> mCoefficients;

public:
	/**
	 * The number type that is ultimately used for the coefficients.
	 */
	using NumberType = typename UnderlyingNumberType<Coefficient>::type;
	/**
	 * The integral type that belongs to the number type.
	 */
	using IntNumberType = typename IntegralType<NumberType>::type;
	
	using CACHE = void;
	using CoeffType = Coefficient;
	using PolyType = UnivariatePolynomial<Coefficient>;

	// Rule of five
	/**
	 * Default constructor shall not exist. Use UnivariatePolynomial(Variable) instead.
	 */
	UnivariatePolynomial() = delete;
	/**
	 * Copy constructor.
	 */
	UnivariatePolynomial(const UnivariatePolynomial& p);
	/**
	 * Move constructor.
	 */
	UnivariatePolynomial(UnivariatePolynomial&& p) noexcept;
	/**
	 * Copy assignment operator.
	 */
	UnivariatePolynomial& operator=(const UnivariatePolynomial& p);
	/**
	 * Move assignment operator.
	 */
	UnivariatePolynomial& operator=(UnivariatePolynomial&& p) noexcept;

	/**
	 * Construct a zero polynomial with the given main variable.
	 * @param mainVar New main variable.
	 */
	explicit UnivariatePolynomial(Variable mainVar);
	/**
	 * Construct \f$ coeff \cdot mainVar^{degree} \f$.
	 * @param mainVar New main variable.
	 * @param coeff Leading coefficient.
	 * @param degree Degree.
	 */
	UnivariatePolynomial(Variable mainVar, const Coefficient& coeff, std::size_t degree = 0);

	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients List of coefficients.
	 */
	UnivariatePolynomial(Variable mainVar, std::initializer_list<Coefficient> coefficients);

	/**
	 * Construct polynomial with the given coefficients from the underlying number type of the coefficient type.
	 * @param mainVar New main variable.
	 * @param coefficients List of coefficients.
	 */
	template<typename C = Coefficient, DisableIf<std::is_same<C, typename UnderlyingNumberType<C>::type>> = dummy>
	UnivariatePolynomial(Variable mainVar, std::initializer_list<typename UnderlyingNumberType<C>::type> coefficients);

	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Vector of coefficients.
	 */
	UnivariatePolynomial(Variable mainVar, const std::vector<Coefficient>& coefficients);
	/**
	 * Construct polynomial with the given coefficients, moving the coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Vector of coefficients.
	 */
	UnivariatePolynomial(Variable mainVar, std::vector<Coefficient>&& coefficients);
	/**
	 * Construct polynomial with the given coefficients.
	 * @param mainVar New main variable.
	 * @param coefficients Assignment of degree to coefficients.
	 */
	UnivariatePolynomial(Variable mainVar, const std::map<uint, Coefficient>& coefficients);

	/**
	 * Destructor.
	 */
	~UnivariatePolynomial() override = default;

	//Polynomial interface implementations.

	/**
	 * Checks if the polynomial is represented univariately.
	 * @see Polynomial#isUnivariateRepresented
	 * @return true.
	 */
	bool isUnivariateRepresented() const override
	{
		return true;
	}

	/**
	 * Checks if the polynomial is represented multivariately.
	 * @see Polynomial#isMultivariateRepresented
	 * @return false.
	 */
	bool isMultivariateRepresented() const override
	{
		return false;
	}

	/**
	 * Checks if the polynomial is equal to zero.
	 * @return If polynomial is zero.
	 */
	[[deprecated("use carl::isZero(p) instead.")]]
	bool isZero() const
	{
		return mCoefficients.size() == 0;
	}
	
	/**
	 * Checks if the polynomial is equal to one.
	 * @return If polynomial is one.
	 */
	[[deprecated("use carl::isOne(p) instead.")]]
	bool isOne() const
	{
		return mCoefficients.size() == 1 && mCoefficients.back() == Coefficient(1);
	}
	
	/**
	 * Creates a polynomial of value one with the same main variable.
	 * @return One.
	 */
	UnivariatePolynomial one() const {
		if constexpr (carl::is_instantiation_of<GFNumber, Coefficient>::value) {
			if (mCoefficients.empty()) {
				return UnivariatePolynomial(mMainVar, Coefficient(1));
			} else {
				return UnivariatePolynomial(mMainVar, Coefficient(1, lcoeff().gf()));
			}
		} else {
			return UnivariatePolynomial(mMainVar, Coefficient(1));
		}
	}
	
	/**
	 * Returns the leading coefficient.
	 * Asserts, that the polynomial is not empty.
	 * @return The leading coefficient.
	 */
	const Coefficient& lcoeff() const
	{
		assert(mCoefficients.size() > 0);
		return mCoefficients.back();
	}
	/**
	 * Returns the trailing coefficient.
	 * Asserts, that the polynomial is not empty.
	 * @return The trailing coefficient.
	 */
	const Coefficient& tcoeff() const {
		assert(mCoefficients.size() > 0);
		return mCoefficients.front();
	}

	/**
	 * Checks whether the polynomial is constant with respect to the main variable.
	 * @return If polynomial is constant.
	 */
	bool isConstant() const
	{
		assert(isConsistent());
		return mCoefficients.size() <= 1;
	}
	
	bool isLinearInMainVar() const
	{
		assert(isConsistent());
		return mCoefficients.size() <= 2;
	}

	/**
	 * Checks whether the polynomial is only a number.
	 * @return If polynomial is a number.
	 */
	bool isNumber() const
	{
		if constexpr (carl::is_number<Coefficient>::value) {
			return isConstant();
		} else {
			if (mCoefficients.empty()) return true;
			return isConstant() && lcoeff().isNumber();
		}
	}

	/**
	 * Returns the constant part of this polynomial.
	 * @return Constant part.
	 */
	NumberType constantPart() const
	{
		if (mCoefficients.empty()) return NumberType(0);
		if constexpr (carl::is_number<Coefficient>::value) {
			return tcoeff();
		} else {
			return tcoeff().constantPart();
		}
	}

	/**
	 * Checks if the polynomial is univariate, that means if only one variable occurs.
	 * @return true.
	 */
	bool isUnivariate() const {
		if constexpr (carl::is_number<Coefficient>::value) {
			return true;
		} else {
			for (const auto& c: coefficients()) {
				if (!c.isNumber()) return false;
			}
			return true;
		}
	}

	/**
	 * Get the maximal exponent of the main variable.
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 38
	 * @return Degree.
	 */
	uint degree() const {
		assert(!mCoefficients.empty());
		return uint(mCoefficients.size()-1);
	}
	
	/**
	 * Returns the total degree of the polynomial, that is the maximum degree of any monomial.
	 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
	 * @see @cite GCL92, page 38
	 * @return Total degree.
	 */
	uint totalDegree() const {
		if constexpr (carl::is_number<Coefficient>::value) {
			return degree();
		} else {
			if (carl::isZero(*this)) return 0;
			uint max = 0;
			for (std::size_t deg = 0; deg < mCoefficients.size(); deg++) {
				if (!mCoefficients[deg].isZero()) {
					uint tdeg = deg + mCoefficients[deg].totalDegree();
					if (tdeg > max) max = tdeg;
				}
			}
			return max;
		}
	}
	
	/**
	* @return An approximation of the complexity of this polynomial.
	*/
	std::size_t complexity() const {
		std::size_t result = 0;
		for (std::size_t deg = 0; deg < mCoefficients.size(); ++deg) {
			if (mCoefficients[deg].isZero()) continue;
			result += mCoefficients[deg].complexity() + deg;
		}
		return result;
	}

	/**
	 * Removes the leading term from the polynomial.
	 */
	void truncate() {
		assert(!mCoefficients.empty());
		this->mCoefficients.resize(this->mCoefficients.size()-1);
		this->stripLeadingZeroes();
	}

	/**
	 * Retrieves the coefficients defining this polynomial.
	 * @return Coefficients.
	 */
	const std::vector<Coefficient>& coefficients() const {
		return mCoefficients;
	}

	/**
	 * Retrieves the main variable of this polynomial.
	 * @return Main variable.
	 */
	Variable mainVar() const {
		return mMainVar;
	}

	/**
	 * Switches the main variable using a purely syntactical restructuring.
	 * The resulting polynomial will be algebraicly identical, but have the given variable as its main variable.
	 * @param newVar New main variable.
	 * @return Restructured polynomial.
	 */
	UnivariatePolynomial<MultivariatePolynomial<NumberType>> switchVariable(Variable newVar) const {
		assert(isConsistent());
		return MultivariatePolynomial<NumberType>(*this).toUnivariatePolynomial(newVar);
	}
	
	/**
	 * Replaces the main variable.
	 * @param newVar New main variable.
	 * @return New polynomial.
	 */
	UnivariatePolynomial replaceVariable(Variable newVar) const {
		if constexpr (carl::is_number<Coefficient>::value) {
			return UnivariatePolynomial<Coefficient>(newVar, mCoefficients);
		} else {
			return MultivariatePolynomial<NumberType>(*this).substitute(mainVar(), MultivariatePolynomial<NumberType>(newVar)).toUnivariatePolynomial(newVar);
		}
	}

	/**
	 * Gathers all variables that occur in the polynomial.
	 * @return Set of variables.
	 */
	std::set<Variable> gatherVariables() const {
		if constexpr (carl::is_number<Coefficient>::value) {
			return std::set<Variable>({mainVar()});
		} else {
			std::set<Variable> res({this->mainVar()});
			for (const auto& c: this->mCoefficients) {
				auto tmp = c.gatherVariables();
				res.insert(tmp.begin(), tmp.end());
			}
			return res;
		}
	}
	
	/**
	 * Gathers all variables that occur in the polynomial.
	 * @return Set of variables.
	 */
	void gatherVariables(std::set<Variable>& vars) const {
		vars = {mainVar()};
		if constexpr (!carl::is_number<Coefficient>::value) {
			for (const auto& c: mCoefficients) {
				auto tmp = c.gatherVariables();
				vars.insert(tmp.begin(), tmp.end());
			}
		}
	}

	/**
	 * Checks if the given variable occurs in the polynomial.
	 * @param v Variable.
	 * @return If v occurs in the polynomial.
	 */
	bool has(Variable v) const {
		if (v == mainVar()) return true;
		if constexpr (!carl::is_number<Coefficient>::value) {
			for (const auto& c: mCoefficients) {
				if (c.has(v)) return true;
			}
		}
		return false;
	}

	/**
	 * Calculates a factor that would make the coefficients of this polynomial coprime integers.
	 *
	 * We consider a set of integers coprime, if they share no common factor.
	 * Technically, the coprime factor is \f$ lcm(N) / gcd(D) \f$ where `N` is the set of the numerators and `D` is the set of the denominators of all coefficients.
	 * @return Coprime factor of this polynomial.
	 */
	template<typename C = Coefficient, EnableIf<is_subset_of_rationals<C>> = dummy>
	Coefficient coprimeFactor() const;
	template<typename C = Coefficient, DisableIf<is_subset_of_rationals<C>> = dummy>
	typename UnderlyingNumberType<Coefficient>::type coprimeFactor() const;

	/**
	 * Constructs a new polynomial that is scaled such that the coefficients are coprime.
	 * It is calculated by multiplying it with the coprime factor.
	 * By definition, this results in a polynomial with integral coefficients.
	 * @return This polynomial multiplied with the coprime factor.
	 */
	template<typename C = Coefficient, EnableIf<is_subset_of_rationals<C>> = dummy>
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> coprimeCoefficients() const;

	template<typename C = Coefficient, DisableIf<is_subset_of_rationals<C>> = dummy>
	UnivariatePolynomial<Coefficient> coprimeCoefficients() const;

	/**
	 * Checks whether the polynomial is unit normal.
	 * A polynomial is unit normal, if the leading coefficient is unit normal, that is if it is either one or minus one.
	 * @see @cite GCL92, page 39
	 * @return If polynomial is normal.
	 */
	bool isNormal() const;
	/**
	 * The normal part of a polynomial is the polynomial divided by the unit part.
	 * @see @cite GCL92, page 42.
	 * @return This polynomial divided by the unit part.
	 */
	UnivariatePolynomial normalized() const;
	
	/**
	 * The unit part of a polynomial over a field is its leading coefficient for nonzero polynomials, 
	 * and one for zero polynomials.
	 * @see @cite GCL92, page 42.
	 * @return The unit part of the polynomial.
	 */
	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	const Coefficient& unitPart() const;
	/**
	 * The unit part of a polynomial over a ring is the sign of the polynomial for nonzero polynomials, 
	 * and one for zero polynomials.
	 * @see @cite GCL92, page 42.
	 * @return The unit part of the polynomial.
	 */
#ifdef __VS
	template<typename C = Coefficient, EnableIfBool<!is_number<C>::value > = dummy>
	Coefficient unitPart() const;
#else
	template<typename C = Coefficient, EnableIf<Not<is_number<C>> > = dummy>
	Coefficient unitPart() const;
#endif
	/**
	 * The unit part of a polynomial over a ring is the sign of the polynomial for nonzero polynomials, 
	 * and one for zero polynomials.
	 * @see @cite GCL92, page 42.
	 * @return The unit part of the polynomial.
	 */
#ifdef __VS
	template<typename C = Coefficient, EnableIfBool<!is_field<C>::value && is_number<C>::value> = dummy>
	Coefficient unitPart() const;
#else
	template<typename C = Coefficient, EnableIf<Not<is_field<C>>, is_number<C>> = dummy>
	Coefficient unitPart() const;
#endif
	
	/**
	 * The n'th derivative of the polynomial in its main variable.
	 * @param nth how many times the derivative should be applied.
	 * @return A polynomial \f$(d/dx)^n p(x)\f$ where \f$p(x)\f$ is the input polynomial.
	 */
	[[deprecated("Use carl::derivative() instead.")]]
	UnivariatePolynomial derivative(uint nth = 1) const;

	UnivariatePolynomial remainder(const UnivariatePolynomial& divisor, const Coefficient& prefactor) const;
	UnivariatePolynomial remainder(const UnivariatePolynomial& divisor) const;
	/**
	 * Calculates the pseudo-remainder.
	 * @see @cite GCL92, page 55, Pseudo-Division Property
	 */
	UnivariatePolynomial prem_old(const UnivariatePolynomial& divisor) const;
	UnivariatePolynomial prem(const UnivariatePolynomial& divisor) const;
	UnivariatePolynomial sprem(const UnivariatePolynomial& divisor) const;

	/**
	 * Constructs a new polynomial `q` such that \f$ q(x) = p(-x) \f$ where `p` is this polynomial.
	 * @return New polynomial with negated variable.
	 */
	UnivariatePolynomial negateVariable() const {
		UnivariatePolynomial<Coefficient> res(*this);
		for (std::size_t deg = 0; deg < res.coefficients().size(); deg++) {
			if (deg % 2 == 1) res.mCoefficients[deg] = -res.mCoefficients[deg];
		}
		return res;
	}
	
	
	/**
	 * Divides the polynomial by another polynomial.
	 * Applies if the polynomial both have integer coefficients.
	 * @param divisor Divisor.
	 * @return this / divisor.
	 */
	template<typename C = Coefficient, EnableIf<is_integer<C>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const UnivariatePolynomial& divisor) const;

	/**
	 * Divides the polynomial by another polynomial.
	 * Applies if the polynomial both have coefficients over a field.
	 * @param divisor Divisor.
	 * @return this / divisor.
	 */
	template<typename C = Coefficient, DisableIf<is_integer<C>> = dummy, EnableIf<is_field<C>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const UnivariatePolynomial& divisor) const;

	/**
	 * Divides the polynomial by a coefficient.
	 * Applies if the polynomial has coefficients from a field.
	 * @param divisor Divisor.
	 * @return this / divisor.
	 */
	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const Coefficient& divisor) const;

	/**
	 * Divides the polynomial by a coefficient.
	 * If the divisor divides this polynomial, quotient contains the result of the division and true is returned.
	 * Otherwise, false is returned and the content of quotient is undefined.
	 * Applies if the polynomial has coefficients that are neither numeric nor from a field.
	 * @param divisor Divisor.
	 * @param quotient Resulting quotient.
	 * @return If remainder was zero.
	 */
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy, DisableIf<is_number<C>> = dummy>
	bool divideBy(const Coefficient& divisor, UnivariatePolynomial& quotient) const;

	/**
	 * Divides the polynomial by a number.
	 * Applies if the polynomial has coefficients that are polynomials with coefficients from a field.
	 * @param divisor Divisor.
	 * @return Quotient and remainder.
	 */
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy, DisableIf<is_number<C>> = dummy, EnableIf<is_field<typename UnderlyingNumberType<C>::type>> = dummy>
	DivisionResult<UnivariatePolynomial> divideBy(const NumberType& divisor) const;

	/**
	 * Checks if this polynomial is divisible by the given divisor, that is if the remainder is zero.
	 * @param divisor Polynomial.
	 * @return If divisor divides this polynomial.
	 */
	bool divides(const UnivariatePolynomial& divisor) const;
	
	/**
	 * Replaces every coefficient `c` by `c mod modulus`.
	 * @param modulus Modulus.
	 * @return This.
	 */
	UnivariatePolynomial& mod(const Coefficient& modulus);
	/**
	 * Constructs a new polynomial where every coefficient `c` is replaced by `c mod modulus`.
	 * @param modulus Modulus.
	 * @return New polynomial.
	 */
	UnivariatePolynomial mod(const Coefficient& modulus) const;

	/**
	 * Returns this polynomial to the given power.
	 * @param exp Exponent.
	 * @return This to the power of exp.
	 */
	UnivariatePolynomial pow(std::size_t exp) const;

	Coefficient evaluate(const Coefficient& value) const;
	
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	void substituteIn(Variable var, const Coefficient& value);
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	void substituteIn(Variable var, const Coefficient& value);

	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	UnivariatePolynomial substitute(Variable var, const Coefficient& value) const;
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial substitute(Variable var, const Coefficient& value) const;

	/**
	 * Calculates the sign of the polynomial at some point.
	 * @param value Point to evaluate.
	 * @return Sign at value.
	 */
	carl::Sign sgn(const Coefficient& value) const {
		return carl::sgn(this->evaluate(value));
	}
	bool isRoot(const Coefficient& value) const {
		return this->sgn(value) == Sign::ZERO;
	}
	
	template<typename SubstitutionType, typename C = Coefficient, EnableIf<is_instantiation_of<MultivariatePolynomial, C>> = dummy>
	UnivariatePolynomial<Coefficient> evaluateCoefficient(const std::map<Variable, SubstitutionType>&) const
	{
		CARL_LOG_NOTIMPLEMENTED();
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
		CARL_LOG_WARN("carl.core", "normalize not possible");
		return *this;
	}
	/**
	 * Works only from rationals, if the numbers are already integers.
	 * @return 
	 */
	template<typename C=Coefficient, EnableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> toIntegerDomain() const;
	template<typename C=Coefficient, DisableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> toIntegerDomain() const;
	
	UnivariatePolynomial<GFNumber<typename IntegralType<Coefficient>::type>> toFiniteDomain(const GaloisField<typename IntegralType<Coefficient>::type>* galoisField) const;

	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial<NumberType> toNumberCoefficients(bool check = true) const;

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
	[[deprecated("use carl::cauchyBound() or an alternative from RootBounds.h instead")]]
	Coefficient cauchyBound() const;
	Coefficient modifiedCauchyBound() const;

	/**
	 * The maximum norm of a polynomial is the maximum absolute value of the coefficients of
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
	NumberType numericContent(std::size_t i) const
	{
		if constexpr (carl::is_number<Coefficient>::value) {
			return this->mCoefficients[i];
		} else {
			return this->mCoefficients[i].numericContent();
		}
	}

	/**
	 * Returns the numeric unit part of the polynomial.
	 *
	 * If the coefficients are numbers, this is the sign of the leading coefficient.
	 * If the coefficients are polynomials, this is the unit part of the leading coefficient.s
	 * @return unit part of the polynomial.
	 */
	NumberType numericUnit() const
	{
		if constexpr (carl::is_number<Coefficient>::value) {
			return (this->lcoeff() >= Coefficient(0) ? NumberType(1) : NumberType(-1));
		} else {
			return this->lcoeff().numericUnit();
		}
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
	 * @see UnivariatePolynomials::numericContent(std::size_t)
	 */
	template<typename N=NumberType, EnableIf<is_subset_of_rationals<N>> = dummy>
	typename UnderlyingNumberType<Coefficient>::type numericContent() const;

	/**
	 * Returns this/divisor where divisor is the numeric content of this polynomial.
	 * @return 
	 */
	UnivariatePolynomial pseudoPrimpart() const {
		auto c = this->numericContent();
		if ((c == NumberType(0)) || (c == NumberType(1))) return *this;
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

	template<typename Integer>
	static UnivariatePolynomial excludeLinearFactors(const UnivariatePolynomial& poly, FactorMap<Coefficient>& linearFactors, const Integer& maxInt = 0 );

	Coefficient syntheticDivision(const Coefficient& zeroOfDivisor);

	/**
	 * Checks if zero is a real root of this polynomial.
	 * @return True if zero is a root.
	 */
	bool zeroIsRoot() const {
		return mCoefficients.empty() || carl::isZero(mCoefficients[0]);
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
	
public:
	[[deprecated("Use carl::sturm_sequence() instead.")]]
	std::vector<UnivariatePolynomial> standardSturmSequence() const;
	[[deprecated("Use carl::sturm_sequence() instead.")]]
	std::vector<UnivariatePolynomial> standardSturmSequence(const UnivariatePolynomial& polynomial) const;

	/**
	 * Counts the sign variations (i.e. an upper bound for the number of real roots) via Descarte's rule of signs.
	 * This is an upper bound for countRealRoots().
	 * @param interval Count roots within this interval.
	 * @return Upper bound for number of real roots within the interval.
	 */
	uint signVariations(const Interval<Coefficient>& interval) const;

	/**
	 * Count the number of real roots within the given interval using Sturm sequences.
	 * @param interval Count roots within this interval.
	 * @return Number of real roots within the interval.
	 */
	[[deprecated("Use carl::count_real_roots() instead.")]]
	int countRealRoots(const Interval<Coefficient>& interval) const;

	/**
	 * Calculate the number of real roots of a polynomial within a given interval based on a sturm sequence of this polynomial.
	 * @param seq Sturm sequence.
	 * @param interval Interval.
	 * @return Number of real roots in the interval.
	 */
	template<typename C = Coefficient, typename Number = typename UnderlyingNumberType<C>::type>
	static int countRealRoots(const std::vector<UnivariatePolynomial<Coefficient>>& seq, const Interval<Number>& interval);

	/// @name Equality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are equal.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs == rhs`
	 */
	template<typename C>
	friend bool operator==(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const C& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs);
	/// @}

	/// @name Inequality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are not equal.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs != rhs`
	 */
	template<typename C>
	friend bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator!=(const UnivariatePolynomialPtr<C>& lhs, const UnivariatePolynomialPtr<C>& rhs);
	/// @}
	
	bool less(const UnivariatePolynomial<Coefficient>& rhs, const PolynomialComparisonOrder& order = PolynomialComparisonOrder::Default) const;
	template<typename C>
	friend bool operator<(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);

	UnivariatePolynomial operator-() const;
	
	/// @name In-place addition operators
	/// @{
	/**
	 * Add something to this polynomial and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	UnivariatePolynomial& operator+=(const Coefficient& rhs);
	UnivariatePolynomial& operator+=(const UnivariatePolynomial& rhs);
	/// @}
	
	/// @name Addition operators
	/// @{
	/**
	 * Performs an addition involving a polynomial.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs + rhs`
	 */
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const C& rhs);
	/// @}
	
	/// @name In-place subtraction operators
	/// @{
	/**
	 * Subtract something from this polynomial and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	UnivariatePolynomial& operator-=(const Coefficient& rhs);
	UnivariatePolynomial& operator-=(const UnivariatePolynomial& rhs);
	/// @}
	
	/// @name Subtraction operators
	/// @{
	/**
	 * Performs a subtraction involving a polynomial.
	 * @param lhs First argument.
	 * @param rhs Second argument.
	 * @return `lhs - rhs`
	 */
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const C& rhs);
	/// @}
	
	/// @name In-place multiplication operators
	/// @{
	/**
	 * Multiply this polynomial with something and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	UnivariatePolynomial& operator*=(Variable rhs);
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	UnivariatePolynomial& operator*=(Variable rhs);
	UnivariatePolynomial& operator*=(const Coefficient& rhs);
	template<typename I = Coefficient, DisableIf<std::is_same<Coefficient, I>>...>
	UnivariatePolynomial& operator*=(const typename IntegralType<Coefficient>::type& rhs);
	UnivariatePolynomial& operator*=(const UnivariatePolynomial& rhs);
	/// @}

	/// @name Multiplication operators
	/// @{
	/**
	 * Perform a multiplication involving a polynomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs * rhs`
	 */
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, Variable rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(Variable lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const C& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const IntegralTypeIfDifferent<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const IntegralTypeIfDifferent<C>& rhs);
	template<typename C, typename O, typename P>
	friend UnivariatePolynomial<MultivariatePolynomial<C,O,P>> operator*(const UnivariatePolynomial<MultivariatePolynomial<C,O,P>>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend UnivariatePolynomial<MultivariatePolynomial<C,O,P>> operator*(const C& lhs, const UnivariatePolynomial<MultivariatePolynomial<C,O,P>>& rhs);
	/// @}

	/// @name In-place division operators
	/// @{
	/**
	 * Divide this polynomial by something and return the changed polynomial.
	 * @param rhs Right hand side.
	 * @return Changed polynomial.
	 */
	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	UnivariatePolynomial& operator/=(const Coefficient& rhs);
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
	UnivariatePolynomial& operator/=(const Coefficient& rhs);
	/// @}
	
	/// @name Division operators
	/// @{
	/**
	 * Perform a division involving a polynomial.
	 * @param lhs Left hand side.
	 * @param rhs Right hand side.
	 * @return `lhs / rhs`
	 */
	template<typename C>
	friend UnivariatePolynomial<C> operator/(const UnivariatePolynomial<C>& lhs, const C& rhs);
	/// @}
	
	/**
	 * Streaming operator for univariate polynomials.
	 * @param os Output stream.
	 * @param rhs Polynomial.
	 * @return `os`
	 */
	template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs);

	/**
	 * Asserts that this polynomial over numeric coefficients complies with the requirements and assumptions for UnivariatePolynomial objects.
	 * 
	 * <ul>
	 * <li>The leading term is not zero.</li>
	 * </ul>
	 */
	template<typename C=Coefficient, EnableIf<is_number<C>> = dummy>
	bool isConsistent() const;

	/**
	 * Asserts that this polynomial over polynomial coefficients complies with the requirements and assumptions for UnivariatePolynomial objects.
	 * 
	 * <ul>
	 * <li>The leading term is not zero.</li>
	 * <li>The main variable does not occur in any coefficient.</li>
	 * </ul>
	 */
	template<typename C=Coefficient, DisableIf<is_number<C>> = dummy>
	bool isConsistent() const;
private:
	
	/**
	 * Reverses the order of the coefficients of this polynomial.
	 * This method is meant to be called by signVariations only.
	 * @complexity O(n)
	 */
	void reverse();

	/**
	 * Scale the variable, i.e. apply \f$ x \rightarrow factor * x \f$
	 * This method is meant to be called by signVariations only.
	 * @param factor Factor to scale x.
	 * @complexity O(n)
	 */
	void scale(const Coefficient& factor);

	/**
	 * Shift the variable by a, i.e. apply \f$ x \rightarrow x + a \f$
	 * This method is meant to be called by signVariations only.
	 * @param a Offset to shift x.
	 * @complexity O(n^2)
	 */
	void shift(const Coefficient& a);	
	
	/**
	 * Calculates the remainder of polynomial division.
	 * @param divisor
	 * @param prefactor
	 * @see @cite GCL92, page 55, Pseudo-Division Property
	 * @return 
	 */
	UnivariatePolynomial remainder_helper(const UnivariatePolynomial& divisor, const Coefficient* prefactor = nullptr) const;
	
	void stripLeadingZeroes() 
	{
		while(mCoefficients.size() > 0 && carl::isZero(lcoeff()))
		{
			mCoefficients.pop_back();
		}
	}
};

/**
 * Checks if the polynomial is equal to zero.
 * @return If polynomial is zero.
 */
template<typename Coefficient>
bool isZero(const UnivariatePolynomial<Coefficient>& p) {
	return p.coefficients().size() == 0;
}

/**
 * Checks if the polynomial is equal to one.
 * @return If polynomial is one.
 */
template<typename Coefficient>
bool isOne(const UnivariatePolynomial<Coefficient>& p) {
	return p.coefficients().size() == 1 && carl::isOne(p.coefficients().front());
}

}

namespace std {
/**
 * Specialization of `std::hash` for univariate polynomials.
 */
template<typename Coefficient>
struct hash<carl::UnivariatePolynomial<Coefficient>> {
	/**
	 * Calculates the hash of univariate polynomial.
	 * @param p UnivariatePolynomial.
	 * @return Hash of p.
	 */
	std::size_t operator()(const carl::UnivariatePolynomial<Coefficient>& p) const {
		std::size_t result = 0;
		std::hash<Coefficient> h;
		for(auto c: p.coefficients()) {
			result ^= h(c);
		}
		return result;
	}
};

/**
 * Specialization of `std::less` for univariate polynomials.
 */
template<typename Coefficient>
struct less<carl::UnivariatePolynomial<Coefficient>> {
	carl::PolynomialComparisonOrder order;
	explicit less(carl::PolynomialComparisonOrder _order = carl::PolynomialComparisonOrder::Default) noexcept : order(_order) {}
	/**
	 * Compares two univariate polynomials.
	 * @param lhs First polynomial.
	 * @param rhs Second polynomial
	 * @return `lhs < rhs`.
	 */
	bool operator()(const carl::UnivariatePolynomial<Coefficient>& lhs, const carl::UnivariatePolynomial<Coefficient>& rhs) const {
		return lhs.less(rhs, order);
	}
	/**
	 * Compares two pointers to univariate polynomials.
	 * @param lhs First polynomial.
	 * @param rhs Second polynomial
	 * @return `lhs < rhs`.
	 */
	bool operator()(const carl::UnivariatePolynomial<Coefficient>* lhs, const carl::UnivariatePolynomial<Coefficient>* rhs) const {
		if (lhs == nullptr) return rhs != nullptr;
		if (rhs == nullptr) return true;
		return lhs->less(*rhs, order);
	}
	/**
	 * Compares two shared pointers to univariate polynomials.
	 * @param lhs First polynomial.
	 * @param rhs Second polynomial
	 * @return `lhs < rhs`.
	 */
	bool operator()(const carl::UnivariatePolynomialPtr<Coefficient>& lhs, const carl::UnivariatePolynomialPtr<Coefficient>& rhs) const {
		return (*this)(lhs.get(), rhs.get());
	}
};

}

#include "UnivariatePolynomial.tpp"
