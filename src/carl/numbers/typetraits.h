/**
 * @file   numbers/typetraits.h
 * @ingroup typetraits
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 *
 * @addtogroup typetraits Type Traits
 * We define custom type traits for number types we use.
 * We use the notation conventions of the STL, being lower cases with underscores.
 *
 * We define the following type traits:
 * - `is_field`: Types that represent elements from a field.
 * - `is_finite`: Types that represent only a finite domain.
 * - `is_float`: Types that represent real numbers using a floating point representation.
 * - `is_integer`: Types that represent the set of integral numbers.
 * - `is_subset_of_integers`: Types that may represent some integral numbers.
 * - `is_number`: Types that represent numbers.
 * - `is_rational`: Types that may represent any rational number.
 * - `is_subset_of_rationals`: Types that may represent some rational numbers.
 *
 * A more exact definition for each of these type traits can be found in their own documentation.
 *
 * Additionally, we define related types in a type traits like manner:
 * - `IntegralType`: Integral type, that the given type is based on. For fractions, this would be the type of the numerator and denominator.
 * - `UnderlyingNumberType`: Number type that is used within a more complex type. For polynomials, this would be the number type of the coefficients.
 *
 * Note that we keep away from similar type traits defined in the standard @cite C++Standard (20.9) (like `std::is_integral` or `std::is_floating_point`, as they are not meant to be specialized for custom types.
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include "../util/platform.h"
#include "config.h"
#include <limits>
#include <type_traits>


namespace carl {
	
template<typename T, typename U =
	typename std::remove_cv<
		typename std::remove_pointer<
			typename std::remove_reference<
				typename std::remove_extent<
					T
				>::type
			>::type
		>::type
	>::type
> struct remove_all : remove_all<U> {};
template<typename T> struct remove_all<T, T> { using type = T; };

/**
 * This template is designed to provide types that are related to other types.
 * It works very much like std::integral_constant, except that it provides a type instead of a constant.
 * We use it as an extension to type traits, meaning that types may have traits that are boolean or other types.
 *
 * The class can be used as follows.
 * Assume that you have a class `A` with an associated type `B`.
 * @code
 * template<T> struct Associated {};
 * template<> struct Associated<A>: has_subtype<B> {};
 * @endcode
 * Now you can obtain the associated type with `Associated<A>::type`.
 *
 * @ingroup typetraits
 */
template<typename T>
struct has_subtype {
	/// A type associated with the type
	using type = T;
};

}

#define TRAIT_TRUE(name,type,groups) \
/** States that type has the trait name. @ingroup typetraits_ ## name groups */ \
template<> struct name<type>: std::true_type {};

#define TRAIT_FALSE(name,type,groups) \
/** States that type does not have the trait name. @ingroup typetraits_ ## name groups */ \
template<> struct name<type>: std::false_type {};

#define TRAIT_TYPE(name,_type,value,groups) \
/** States that name of _type is value. @ingroup typetraits_ ## name groups */ \
template<> struct name<_type>: carl::has_subtype<value> {};

namespace carl {

//
// Forward declarations
//
template<typename IntegerT>
class GFNumber;

template<typename C>
class UnivariatePolynomial;

template<typename C, typename O, typename P>
class MultivariatePolynomial;

template<typename T> struct is_rational;
template<typename T> struct is_subset_of_rationals;

/**
 * @addtogroup typetraits_is_field
 * All types that represent a field are marked with `is_field`.
 *
 * To be a field, the type must satisfy the common axioms for fields (and their technical interpretation):
 * - It represents some (not empty) set of numbers.
 * - It defines the basic operators \f$+,-,\cdot,/\f$, implemented as `operator+()`, `operator-()`, `operator*()`, `operator/()`. The result of these operators is of the same type, i.e. the type is closed under the given operations.
 * - It's operations are *associative* and *commutative*. Multiplication and addition are *distributive*.
 * - There are *identity elements* for addition and multiplication.
 * - For every element of the type, there are *inverse elements* for addition and multiplication.
 *
 * All types that are marked with `is_rational` represent a field.
 */

/**
 * States if a type is a field.
 * Default is true for rationals, false otherwise.
 * @ingroup typetraits_is_field
 * @see UnivariatePolynomial - CauchyBound for example.
 */
template<typename T>
struct is_field: std::integral_constant<bool, is_rational<T>::value> {};
/**
 * States that a Gallois field is a field.
 * @ingroup typetraits_is_field
 */
template<typename C>
struct is_field<GFNumber<C>>: std::true_type {};


/**
 * @addtogroup typetraits_is_finite is_finite
 * All types that can represent only numbers from a finite domain are marked with `is_finite`.
 *
 * All fundamental types are also finite.
 */
/**
 * States if a type represents only a finite domain.
 * Default is true for fundamental types, false otherwise.
 * @ingroup typetraits_is_finite
 */
template<typename T>
struct is_finite: std::integral_constant<bool, std::is_fundamental<T>::value> {};

/**
 * Type trait is_finite_domain.
 * Default is false.
 * @ingroup typetraits_is_finite
 */
template<typename C>
struct is_finite<GFNumber<C>>: std::false_type {};

/**
 * @addtogroup typetraits_is_float is_float
 * All types that represent floating point numbers are marked with `is_float`.
 *
 * A floating point type is used to approximate real number and in general behaves like a field.
 * However, it does not guarantee exact computation and may be subject to rounding errors or overflows.
 */
/**
 * States if a type is a floating point type.
 *
 * Default is true if `std::is_floating_point` is true for this type.
 * @ingroup typetraits_is_float
 */
template<typename T>
struct is_float: std::integral_constant<bool, std::is_floating_point<T>::value> {};

/**
 * @addtogroup typetraits_is_integer is_integer
 * All integral types that can (in theory) represent all integers are marked with `is_integer`.
 *
 * To be an integer type, the type must satisfy the following conditions:
 * - It represents exactly all integer numbers.
 * - It defines the basic operators \f$+, -, \cdot\f$ by implementing `operator+()`, `operator-()` and `operator*()` which are closed.
 * - It's operations are *associative* and *commutative*. Multiplication and addition are *distributive*.
 * - There are *identity elements* for addition and multiplication.
 * - For every element of the type, there is an *inverse element* for addition.
 * - Additionally, it defines the following operations:
 *   - `div()`: Performs an integer division, asserting that the remainder is zero.
 *   - `quotient()`: Calculates the quotient of an integer division.
 *   - `remainder()`: Calculates the remainder of an integer division.
 *   - `mod()`: Calculated the modulus of an integer.
 *   - `operator/()` shall be an alias for `quotient()`.
 */
/**
 * States if a type is an integer type.
 *
 * Default is false.
 * @ingroup typetraits_is_integer
 */
template<typename T>
struct is_integer: std::false_type {};

/**
 * @addtogroup typetraits_is_subset_of_integers is_subset_of_integers
 * All integral types are marked with `is_subset_of_integers`.
 *
 * They must satisfy the same conditions as for `is_integer`, except that they may represent only a subset of all integer numbers.
 * If this is the case, `std::numeric_limits` must be specialized.
 * If the limits are exceeded, the type may behave arbitrarily and the type is not obliged to check for this.
 */
/**
 * States if a type represents a subset of all integers.
 * Default is true for integer types, false otherwise.
 * @ingroup typetraits_is_subset_of_integers
 */
template<typename Type>
struct is_subset_of_integers: std::integral_constant<bool, is_integer<Type>::value> {};

/**
 * @addtogroup typetraits_is_number
 * All types that represent any kind of number are marked with `is_number`.
 *
 * All number types are required to implement the following methods:
 * - `abs()`: Returns the absolute value.
 * - `floor()`: Returns the nearest integer below.
 * - `ceil()`: Returns the nearest integer above.
 * - `pow()`: Returns the power.
 */
/**
 * States if a type is a number type.
 * Default is true for rationals, integers and floats, false otherwise.
 * @ingroup typetraits_is_number
 */
template<typename T>
struct is_number {
	/// Default value of this trait.
	static const bool value = is_subset_of_rationals<T>::value || is_subset_of_integers<T>::value || is_float<T>::value;
};
template<typename T>
constexpr bool is_number<T>::value;

/**
 * @ingroup typetraits_is_number
 * @see GFNumber
 */
template<typename C>
struct is_number<GFNumber<C>>: std::true_type {};

/**
 * @addtogroup typetraits_is_rational is_rational
 * All integral types that can (in theory) represent all rationals are marked with `is_rational`.
 *
 * It is assumed that a fractional representation is used.
 * A type that is rational must satisfy all requirements of `is_field`.
 * Additionally, it must implement the following methods:
 * - `getNum()`: Returns the numerator of a fraction.
 * - `getDenom()`: Return the denominator of a fraction.
 * - `rationalize()`: Converts a native floating point number to the rational type.
 */
/**
 * States if a type is a rational type.
 *
 * We consider a type to be rational, if it can (in theory) represent any rational number.
 * Default is false.
 */
template<typename T>
struct is_rational: std::false_type {};

/**
* States whether a given type is an `Interval`.
* By default, a type is not.
*/
template <class Number>
struct is_interval : std::false_type {};

/**
 * @addtogroup typetraits_is_subset_of_rationals is_subset_of_rationals
 * All rational types that can represent a subset of all rationals are marked with `is_subset_of_rationals`.
 *
 * It is assumed that a fractional representation is used and the restriction to a subset of all rationals is due to the type of the numerator and the denominator.
 */
/**
 * States if a type represents a subset of all rationals and the representation is similar to a rational.
 * Default is true for rationals, false otherwise.
 * @ingroup typetraits_is_subset_of_rationals
 */
template<typename T>
struct is_subset_of_rationals {
	/// Default value of this trait.
	static constexpr bool value = is_rational<T>::value;
};
template<typename T>
constexpr bool is_subset_of_rationals<T>::value;

template<typename T>
struct is_polynomial: std::false_type {};
template<typename T>
struct is_polynomial<carl::UnivariatePolynomial<T>>: std::true_type {};
template<typename T, typename O, typename P>
struct is_polynomial<carl::MultivariatePolynomial<T, O, P>>: std::true_type {};

/**
 * Type trait for the characteristic of the given field (template argument).
 * @see UnivariatePolynomial - squareFreeFactorization for example.
 */
template<typename type>
struct characteristic: std::integral_constant<uint, 0> {};


/**
 * @addtogroup typetraits_IntegralType
 * The associated integral type of any type can be defined with IntegralType.
 *
 * Any function that operates on the type and naturally returns an integer, regardless whether the input was actually integral, uses the associated integral type as result type.
 * Simple examples for this are getNum() and getDenom() which return the numerator and denominator respectively of a fraction.
 */
/**
 * Gives the corresponding integral type.
 * Default is int.
 * @ingroup typetraits_IntegralType
 */
template<typename RationalType>
struct IntegralType {
	/// @todo Should *any* type have an integral type?
	using type = sint;
};

template<typename C>
struct IntegralType<GFNumber<C>> {
	using type = C;
};

template<typename C>
using IntegralTypeIfDifferent = typename std::enable_if<!std::is_same<C, typename IntegralType<C>::type>::value, typename IntegralType<C>::type>::type;

/**
 * @addtogroup typetraits_UnderlyingNumberType
 * The number type that some type is built upon can be defined with UnderlyingNumberType.
 *
 * Any function that operates on the (more complex) type and returns a number can use this trait.
 * The function can thereby easily retrieve the exact number type that is used within the complex type.
 */
/**
 * Gives the underlying number type of a complex object.
 * Default is the type itself.
 * @ingroup typetraits_UnderlyingNumberType
 */
template<typename T>
struct UnderlyingNumberType: has_subtype<T> {};

/**
 * States that UnderlyingNumberType of UnivariatePolynomial<T> is UnderlyingNumberType<C>::type.
 * @ingroup typetraits_UnderlyingNumberType
 */
template<typename C>
struct UnderlyingNumberType<UnivariatePolynomial<C>>: has_subtype<typename UnderlyingNumberType<C>::type> {};

/**
 * States that UnderlyingNumberType of MultivariatePolynomial<C,O,P> is UnderlyingNumberType<C>::type.
 * @ingroup typetraits_UnderlyingNumberType
 */
template<typename C, typename O, typename P>
struct UnderlyingNumberType<MultivariatePolynomial<C, O, P>>: has_subtype<typename UnderlyingNumberType<C>::type> {};
}

namespace carl
{
template<typename T> struct needs_cache : std::false_type {};

template<typename T> struct is_factorized : std::true_type {};

template<typename T>
class PreventConversion
{
    private:
        T mContent;
    public:
        explicit PreventConversion( const T& _other ) : mContent( _other ) {}
//        template<typename O>
//        PreventConversion( const O& _other ) = delete;
        operator const T&() const { return mContent; }
};

template<typename T, typename T2>
bool fitsWithin(const T2& t) {
	return std::numeric_limits<T>::min() <= t && t <= std::numeric_limits<T>::max();
}

}
