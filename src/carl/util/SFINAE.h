/** 
 * @file   SFINAE.h
 * @author Sebastian Junges
 *
 * @since September 10, 2013
 * 
 */

#pragma once
#include <type_traits>


namespace carl
{

// from flamingdangerzone.com/cxx11/2012/05/29/type-traits-galore.html
template <bool If, typename Then, typename Else>
using Conditional = typename std::conditional<If, Then, Else>::type;
template <bool B, typename...>
struct dependent_bool_type : std::integral_constant<bool, B> {};
// and an alias, just for kicks :)
template <bool B, typename... T>
using Bool = typename dependent_bool_type<B, T...>::type;	
	
/// Meta-logical negation
template <typename T>
using Not = Bool<!T::value>;

/// Meta-logical disjunction
template <typename... T>
struct any : Bool<false> {};
template <typename Head, typename... Tail>
struct any<Head, Tail...> : Conditional<Head::value, Bool<true>, any<Tail...>> {};

/// Meta-logical conjunction
template <typename... T>
struct all : Bool<true> {};
template <typename Head, typename... Tail>
struct all<Head, Tail...> : Conditional<Head::value, all<Tail...>, Bool<false>> {};

// http://flamingdangerzone.com/cxx11/2012/06/01/almost-static-if.html	
namespace dtl
{
	enum class enabled {};
}

// Support for Clang 3.1.
constexpr dtl::enabled dummy = {};

template <typename Condition>
using EnableIf = typename std::enable_if<Condition::value, dtl::enabled>::type;	
template <typename Condition>
using DisableIf = typename std::enable_if<!Condition::value, dtl::enabled>::type;	

template<typename> struct Void { typedef void type; };

#define has_method_struct(methodname) \
template<typename T, typename SFINAE = void> \
struct has_##methodname : std::false_type {}; \
template<typename T> \
struct has_##methodname<T, typename Void<decltype( std::declval<T&>().methodname() )>::type> : std::true_type {}; 

has_method_struct(normalize)

//http://stackoverflow.com/questions/11251376/how-can-i-check-if-a-type-is-an-instantiation-of-a-given-class-template
template < template <typename...> class Template, typename T >
struct is_instantiation_of : std::false_type { static constexpr bool value = false; };
template < template <typename...> class Template, typename... Args >
struct is_instantiation_of< Template, Template<Args...> > : std::true_type { static constexpr bool value = true; };

}