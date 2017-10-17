/**
 * @file tuple_util.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <tuple>

namespace carl {

	
namespace detail {
	/**
	 * Helper method for carl::tuple_apply that actually performs the call.
	 */
	template<typename Tuple1, typename Tuple2, std::size_t... I1, std::size_t... I2>
	auto tuple_cat_impl(Tuple1&& t1, Tuple2&& t2, std::index_sequence<I1...> /*unused*/, std::index_sequence<I2...> /*unused*/) {
		return std::make_tuple(std::get<I1>(std::forward<Tuple1>(t1))..., std::get<I2>(std::forward<Tuple2>(t2))...);
	}
}
template<typename Tuple1, typename Tuple2>
auto tuple_cat(Tuple1&& t1, Tuple2&& t2) {
	return detail::tuple_cat_impl(
		std::forward<Tuple1>(t1),
		std::forward<Tuple2>(t2),
		std::make_index_sequence<std::tuple_size<typename std::decay<Tuple1>::type>::value>{},
		std::make_index_sequence<std::tuple_size<typename std::decay<Tuple2>::type>::value>{}
	);
}

namespace detail {
	/**
	 * Helper method for carl::tuple_tail that actually performs the call.
	 */
	template<typename Tuple, std::size_t... I>
	auto tuple_tail_impl(Tuple&& t, std::index_sequence<I...> /*unused*/) {
		return std::make_tuple(std::get<I+1>(std::forward<Tuple>(t))...);
	}
}

/**
 * Returns a new tuple containing everything but the first element.
 */
template<typename Tuple>
auto tuple_tail(Tuple&& t) {
	return detail::tuple_tail_impl(std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value-1>{});
}

template<typename Converter, typename Information, typename FOut, typename... TOut>
struct tuple_convert {
private:
	Information i;
	tuple_convert<Converter, Information, TOut...> conv;
public:
	explicit tuple_convert(const Information& i): i(i), conv(i) {}
	template<typename Tuple>
	std::tuple<FOut, TOut...> operator()(const Tuple& in) {
		return std::tuple_cat(std::make_tuple(Converter::template convert<FOut>(std::get<0>(in), i)), conv(tuple_tail(in)));
	}
};

template<typename Converter, typename Information, typename Out>
struct tuple_convert<Converter, Information, Out> {
private:
	Information i;
public:
	explicit tuple_convert(const Information& i): i(i) {}
	template<typename In>
	std::tuple<Out> operator()(const std::tuple<In>& in) {
		return std::make_tuple(Converter::template convert<Out>(std::get<0>(in), i));
	}
};

namespace detail {
	/**
	 * Helper method for carl::tuple_apply that actually performs the call.
	 */
	template<typename F, typename Tuple, std::size_t... I>
	auto tuple_apply_impl(F&& f, Tuple&& t, std::index_sequence<I...> /*unused*/) {
		return (std::forward<F>(f))(std::get<I>(std::forward<Tuple>(t))...);
	}
}

/**
 * Invokes a callable object f on a tuple of arguments.
 * This is basically std::apply (available with C++17).
 */
template<typename F, typename Tuple>
auto tuple_apply(F&& f, Tuple&& t) {
	return detail::tuple_apply_impl(std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{});
}


namespace detail {
	/**
	 * Helper method for carl::tuple_foreach that actually does the work.
	 */
	template<typename F, typename Tuple, std::size_t... I>
	auto tuple_foreach_impl(F&& f, Tuple&& t, std::index_sequence<I...> /*unused*/) {
		return std::make_tuple(f(std::get<I>(std::forward<Tuple>(t)))...);
	}
}

/**
 * Invokes a callable object f on every element of a tuple and returns a tuple containing the results.
 * This basically corresponds to the functional `map(func, list)`.`
 */
template<typename F, typename Tuple>
auto tuple_foreach(F&& f, Tuple&& t) {
	return detail::tuple_foreach_impl(std::forward<F>(f), std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>{});
}

}
