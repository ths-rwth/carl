#pragma once

#include "numbers.h"

#include "../core/MultivariatePolynomial.h"

#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>

namespace carl {
	
template<typename TypeSelector, typename Types, typename... Args>
auto createFunctionSelector(Args&&... args);

namespace function_selector {
	template<template<typename> class Trait>
	struct NaryTypeSelectorWrapper {
		template<typename T, typename... Others>
		using type = typename Trait<typename remove_all<T>::type>::type;
	};
	
	/**
	 * This class allows for a compile-time switch between different functions for different number types.
	 *
	 * Note that the constructor is private, please use the createFunctionSelector() wrapper.
	 * The basic usage is like this:
	 * 
	 * - Firstly, choose a `TypeSelector` type trait which extracts the type deciding upon the selection from the arguments.
	 *   For most carl classes and the use case we have in mind, relying on carl::UnderlyingNumberType will work. Hence, there is NaryTypeSelector which will use only the first argument to deduce the type.
	 *   Note there is no check that all other arguments (if there are any) would yield the same type!
	 * @code{.cpp}
	 * using TypeSelector = carl::function_selector::NaryTypeSelector;
	 * @endcode
	 * - Secondly, create a list of types that the FunctionSelector should detect:
	 * @code{.cpp}
	 * using types = carl::function_selector::wrap_types<mpz_class,mpq_class>;
	 * @endcode
	 * - Thirdly, call createFunctionSelector with the TypeSelector and these types and pass it a list of (lambda) functions for each of the types specified before:
	 * @code{.cpp}
	 * auto selector = carl::createFunctionSelector<TypeSelector, types>(
	 *     [](const auto& n){ return function_for_mpz_class(n); },
	 *     [](const auto& n){ return function_for_mpq_class(n); }
	 * );
	 * @endcode
	 * - Finally, call this selector object with the arguments you want:
	 * @code{.cpp}
	 * selector(mpq_class(0));
	 * selector(mpq_class(1));
	 * selector(mpq_class(2));
	 * @endcode
	 */
	template<typename TypeSelector, typename TypeVector, typename... Functions>
	class FunctionSelector {
		friend auto createFunctionSelector<TypeSelector, TypeVector, Functions...>(Functions&&...);
		static_assert(sizeof...(Functions) == boost::mpl::size<TypeVector>::value, "Functions and TypeVector must have the same size.");
	private:
		std::tuple<Functions...> mFunctions;
	public:
		explicit FunctionSelector(Functions&&... f): mFunctions(std::forward<Functions>(f)...) {}
		template<typename... Args>
		auto operator()(Args&&... args) const {
			using T = typename TypeSelector::template type<Args...>;
			using it = typename boost::mpl::find<TypeVector,T>::type;
			static_assert(!std::is_same<it, typename boost::mpl::end<TypeVector>::type>::value, "Obtained type T was not found in TypeVector.");
			return std::get<it::pos::value>(mFunctions)(std::forward<Args>(args)...);
		}
	};
	
	template<typename... Args>
	using wrap_types = boost::mpl::vector<Args...>;
	
	using NaryTypeSelector = NaryTypeSelectorWrapper<carl::UnderlyingNumberType>;
}

template<typename TypeSelector, typename Types, typename... Args>
auto createFunctionSelector(Args&&... args) {
	return function_selector::FunctionSelector<TypeSelector, Types, Args...>(std::forward<Args>(args)...);
}

}
