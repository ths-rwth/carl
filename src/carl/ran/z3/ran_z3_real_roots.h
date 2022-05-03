#pragma once

#include <carl-common/config.h>

#ifdef RAN_USE_Z3

#include "../real_roots_common.h"

#include "ran_z3.h"

namespace carl::ran::z3 {
    template<typename Coeff, typename Number>
    real_roots_result<real_algebraic_number_z3<Number>> real_roots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
    );

    template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
    real_roots_result<real_algebraic_number_z3<Number>> real_roots(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, real_algebraic_number_z3<Number>>& m,
        const Interval<Number>& interval = Interval<Number>::unboundedInterval()
    );
}

#endif