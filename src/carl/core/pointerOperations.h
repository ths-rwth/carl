/* 
 * File:   pointerOperations.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file contains generic operations on pointers.
 * Most are specializations of standard routines like std::equal_to or std::hash.
 */

#pragma once

#include <functional>

namespace std {

/**
 * Specialization of std::equal_to for pointer types.
 *
 * We consider two pointers equal, if they point to the same memory location or the objects they point to are equal.
 * Note that the memory location may also be zero.
 */
template<typename T>
struct equal_to<T*> {
	/**
	 * Checks if two pointers are equal.
	 * @param lhs First pointer.
	 * @param rhs Second pointer.
	 * @return If lhs and rhs are equal.
	 */
	bool operator()(const T* lhs, const T* rhs) const {
		if (lhs == nullptr && rhs == nullptr) return true;
		if (lhs == nullptr || rhs == nullptr) return false;
		if (lhs == rhs) return true;
		return std::equal_to<T>()(*lhs, *rhs);
	}
};

}

namespace carl {

/**
 * An alternative specialization of std::hash for pointer types.
 *
 * In case the pointer is not a nullptr, we return the hash of the object it points to.
 */
template<typename T>
struct ptr_hash {
	/**
	 * Calculates the hash of a pointer.
	 * If the pointer is a nullptr, the result is zero.
	 * Otherwise, the result is the hash of the referenced object.
	 * @param t Pointer.
	 * @return Hash of t.
	 */
	std::size_t operator()(const T* t) const {
		if (t == nullptr) return 0;
		return std::hash<T>()(*t);
	}
};

}