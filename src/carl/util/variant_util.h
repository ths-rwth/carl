#pragma once

#include <boost/variant.hpp>

#include <type_traits>

namespace carl {

template<typename T>
struct variant_is_type_visitor: public boost::static_visitor<bool> {
	template<typename TT>
	constexpr bool operator()(const TT& /*unused*/) const noexcept {
		return std::is_same<T,TT>::value;
	}
};
template<typename T, typename Variant>
bool variant_is_type(const Variant& variant) noexcept {
	return boost::apply_visitor(variant_is_type_visitor<T>(), variant);
}

template<typename Target>
struct variant_extend: boost::static_visitor<Target> {
	template<typename T>
	Target operator()(const T& t) const {
		return Target(t);
	}
	template<typename ...Args>
	static Target extend(const boost::variant<Args...>& variant) {
		return boost::apply_visitor(variant_extend(), variant);
	}
};

}
