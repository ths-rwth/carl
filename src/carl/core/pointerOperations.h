/* 
 * File:   pointerOperations.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file contains generic operations on pointers.
 * Most are specializations of standard routines like std::equal_to or std::hash.
 */

#pragma once

#include <functional>

template<typename T>
struct std::equal_to<T*> {
	bool operator()(const T* lhs, const T* rhs) const {
		if (lhs == nullptr && rhs == nullptr) return true;
		if (lhs == nullptr || rhs == nullptr) return false;
		if (lhs == rhs) return true;
		return std::equal_to<T>()(*lhs, *rhs);
	}
};

namespace carl {

template<typename T>
struct ptr_hash {
	std::size_t operator()(const T* t) const {
		if (t == nullptr) return 0;
		return std::hash<T>()(*t);
	}
};

}