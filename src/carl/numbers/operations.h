/* 
 * File:   operations.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

/**
 * This file contains a common interface for basic number functions.
 * All methods here are meant as an interface that should be implemented for each specific number type, if applicable.
 */

#pragma once

#include "typetraits.h"
#include "../util/SFINAE.h"

namespace carl {

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */

template<typename Number>
inline typename IntegralT<Number>::type getNum(const Number&);

template<typename Number>
inline typename IntegralT<Number>::type getDenom(const Number&);

template<typename Number>
inline bool isInteger(const Number&);

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */
template<typename Number>
inline double toDouble(const Number&);

template<typename Number, typename Integer>
inline Integer toInt(const Number&);

template<typename Number>
inline Number rationalize(double);

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */
template<typename Number>
inline typename IntegralT<Number>::type floor(const Number&);

template<typename Number>
inline typename IntegralT<Number>::type ceil(const Number&);

template<typename Number, EnableIf<is_integer<Number>> = dummy>
inline Number gcd(const Number&, const Number&);

template<typename Number, EnableIf<is_integer<Number>> = dummy>
inline Number lcm(const Number&, const Number&);

template<typename Number>
inline Number pow(const Number&, unsigned);

template<typename Number, EnableIf<is_integer<Number>> = dummy>
inline Number mod(const Number&, const Number&);

template<typename Number, EnableIf<is_integer<Number>> = dummy>
inline Number div(const Number&, const Number&);

}

#include "operations_cln.h"