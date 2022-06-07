#pragma once

#include "../Monomial.h"

namespace carl {

/**
 * Calculates the separable part of this monomial.
 * For a monomial \f$ \\prod_i x_i^{e_i}\f$ with \f$e_i \neq 0 \f$, this is \f$ \\prod_i x_i^1 \f$.
 * @return Separable part.
 */
inline Monomial::Arg separable_part(const Monomial& m) {
	Monomial::Content newExps;
	for (auto& it: m) {
		newExps.emplace_back(it.first, 1);
	}
	return createMonomial(std::move(newExps), m.nrVariables());
}
		

}