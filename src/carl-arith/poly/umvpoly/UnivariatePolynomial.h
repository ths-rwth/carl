/**
 * @file UnivariatePolynomial.h 
 * @ingroup unirp
 * @author Sebastian Junges
 */

#pragma once

#include <carl-arith/numbers/numbers.h>
#include <carl-common/meta/SFINAE.h>
#include <carl-common/util/hash.h>
#include <carl-arith/core/Sign.h>
#include <carl-arith/core/Variable.h>

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <vector>

#include "../typetraits.h"

namespace carl {
//
// Forward declarations
//
template<typename Coefficient> class UnivariatePolynomial;

template<typename Coefficient> class IntRepRealAlgebraicNumber;

template<typename Coefficient>
using UnivariatePolynomialPtr = std::shared_ptr<UnivariatePolynomial<Coefficient>>;

template<typename Coefficient>
using FactorMap = std::map<UnivariatePolynomial<Coefficient>, uint>;
}

#include "MultivariatePolynomial.h"


#include <carl-logging/carl-logging.h>

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
class UnivariatePolynomial
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
	using RootType = IntRepRealAlgebraicNumber<NumberType>;

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
	~UnivariatePolynomial() = default;

	/**
	 * Checks if the polynomial is equal to zero.
	 * @return If polynomial is zero.
	 */
	[[deprecated("use carl::is_zero(p) instead.")]]
	bool is_zero() const
	{
		return mCoefficients.size() == 0;
	}
	
	/**
	 * Checks if the polynomial is equal to one.
	 * @return If polynomial is one.
	 */
	[[deprecated("use carl::is_one(p) instead.")]]
	bool is_one() const
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
	[[deprecated("use carl::is_constant(p) instead.")]]
	bool is_constant() const
	{
		assert(is_consistent());
		return mCoefficients.size() <= 1;
	}
	
	bool is_linear_in_main_var() const
	{
		assert(is_consistent());
		return mCoefficients.size() <= 2;
	}

	/**
	 * Checks whether the polynomial is only a number.
	 * @return If polynomial is a number.
	 */
	bool is_number() const
	{
		if constexpr (carl::is_number_type<Coefficient>::value) {
			return mCoefficients.size() <= 1;
		} else {
			if (mCoefficients.empty()) return true;
			return (mCoefficients.size() <= 1) && lcoeff().is_number();
		}
	}

	/**
	 * Returns the constant part of this polynomial.
	 * @return Constant part.
	 */
	NumberType constant_part() const
	{
		if (mCoefficients.empty()) return NumberType(0);
		if constexpr (carl::is_number_type<Coefficient>::value) {
			return tcoeff();
		} else {
			return tcoeff().constant_part();
		}
	}

	/**
	 * Checks if the polynomial is univariate, that means if only one variable occurs.
	 * @return true.
	 */
	bool is_univariate() const {
		if constexpr (carl::is_number_type<Coefficient>::value) {
			return true;
		} else {
			for (const auto& c: coefficients()) {
				if (!c.is_number()) return false;
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
	[[deprecated("use carl::total_degree(p) instead.")]]
	uint total_degree() const {
		if constexpr (carl::is_number_type<Coefficient>::value) {
			return degree();
		} else {
			if (is_zero()) return 0;
			uint max = 0;
			for (std::size_t deg = 0; deg < mCoefficients.size(); deg++) {
				if (!mCoefficients[deg].is_zero()) {
					uint tdeg = deg + mCoefficients[deg].total_degree();
					if (tdeg > max) max = tdeg;
				}
			}
			return max;
		}
	}

	/**
	 * Removes the leading term from the polynomial.
	 */
	void truncate() {
		assert(!mCoefficients.empty());
		this->mCoefficients.resize(this->mCoefficients.size()-1);
		this->strip_leading_zeroes();
	}

	/**
	 * Retrieves the coefficients defining this polynomial.
	 * @return Coefficients.
	 */
	const std::vector<Coefficient>& coefficients() const & {
		return mCoefficients;
	}
	/// Returns the coefficients as non-const reference.
	std::vector<Coefficient>& coefficients() & {
		return mCoefficients;
	}
	/// Returns the coefficients as rvalue. The polynomial may be in an undefined state afterwards!
	std::vector<Coefficient>&& coefficients() && {
		return std::move(mCoefficients);
	}

	/**
	 * Retrieves the main variable of this polynomial.
	 * @return Main variable.
	 */
	Variable main_var() const {
		return mMainVar;
	}

	/**
	 * Checks if the given variable occurs in the polynomial.
	 * @param v Variable.
	 * @return If v occurs in the polynomial.
	 */
	bool has(Variable v) const {
		if (v == main_var()) return true;
		if constexpr (!carl::is_number_type<Coefficient>::value) {
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
	template<typename C = Coefficient, EnableIf<is_subset_of_rationals_type<C>> = dummy>
	Coefficient coprime_factor() const;
	template<typename C = Coefficient, DisableIf<is_subset_of_rationals_type<C>> = dummy>
	typename UnderlyingNumberType<Coefficient>::type coprime_factor() const;

	/**
	 * Constructs a new polynomial that is scaled such that the coefficients are coprime.
	 * It is calculated by multiplying it with the coprime factor.
	 * By definition, this results in a polynomial with integral coefficients.
	 * @return This polynomial multiplied with the coprime factor.
	 */
	template<typename C = Coefficient, EnableIf<is_subset_of_rationals_type<C>> = dummy>
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> coprime_coefficients() const;

	template<typename C = Coefficient, DisableIf<is_subset_of_rationals_type<C>> = dummy>
	UnivariatePolynomial<Coefficient> coprime_coefficients() const;
	

	template<typename C = Coefficient, EnableIf<is_subset_of_rationals_type<C>> = dummy>
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> coprime_coefficients_sign_preserving() const;

	template<typename C = Coefficient, DisableIf<is_subset_of_rationals_type<C>> = dummy>
	UnivariatePolynomial<Coefficient> coprime_coefficients_sign_preserving() const;

	/**
	 * Checks whether the polynomial is unit normal.
	 * A polynomial is unit normal, if the leading coefficient is unit normal, that is if it is either one or minus one.
	 * @see @cite GCL92, page 39
	 * @return If polynomial is normal.
	 */
	bool is_normal() const;
	/**
	 * The normal part of a polynomial is the polynomial divided by the unit part.
	 * @see @cite GCL92, page 42.
	 * @return This polynomial divided by the unit part.
	 */
	UnivariatePolynomial normalized() const;
	
	/**
	 * The unit part of a polynomial over a field is its leading coefficient for nonzero polynomials, 
	 * and one for zero polynomials.
	 * The unit part of a polynomial over a ring is the sign of the polynomial for nonzero polynomials, 
	 * and one for zero polynomials.
	 * @see @cite GCL92, page 42.
	 * @return The unit part of the polynomial.
	 */
	Coefficient unit_part() const;

	/**
	 * Constructs a new polynomial `q` such that \f$ q(x) = p(-x) \f$ where `p` is this polynomial.
	 * @return New polynomial with negated variable.
	 */
	UnivariatePolynomial negate_variable() const {
		UnivariatePolynomial<Coefficient> res(*this);
		for (std::size_t deg = 0; deg < res.coefficients().size(); deg++) {
			if (deg % 2 == 1) res.mCoefficients[deg] = -res.mCoefficients[deg];
		}
		return res;
	}

	/**
	 * Reverse coefficients safely.
	 */
	UnivariatePolynomial reverse_coefficients() const {
		UnivariatePolynomial<Coefficient> res(*this);
		std::reverse(res.mCoefficients.begin(), res.mCoefficients.end());
		while(carl::is_zero(*std::prev(res.mCoefficients.end())) && std::prev(res.mCoefficients.end()) != res.mCoefficients.begin()) {
			res.mCoefficients.erase(std::prev(res.mCoefficients.end()));
		}
		assert(res.is_consistent());
		return res;
	}
	
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
	[[deprecated("Use carl::pow() instead.")]]
	UnivariatePolynomial pow(std::size_t exp) const;

	[[deprecated("Use carl::evaluate() instead.")]]
	Coefficient evaluate(const Coefficient& value) const;

	/**
	 * Calculates the sign of the polynomial at some point.
	 * @param value Point to evaluate.
	 * @return Sign at value.
	 */
	[[deprecated("Use carl::sgn(carl::evaluate()) instead.")]]
	carl::Sign sgn(const Coefficient& value) const {
		return carl::sgn(this->evaluate(value));
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
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> to_integer_domain() const;
	template<typename C=Coefficient, DisableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralType<Coefficient>::type> to_integer_domain() const;
	
	UnivariatePolynomial<GFNumber<typename IntegralType<Coefficient>::type>> toFiniteDomain(const GaloisField<typename IntegralType<Coefficient>::type>* galoisField) const;

	/**
	 * Asserts that is_univariate() is true.
	 */
	template<typename C=Coefficient, DisableIf<is_number_type<C>> = dummy>
	UnivariatePolynomial<NumberType> toNumberCoefficients() const;

	template<typename NewCoeff>
	UnivariatePolynomial<NewCoeff> convert() const;
	template<typename NewCoeff>
	UnivariatePolynomial<NewCoeff> convert(const std::function<NewCoeff(const Coefficient&)>& f) const;

	/**
	 * Returns the numeric content part of the i'th coefficient.
	 *
	 * If the coefficients are numbers, this is simply the i'th coefficient.
	 * If the coefficients are polynomials, this is the numeric content part of the i'th coefficient.
	 * @param i number of the coefficient
	 * @return numeric content part of i'th coefficient.
	 */
	NumberType numeric_content(std::size_t i) const
	{
		if constexpr (carl::is_number_type<Coefficient>::value) {
			return this->mCoefficients[i];
		} else {
			return this->mCoefficients[i].numeric_content();
		}
	}

	/**
	 * Returns the numeric unit part of the polynomial.
	 *
	 * If the coefficients are numbers, this is the sign of the leading coefficient.
	 * If the coefficients are polynomials, this is the unit part of the leading coefficient.s
	 * @return unit part of the polynomial.
	 */
	NumberType numeric_unit() const
	{
		if constexpr (carl::is_number_type<Coefficient>::value) {
			return (this->lcoeff() >= Coefficient(0) ? NumberType(1) : NumberType(-1));
		} else {
			return this->lcoeff().numeric_unit();
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
	 * @see UnivariatePolynomials::numeric_content(std::size_t)
	 */
	template<typename N=NumberType, EnableIf<is_subset_of_rationals_type<N>> = dummy>
	typename UnderlyingNumberType<Coefficient>::type numeric_content() const;

	/**
	 * Compute the main denominator of all numeric coefficients of this polynomial.
	 * This method only applies if the Coefficient type is a number.
	 * @return the main denominator of all coefficients of this polynomial.
	 */
	template<typename C=Coefficient, EnableIf<is_number_type<C>> = dummy>
	IntNumberType main_denom() const;
	template<typename C=Coefficient, DisableIf<is_number_type<C>> = dummy>
	IntNumberType main_denom() const;

	Coefficient synthetic_division(const Coefficient& zeroOfDivisor);

	/**
	 * Checks if zero is a real root of this polynomial.
	 * @return True if zero is a root.
	 */
	bool zero_is_root() const {
		assert(!mCoefficients.empty());
		return carl::is_zero(mCoefficients[0]);
	}

public:
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
	template<typename C=Coefficient, EnableIf<is_number_type<C>> = dummy>
	UnivariatePolynomial& operator*=(Variable rhs);
	template<typename C=Coefficient, DisableIf<is_number_type<C>> = dummy>
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
	template<typename C = Coefficient, EnableIf<is_field_type<C>> = dummy>
	UnivariatePolynomial& operator/=(const Coefficient& rhs);
	template<typename C = Coefficient, DisableIf<is_field_type<C>> = dummy>
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
	template<typename C=Coefficient, EnableIf<is_number_type<C>> = dummy>
	bool is_consistent() const;

	/**
	 * Asserts that this polynomial over polynomial coefficients complies with the requirements and assumptions for UnivariatePolynomial objects.
	 * 
	 * <ul>
	 * <li>The leading term is not zero.</li>
	 * <li>The main variable does not occur in any coefficient.</li>
	 * </ul>
	 */
	template<typename C=Coefficient, DisableIf<is_number_type<C>> = dummy>
	bool is_consistent() const;
	
	void strip_leading_zeroes() 
	{
		while(mCoefficients.size() > 0 && carl::is_zero(lcoeff()))
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
bool is_zero(const UnivariatePolynomial<Coefficient>& p) {
	return p.coefficients().size() == 0;
}

/**
 * Checks if the polynomial is equal to one.
 * @return If polynomial is one.
 */
template<typename Coefficient>
bool is_one(const UnivariatePolynomial<Coefficient>& p) {
	return p.coefficients().size() == 1 && carl::is_one(p.coefficients().front());
}

/// Add the variables of the given polynomial to the variables.
template<typename Coeff>
void variables(const UnivariatePolynomial<Coeff>& p, carlVariables& vars) {
	if (!is_constant(p)) vars.add(p.main_var());
	if constexpr (!carl::is_number_type<Coeff>::value) {
		for (const auto& c : p.coefficients()) {
			variables(c, vars);
		}
	}
}

template<typename T>
struct is_polynomial_type<carl::UnivariatePolynomial<T>>: std::true_type {};
/**
 * States that UnderlyingNumberType of UnivariatePolynomial<T> is UnderlyingNumberType<C>::type.
 * @ingroup typetraits_UnderlyingNumberType
 */
template<typename C>
struct UnderlyingNumberType<UnivariatePolynomial<C>>: has_subtype<typename UnderlyingNumberType<C>::type> {}; 

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
		return carl::hash_all(p.main_var(), p.coefficients());
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
