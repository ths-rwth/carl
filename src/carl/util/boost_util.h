#pragma once

#if BOOST_VERSION < 107100

#include "hash.h"

#include <boost/dynamic_bitset.hpp>

namespace std {

/// Hash method for boost::dynamic_bitset. Since boost 1.71 this comes with boost.
template<typename Block, typename Allocator>
struct hash<boost::dynamic_bitset<Block,Allocator>> {
	std::size_t operator()(const boost::dynamic_bitset<Block,Allocator>& bs) const {
		std::size_t seed = bs.size();
		boost::to_block_range(bs, carl::hash_inserter<Block>{ seed });
		return seed;
	}
};

}

#endif