#pragma once

#include "Z3Ran.h"

#include "../RealAlgebraicNumber.h"


namespace carl {
    template<typename Coeff, typename Number>
    std::vector<RealAlgebraicNumber<Number>> realRootsZ3(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
    ) {
        auto poly = z3().toZ3(polynomial);

        anum_vector roots;
        z3().anumMan().isolate_roots(poly, roots);

        std::vector<RealAlgebraicNumber<Number>> res;
        for (size_t i = 0; i < roots.size(); i++) {
            Z3Ran<Number> z3ran(roots[i]);
            if (z3ran.containedIn(interval)) {
                res.push_back(RealAlgebraicNumber<Number>(std::move(z3ran))); 
            }
        }
        
        return res;
    }

    template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
    boost::optional<std::vector<RealAlgebraicNumber<Number>>> realRootsZ3(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, RealAlgebraicNumber<Number>>& m,
        const Interval<Number>& interval = Interval<Number>::unboundedInterval()
    ) {
        auto poly = z3().toZ3(p);

        nlsat::assignment map(z3().anumMan());
        for(auto const &pair : m) {
            polynomial::var var = z3().toZ3(pair.first);
            algebraic_numbers::anum val = pair.second.getZ3Ran().content();
            map.set(var, val);
        }

        anum_vector roots;
        z3().anumMan().isolate_roots(poly, map, roots);

        std::vector<RealAlgebraicNumber<Number>> res;
        for (size_t i = 0; i < roots.size(); i++) {
            Z3Ran<Number> z3ran(roots[i]);
            if (z3ran.containedIn(interval)) {
                res.push_back(RealAlgebraicNumber<Number>(std::move(z3ran))); 
            }
        }
        
        return res;
    }
}