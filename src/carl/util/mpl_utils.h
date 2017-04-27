#pragma once
/**
 * @file mpl_utils.h
 * This file provides two helpers to mangle with type sequences using boost::mpl.
 * `mpl_unique` makes a `mpl::vector<>` almost unique. (If two types have the same size, this may fail)
 * `mpl_concatenate` concatenates an arbitrary number of `mpl::vector<>` types.
 * `mpl_variant_of` constructs a variant from a `mpl::vector<>` making the types unique.
 */

#include <type_traits>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/vector.hpp>

namespace carl {

template<typename T>
struct mpl_unique {
	typedef boost::mpl::less<boost::mpl::sizeof_<boost::mpl::_>, boost::mpl::sizeof_<boost::mpl::_>> Less;
	typedef std::is_same<boost::mpl::_, boost::mpl::_> Equal;
	typedef typename boost::mpl::sort<T, Less>::type Sorted;
	typedef typename boost::mpl::unique<Sorted, Equal>::type Unique;
	typedef Unique type;
};

template<std::size_t S, typename Front, typename... Tail>
struct mpl_concatenate_impl {
	typedef typename mpl_concatenate_impl<S-1, Tail...>::type TailConcatenation;
	typedef typename boost::mpl::copy<Front, boost::mpl::back_inserter<TailConcatenation>>::type type;
};
template<typename Front, typename... Tail>
struct mpl_concatenate_impl<1, Front, Tail...> {
	typedef Front type;
};
template<typename... T>
struct mpl_concatenate {
	typedef typename mpl_concatenate_impl<sizeof...(T), T...>::type type;
};

template<bool, typename Vector, typename... Unpacked>
struct mpl_variant_of_impl {
	typedef typename boost::mpl::front<Vector>::type Front;
	typedef typename boost::mpl::pop_front<Vector>::type Tail;
	typedef typename mpl_variant_of_impl<boost::mpl::empty<Tail>::value, Tail, Front, Unpacked...>::type type;
};
template<typename Vector, typename... Unpacked>
struct mpl_variant_of_impl<true, Vector, Unpacked...> {
	typedef boost::variant<Unpacked...> type;
};
template<typename Vector>
struct mpl_variant_of {
	typedef typename mpl_unique<Vector>::type Unique;
	typedef typename mpl_variant_of_impl<boost::mpl::empty<Unique>::value, Unique>::type type;
};

}
