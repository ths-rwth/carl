#pragma once

#include "GCD_Monomial.h"

#include "../Term.h"

namespace carl {

/**
 * Calculates the gcd of (t1, t2).
 * If t1 or t2 is zero, undefined.
 * @param t1 first term
 * @param t2 second term
 * @return gcd of t1 and t2.
 */
template<typename Coeff>
Term<Coeff> gcd(const Term<Coeff>& t1, const Term<Coeff>& t2) {
	static_assert(is_field_type<Coeff>::value, "Not yet defined for other coefficients");
	assert(!carl::is_zero(t1));
	assert(!carl::is_zero(t2));
	if (t1.is_constant() || t2.is_constant()) {
		return Term<Coeff>(Coeff(carl::gcd(t1.coeff(), t2.coeff())));
	}
	return Term<Coeff>(
		Coeff(carl::gcd(t1.coeff(), t2.coeff())),
		carl::gcd(t1.monomial(), t2.monomial())
	);
}
}

}