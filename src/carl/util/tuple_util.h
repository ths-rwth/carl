/**
 * @file tuple_util.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

namespace carl {

/**
 * This functor actually implements the functionality of tail.
 * This is the recursive version for larger tuples.
 */
template<std::size_t S, typename Tuple, typename First, typename... Tail>
struct tuple_tail_impl {
	tuple_tail_impl<S-1,Tuple,Tail...> tail;
	std::tuple<Tail...> operator()(const Tuple& t) {
		return std::tuple_cat(std::make_tuple(std::get<std::tuple_size<Tuple>::value-S>(t)), tail(t));
	}
};

/**
 * This functor actually implements the functionality of tail.
 * This is the base version for tuples of size one.
 */
template<typename Tuple, typename First, typename... Tail>
struct tuple_tail_impl<1, Tuple, First, Tail...> {
	std::tuple<Tail...> operator()(const Tuple& t) {
		return std::make_tuple(std::get<std::tuple_size<Tuple>::value-1>(t));
	}
};

/**
 * Functor that returns the tail of a tuple.
 * The tail of a tuple are all but the first element.
 */
template<typename First, typename... Tail>
struct tuple_tail {
private:
	/// Recursive implementation.
	tuple_tail_impl<sizeof...(Tail), std::tuple<First, Tail...>, First, Tail...> tail;
public:
	/**
	 * Returns the tail of the given tuple.
     * @param t Tuple.
     * @return Tail of t.
     */
	std::tuple<Tail...> operator()(const std::tuple<First, Tail...>& t) {
		return tail(t);
	}
};



template<typename Converter, typename Information, typename FOut, typename... TOut>
struct tuple_convert {
private:
	Information i;
	tuple_convert<Converter, Information, TOut...> conv;
public:
	tuple_convert(const Information& i): i(i), conv(i) {}
	template<typename First, typename... Tail>
	std::tuple<FOut, TOut...> operator()(const std::tuple<First, Tail...>& in) {
		tuple_tail<First,Tail...> tail;
		return std::tuple_cat(std::make_tuple(Converter::template convert<FOut>(std::get<0>(in), i)), conv(tail(in)));
	}
};

template<typename Converter, typename Information, typename Out>
struct tuple_convert<Converter, Information, Out> {
private:
	Information i;
public:
	tuple_convert(const Information& i): i(i) {}
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
	auto tuple_apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
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

}
