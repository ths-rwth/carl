/** 
 * @file:   SFINAE.h
 * @author: Sebastian Junges
 *
 * @since September 10, 2013
 * 
 */

#pragma once
#include <type_traits>


namespace carl
{

// Logical conjunction metafunction
//template <typename... T>
//struct All : std::true_type 
//{ };
//template <typename Head, typename... Tail>
//struct All<Head, Tail...> : std::conditional<Head, All<Tail...>, std::false_type> { };

	
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

has_method_struct(normalize);

//http://stackoverflow.com/questions/11251376/how-can-i-check-if-a-type-is-an-instantiation-of-a-given-class-template
template < template <typename...> class Template, typename T >
struct is_instantiation_of : std::false_type {};
template < template <typename...> class Template, typename... Args >
struct is_instantiation_of< Template, Template<Args...> > : std::true_type {};

}