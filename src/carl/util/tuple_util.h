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
struct tail_impl {
	tail_impl<S-1,Tuple,Tail...> tail;
	std::tuple<Tail...> operator()(const Tuple& t) {
		return std::tuple_cat(std::make_tuple(std::get<std::tuple_size<Tuple>::value-S>(t)), tail(t));
	}
};

/**
 * This functor actually implements the functionality of tail.
 * This is the base version for tuples of size one.
 */
template<typename Tuple, typename First, typename... Tail>
struct tail_impl<1, Tuple, First, Tail...> {
	std::tuple<Tail...> operator()(const Tuple& t) {
		return std::make_tuple(std::get<std::tuple_size<Tuple>::value-1>(t));
	}
};

/**
 * Functor that returns the tail of a tuple.
 * The tail of a tuple are all but the first element.
 */
template<typename First, typename... Tail>
struct tail {
private:
	/// Recursive implementation.
	tail_impl<sizeof...(Tail), std::tuple<First, Tail...>, First, Tail...> tail;
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

}