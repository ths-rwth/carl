/** 
 * @file   adaption_z3/operations.h
 * @ingroup z3
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * @warning This file should never be included directly but only via operations.h
 */
#pragma once

#include "../../util/platform.h"
#include <limits.h>

#include "include.h"

namespace carl {

inline bool isZero(const rational& n) {
	return n.is_zero();
}
inline bool isOne(const rational& n) {
	return n.is_one();
}

inline auto getDenom(const rational& n) {
	return denominator(n);
}
inline auto getNum(const rational& n) {
	return numerator(n);
}

inline std::string toString(const rational& _number, bool _infix) {
	return toString(_number.to_mpq(), _infix);
}

}
