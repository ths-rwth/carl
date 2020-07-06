#include "ran_z3_real_roots.h"

#ifdef RAN_USE_Z3

#include "../../../../converter/Z3Converter.h"

#include "Z3RanContent.h"

namespace carl::ran::z3 {
    template<typename Coeff, typename Number>
    std::vector<real_algebraic_number_z3<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval
    ) {
        polynomial::polynomial_ref poly = carl::z3().toZ3IntCoeff(polynomial);

		anum_vector roots;
        carl::z3().anumMan().isolate_roots(poly, roots);

        std::vector<real_algebraic_number_z3<Number>> res;
        for (unsigned i = 0; i < roots.size(); i++) {
            Z3Ran<Number> zr(std::move(roots[i]));
            if (zr.containedIn(interval)) {
                res.push_back(real_algebraic_number_z3<Number>(zr)); // std::move(zr)));
            }
        }

        /* values have been moved...
        for (unsigned i = 0; i < roots.size(); i++) {
            z3().free(roots[i]);
        }*/
        
        return res;
    }

    template<typename Coeff, typename Number>
    std::vector<real_algebraic_number_z3<Number>> realRoots(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, real_algebraic_number_z3<Number>>& m,
        const Interval<Number>& interval
    ) {
        polynomial::polynomial_ref poly = carl::z3().toZ3IntCoeff(p);

		nlsat::assignment map(carl::z3().anumMan());
        for(auto const &pair : m) {
			polynomial::var var = carl::z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.z3_ran().content();
            map.set(var, val);
        }

        anum_vector roots;
        carl::z3().anumMan().isolate_roots(poly, map, roots);

        std::vector<real_algebraic_number_z3<Number>> res;
        for (unsigned i = 0; i < roots.size(); i++) {
            Z3Ran<Number> zr(std::move(roots[i]));
            if (zr.containedIn(interval)) {
                res.push_back(real_algebraic_number_z3<Number>(zr)); // std::move(zr)));
            }
        }

        /* values have been moved...
        for (unsigned i = 0; i < roots.size(); i++) {
            z3().free(roots[i]);
        }*/
        
        return res;
    }


    template std::vector<real_algebraic_number_z3<mpq_class>> realRoots(
		const UnivariatePolynomial<mpq_class>& polynomial,
		const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );

    /*
    template std::vector<real_algebraic_number_z3<mpq_class>> realRoots(
        const UnivariatePolynomial<mpq_class>& p,
        const std::map<Variable, real_algebraic_number_z3<mpq_class>>& m,
        const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );
    */

    template std::vector<real_algebraic_number_z3<mpq_class>> realRoots(
        const UnivariatePolynomial<MultivariatePolynomial<mpq_class>>& p,
        const std::map<Variable, real_algebraic_number_z3<mpq_class>>& m,
        const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );
}

#endif