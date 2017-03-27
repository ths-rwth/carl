#pragma once

#include <boost/variant.hpp>

#include <type_traits>

namespace carl {

namespace detail {
	template<typename T>
	struct variant_is_type_visitor: public boost::static_visitor<bool> {
		template<typename TT>
		constexpr bool operator()(const TT& /*unused*/) const noexcept {
			return std::is_same<T,TT>::value;
		}
	};
}
/**
 * Checks whether a variant contains a value of a fiven type
 */
template<typename T, typename Variant>
bool variant_is_type(const Variant& variant) noexcept {
	return boost::apply_visitor(detail::variant_is_type_visitor<T>(), variant);
}

namespace detail {
	template<typename Target>
	struct variant_extend_visitor: boost::static_visitor<Target> {
		template<typename T>
		Target operator()(const T& t) const {
			return Target(t);
		}
	};
}
template<typename Target, typename ...Args>
Target variant_extend(const boost::variant<Args...>& variant) {
	return boost::apply_visitor(detail::variant_extend_visitor<Target>(), variant);
}

}
