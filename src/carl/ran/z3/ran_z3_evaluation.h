#pragma once

#include <carl/config.h>

#ifdef RAN_USE_Z3

#include "ran_z3.h"

namespace carl {

template<typename Number>
std::optional<real_algebraic_number_z3<Number>> evaluate(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, real_algebraic_number_z3<Number>>& evalMap);

template<typename Number, typename Poly>
boost::tribool evaluate(const BasicConstraint<Poly>& constraint, const std::map<Variable, real_algebraic_number_z3<Number>>& evalMap);

}

#endif