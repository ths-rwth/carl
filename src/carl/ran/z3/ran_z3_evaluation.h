#pragma once

#include <carl/config.h>

#ifdef RAN_USE_Z3

#include "ran_z3.h"

namespace carl {

template<typename Number>
std::optional<real_algebraic_number_z3<Number>> evaluate(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, real_algebraic_number_z3<Number>>& evalMap);

template<typename Number, typename Poly>
std::optional<bool> evaluate(const Constraint<Poly>& constraint, const std::map<Variable, real_algebraic_number_z3<Number>>& evalMap);

// TODO workaround, implement for z3
namespace ran::interval {
template<typename Coeff, typename Number>
bool vanishes(
		const UnivariatePolynomial<Coeff>& poly,
		const std::map<Variable, real_algebraic_number_z3<Number>>& varToRANMap
) {
    assert(false && "Not implemented");
	return false;
}
}

}

#endif