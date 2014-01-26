/** 
 * @file:   operations.h
 * @author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file contains a common interface for basic number functions.
 * All methods here are meant as an interface that should be implemented for each specific number type, if applicable.
 * The exact signatures, e.g. if variables are passed by reference, can be changed if necessary.
 */



#pragma once

#include "typetraits.h"
#include <cmath>
#include "../util/SFINAE.h"

namespace carl {
    
/*
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */
/*
inline const Integer getNum(const Number&);

inline const Integer getDenom(const Number&);

inline bool isInteger(const Number&);
*/

template<typename C>
inline bool isInteger(const GFNumber<C>&) {
	return true;
}

/*
 * Conversion functions
 * 
 * The following function convert types to other types.
 */
/*
inline double toDouble(const Number&);

template<typename Integer> 
inline Integer toInt(const Number&);

inline Number rationalize(double);
*/

/*
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */
/*
inline Number abs(const Number&);

inline Integer floor(const Number&);

inline Integer ceil(const Number&);

inline Number gcd(const Number&, const Number&);

inline Number lcm(const Number&, const Number&);

inline Number pow(const Number&, unsigned);

inline std::pair<Number,Number> sqrt(const Number&);

inline Number mod(const Number&, const Number&);

inline Number div(const Number&, const Number&);

inline void divide(const Number& dividend, const Number& divisor, Number& quotient, Number& remainder);
*/

}

#include "adaption_cln/operations.h"
#include "adaption_gmpxx/operations.h"
#include "operations_native.h"
