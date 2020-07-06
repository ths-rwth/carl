#pragma once

#include <carl/config.h>

#ifdef RAN_USE_Z3

#include "ran_z3.h"

namespace carl::ran::z3 {
    template<typename Coeff, typename Number>
    std::vector<real_algebraic_number_z3<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
    );

    template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
    std::vector<real_algebraic_number_z3<Number>> realRoots(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, real_algebraic_number_z3<Number>>& m,
        const Interval<Number>& interval = Interval<Number>::unboundedInterval()
    );
}

#endif