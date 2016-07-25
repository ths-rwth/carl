#pragma once

#include <boost/variant.hpp>

#include <type_traits>

namespace carl {

template<typename T>
struct variant_is_type: boost::static_visitor<bool> {
	template<typename TT>
	typename std::enable_if<std::is_same<T,TT>::value, bool>::type operator()(const TT&) const {
		return true;
	}
	template<typename TT>
	typename std::enable_if<!std::is_same<T,TT>::value, bool>::type operator()(const TT&) const {
		return false;
	}
	template<typename V>
	static bool check(const V& variant) {
		return boost::apply_visitor(variant_is_type(), variant);
	}
};

}
