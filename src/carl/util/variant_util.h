#pragma once

#include <boost/variant.hpp>

#include <type_traits>

namespace carl {

namespace detail {
template<typename T>
struct variant_is_type_visitor : boost::static_visitor<bool> {
	template<typename TT>
	constexpr bool operator()(const TT& /*unused*/) const noexcept {
		return std::is_same<T, TT>::value;
	}
};
} // namespace detail
/**
 * Checks whether a variant contains a value of a fiven type
 */
template<typename T, typename Variant>
bool variant_is_type(const Variant& variant) noexcept {
	return boost::apply_visitor(detail::variant_is_type_visitor<T>(), variant);
}

namespace detail {
template<typename Target>
struct variant_extend_visitor : boost::static_visitor<Target> {
	template<typename T>
	Target operator()(const T& t) const {
		return Target(t);
	}
};
} // namespace detail
template<typename Target, typename... Args>
Target variant_extend(const boost::variant<Args...>& variant) {
	return boost::apply_visitor(detail::variant_extend_visitor<Target>(), variant);
}

namespace detail {
struct variant_hash : boost::static_visitor<std::size_t> {
	template<class T>
	std::size_t operator()(const T& val) const {
		return std::hash<T>()(val);
	}
};
} // namespace detail

template<typename... T>
inline std::size_t variant_hash(const boost::variant<T...>& value) {
	return boost::apply_visitor(detail::variant_hash(), value);
}

namespace detail {
	template<bool allowConversion, typename T, typename... Args>
	struct is_from_variant {
		static constexpr bool value = false;
	};
	template<bool allowConversion, typename T, typename First, typename... Tail>
	struct is_from_variant<allowConversion, T, First, Tail...> {
		static constexpr bool first_value = std::conditional<allowConversion, std::is_convertible<T,First>, std::is_same<T,First>>::type::value;
		static constexpr bool value = first_value || is_from_variant<allowConversion, T, Tail...>::value;
	};
	template<bool allowConversion, typename T, typename Variant> struct is_from_variant_wrapper;
	template<bool allowConversion, typename T, template<typename...> class Variant, typename... Args>
	struct is_from_variant_wrapper<allowConversion, T, Variant<Args...>> {
		static constexpr bool value = is_from_variant<allowConversion, T, Args...>::value;
	};
}

template<typename T, typename Variant>
struct is_from_variant {
	static constexpr bool value = detail::is_from_variant_wrapper<false, T, Variant>::value;
};
template<typename T, typename Variant>
struct convertible_to_variant {
	static constexpr bool value = detail::is_from_variant_wrapper<true, T, Variant>::value;
};

} // namespace carl
