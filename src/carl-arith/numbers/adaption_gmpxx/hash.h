/** 
 * @file    adaption_gmpxx/hash.h
 * @ingroup gmpxx
 * @author  Sebastian Junges
 * @author  Florian Corzilius
 *
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include <carl-common/util/hash.h>
#include "include.h"


#include <cstddef>
#include <functional>

namespace std {

template<> 
struct hash<mpz_class> {
	std::size_t operator()(const mpz_class& z) const {
		return z.get_ui();
	}
};

template<>
struct hash<mpq_class> {
	std::size_t operator()(const mpq_class& q) const {
		return carl::hash_all(q.get_num(), q.get_den());
	}
};

}
