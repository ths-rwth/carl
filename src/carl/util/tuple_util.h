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
class tuple_convert {
private:
	Information mInfo;
	tuple_convert<Converter, Information, TOut...> conv;
public:
	explicit tuple_convert(const Information& i): mInfo(i), conv(i) {}
	template<typename Tuple>
	std::tuple<FOut, TOut...> operator()(const Tuple& in) {
		return std::tuple_cat(std::make_tuple(Converter::template convert<FOut>(std::get<0>(in), mInfo)), conv(tuple_tail(in)));
	}
};

template<typename Converter, typename Information, typename Out>
class tuple_convert<Converter, Information, Out> {
private:
	Information mInfo;
public:
	explicit tuple_convert(const Information& i): mInfo(i) {}
	template<typename In>
	std::tuple<Out> operator()(const std::tuple<In>& in) {
		return std::make_tuple(Converter::template convert<Out>(std::get<0>(in), mInfo));
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
 * This is basically `std::apply` (available with C++17).
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

namespace detail {
	/**
	 * Helper functor for carl::tuple_accumulate that actually does the work.
	 */
	template<typename Tuple, typename T, typename F>
	struct tuple_accumulate_impl {
		template<std::size_t I, typename std::enable_if<0 < I, void>::type* = nullptr>
		T call(Tuple&& t, T&& init, F&& f) {
			return std::forward<F>(f)(call<I-1>(t, init, f), std::get<I-1>(t));
		}
		template<std::size_t I, typename std::enable_if<0 == I, void>::type* = nullptr>
		T call(Tuple&& t, T&& init, F&& f) {
			return init;
		}
	};
}

/**
 * Implements a functional fold (similar to `std::accumulate`) for `std::tuple`.
 * Combines all tuple elements using a combinator function `f` and an initial value `init`.
 */
template<typename Tuple, typename T, typename F>
T tuple_accumulate(Tuple&& t, T&& init, F&& f) {
	return detail::tuple_accumulate_impl<Tuple,T,F>().call<std::tuple_size<Tuple>::value>(t, init, f);
}

}
