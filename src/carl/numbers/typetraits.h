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
 * - `is_integer`: Types that may represent any integral number.
 * - `is_subset_of_integer`: Types that may represent some integral numbers.
 * - `is_number`: Types that represent numbers.
 * - `is_rational`: Types that may represent any rational number.
 * - `is_subset_of_rational`: Types that may represent some rational numbers.
 *
 * We extend the following type traits from the STL:
 * - `std::is_floating_point`: Types that use a floating point representation.
 * - `std::is_fundamental`: Types that fundamental as of @cite C++Standard (3.9.1).
 * - `std::is_integral`: Types that can only represent integers.
 *
 * Additionally, we define related types in a type traits like manner:
 * - `IntegralType`: Integral type, that the given type is based on. For fractions, this would be the type of the numerator and denominator.
 * - `UnderlyingNumberType`: Number type that is used within a more complex type. For polynomials, this would be the number type of the coefficients.
 */

#pragma once

#include "../util/platform.h"
#include "config.h"
#include <type_traits>


namespace carl {

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
	typedef T type;
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
template<typename C>
struct is_finite: std::integral_constant<bool, std::is_fundamental<C>::value> {};

/**
 * Type trait is_finite_domain.
 * Default is false.
 * @ingroup typetraits_is_finite
 */
template<typename C>
struct is_finite<GFNumber<C>>: std::false_type {};

/**
 * @addtogroup typetraits_is_floating_point std::is_floating_point
 * All types that use a floating point representation are marked with `std::is_floating_point`.
 */

/**
 * @addtogroup typetraits_is_fundamental std::is_fundamental
 * All types that are fundamental types as of @cite C++Standard (3.9.1) are marked with `std::is_fundamental`.
 *
 * We regard only primitive types like `bool`, `int` or `double` fundamental, not all number types.
 */

/**
 * @addtogroup typetraits_is_integer is_integer
 * All integral types that can (in theory) represent all integers are marked with `is_integer`.
 *
 * We consider a type integral, if it represents only integers.
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
 * All integral types that can represent only a subset of all integers are marked with `is_subset_of_integers`.
 *
 * We consider a type integral, if it represents only integers.
 */
/**
 * States if a type represents a subset of all integers.
 * Default is true for integer types, false otherwise.
 * @ingroup typetraits_is_subset_of_integers
 */
template<typename Type>
struct is_subset_of_integers: std::integral_constant<bool, is_integer<Type>::value> {};


/**
 * @addtogroup typetraits_is_integral std::is_integral
 * All types that use a integral representation are marked with `std::is_integral`.
 */

/**
 * @addtogroup typetraits_is_number
 * All types that represent any kind of number are marked with `is_number`.
 */
/**
 * States if a type is a number type.
 * Default is true for rationals, integers and floats, false otherwise.
 * @ingroup typetraits_is_number
 */
template<typename T>
struct is_number {
	/// Default value of this trait.
	static constexpr bool value = is_subset_of_rationals<T>::value || is_subset_of_integers<T>::value || std::is_floating_point<T>::value;
};

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
 * This guarantees that function like carl::getDenom() or carl::getNum() work on such types.
 */
/**
 * States if a type is a rational type.
 *
 * We consider a type to be rational, if it can (in theory) represent any rational number.
 * Default is false.
 */
template<typename type>
struct is_rational: std::false_type {};

/**
 * @addtogroup typetraits_is_subset_of_rationals is_subset_of_rationals
 * All rational types that can represent only a subset of all rationals are marked with `is_subset_of_rationals`.
 */
/**
 * States if a type represents a subset of all rationals and the representation is similar to a rational.
 * Default is true for rationals, false otherwise.
 * @ingroup typetraits_is_subset_of_rationals
 */
template<typename Type>
struct is_subset_of_rationals {
	/// Default value of this trait.
	static constexpr bool value = is_rational<Type>::value;
};




/**
 * Type trait for the characteristic of the given field (template argument).
 * @see UnivariatePolynomial - squareFreeFactorization for example.
 */
template<typename type>
struct characteristic: std::integral_constant<unsigned, 0> {};


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
	typedef int type;
};

template<typename C>
struct IntegralType<GFNumber<C>> {
	typedef C type;
};

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
