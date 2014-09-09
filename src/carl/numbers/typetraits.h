/** 
 * @file   numbers/typetraits.h
 * @ingroup typetraits
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 */

#pragma once

#include "../util/platform.h"
#include "config.h"
#include <type_traits>

#include "../util/SFINAE.h"

#define TRAIT_TRUE(name,type) \
/** States that type has the trait name. @ingroup typetraits_ ## name */ \
template<> struct name<type> { \
	/** Value is true. */ \
	static constexpr bool value = true; \
}

#define TRAIT_FALSE(name,type) \
/** States that type does not have the trait name. @ingroup typetraits */ \
template<> struct name<type> { \
	/** Value is false. */ \
	static constexpr bool value = false; \
}

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

/**
 * States if a type is a rational type.
 *
 * We consider a type to be rational, if it can (in theory) represent any rational number.
 * Default is false.
 */
template<typename type>
struct is_rational
{
	/// Default value of this trait.
	static constexpr bool value = false;
};

/**
 * States if a type is an integer type.
 *
 * We consider a type an integer type, if it can (in theory) represent all integers and no other values.
 * Default is false.
 * @ingroup typetraits
 */
template<typename T>
struct is_integer {
	/// Default value of this trait.
	static constexpr bool value = false;
};


/**
 * States if a type represents a subset of all integers.
 * Default is true for integer types, false otherwise.
 * @ingroup typetraits
 */
template<typename Type>
struct is_subset_of_integers {
	/// Default value of this trait.
	static constexpr bool value = is_integer<Type>::value;
};

/**
 * States if a type represents a subset of all rationals and the representation is similar to a rational.
 * Default is true for rationals and subsets of integers, false otherwise.
 * @ingroup typetraits
 */
template<typename Type>
struct is_subset_of_rationals {
	/// Default value of this trait.
	static constexpr bool value = is_rational<Type>::value || is_subset_of_integers<Type>::value;
};

/**
 * States if a type is a field.
 * Default is true for rationals, false otherwise.
 * @ingroup typetraits
 * @see UnivariatePolynomial - CauchyBound for example.
 */
template<typename T>
struct is_field {
	/// Default value of this trait.
	static constexpr bool value = is_rational<T>::value;
};
/**
 * States that a Gallois field is a field.
 * @ingroup typetraits
 */
template<typename C>
struct is_field<GFNumber<C>> {
	/// Value of this trait.
	static constexpr bool value = true;
};




/**
 * Type trait for the characteristic of the given field (template argument).
 * @see UnivariatePolynomial - squareFreeFactorization for example.
 */
template<typename type>
struct characteristic {
	static constexpr unsigned value = 0;
};


/**
 * States if a type represents only a finite domain.
 * Default is false for rationals and integers, true otherwise.
 * @ingroup typetraits
 */
template<typename C>
struct is_finite
{
	static constexpr bool value = !(is_rational<C>::value || is_integer<C>::value);
};

/**
 * Type trait is_finite_domain.
 * Default is false.
 * @ingroup typetraits
 */
template<typename C>
struct is_finite<GFNumber<C>> {
	/// Default value of this trait.
	static constexpr bool value = true;
};

/**
 * States if a type is a float type.
 * Default is false.
 * @ingroup typetraits
 */
template<typename type>
struct is_float {
	/// Default value of this trait.
    static constexpr bool value = false;
};

/**
 * States if a type is a primitive number type.
 * Default is false.
 * @ingroup typetraits_is_primitive
 */
template<typename type>
struct is_primitive {
	/// Default value of this trait.
	static constexpr bool value = false;
};

/**
 * States if a type is a number type.
 * Default is true for rationals, integers and floats, false otherwise.
 * @ingroup typetraits
 */
template<typename T>
struct is_number {
	/// Default value of this trait.
	static constexpr bool value = is_rational<T>::value || is_integer<T>::value || is_float<T>::value;
};

/**
 * @ingroup typetraits
 * @see GFNumber
 */
template<typename C>
struct is_number<GFNumber<C>> {
	/// Default value of this trait.
	static constexpr bool value = true;
};
/**
 * Gives the corresponding integral type.
 * Default is int.
 */
template<typename RationalType>
struct IntegralType {
	typedef int type;
};

template<typename C>
struct IntegralType<GFNumber<C>> {
	typedef C type;
};

/**
 * Gives the underlying number type of a complex object.
 * Default is the type itself.
 */
template<typename C>
struct UnderlyingNumberType
{
	typedef C type;
};

/**
 * Gives the underlying number type of univariate polynomials.
 * This is the underlying number type of the polynomials coefficients.
 */
template<typename C>
struct UnderlyingNumberType<UnivariatePolynomial<C>>
{
	typedef typename UnderlyingNumberType<C>::type type;
};

/**
 * Gives the underlying number type of multivariate polynomials.
 * This is the underlying number type of the polynomials coefficients.
 */
template<typename C, typename O, typename P>
struct UnderlyingNumberType<MultivariatePolynomial<C, O, P>>
{
	typedef typename UnderlyingNumberType<C>::type type;
};
}
