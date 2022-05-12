#pragma once

#include "../Monomial.h"

namespace carl {

/**
 * Calculates the least common multiple of two monomial pointers.
 * If both are valid objects, the gcd of both is calculated.
 * If only one is a valid object, this one is returned.
 * If both are invalid objects, an empty monomial is returned.
 * @param lhs First monomial.
 * @param rhs Second monomial.
 * @return gcd of lhs and rhs.
 */
Monomial::Arg gcd(const Monomial::Arg& lhs, const Monomial::Arg& rhs);

}