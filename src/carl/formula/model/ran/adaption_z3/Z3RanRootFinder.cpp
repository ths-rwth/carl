#include "../../../../converter/Z3Converter.h"

#ifdef USE_Z3_RANS

#include "Z3RanRootFinder.h"
#include "Z3RanContent.h"

namespace carl {
    template<typename Coeff, typename Number>
    std::vector<RealAlgebraicNumber<Number>> realRootsZ3(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval
    ) {
        polynomial::polynomial_ref poly = z3().toZ3(polynomial);

        anum_vector roots;
        z3().anumMan().isolate_roots(poly, roots);

        std::vector<RealAlgebraicNumber<Number>> res;
        for (size_t i = 0; i < roots.size(); i++) {
            Z3Ran<Number> zr(std::move(roots[i]));
            if (zr.containedIn(interval)) {
                res.push_back(RealAlgebraicNumber<Number>(zr)); // TODO move?
            }
        }

        /* values have been moved...
        for (size_t i = 0; i < roots.size(); i++) {
            z3().free(roots[i]);
        }*/
        
        return res;
    }

    template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
    boost::optional<std::vector<RealAlgebraicNumber<Number>>> realRootsZ3(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, RealAlgebraicNumber<Number>>& m,
        const Interval<Number>& interval
    ) {
        polynomial::polynomial_ref poly = z3().toZ3(p);

        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : m) {
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.getZ3Ran().content();
            map.set(var, val);
        }

        anum_vector roots;
        z3().anumMan().isolate_roots(poly, map, roots);

        std::vector<RealAlgebraicNumber<Number>> res;
        for (size_t i = 0; i < roots.size(); i++) {
            Z3Ran<Number> zr(std::move(roots[i]));
            if (zr.containedIn(interval)) {
                res.push_back(RealAlgebraicNumber<Number>(zr)); // TODO move?
            }
        }

        /* values have been moved...
        for (size_t i = 0; i < roots.size(); i++) {
            z3().free(roots[i]);
        }*/
        
        return res;
    }


    template std::vector<RealAlgebraicNumber<mpq_class>> realRootsZ3(
		const UnivariatePolynomial<mpq_class>& polynomial,
		const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );

    /*
    template boost::optional<std::vector<RealAlgebraicNumber<mpq_class>>> realRootsZ3(
        const UnivariatePolynomial<mpq_class>& p,
        const std::map<Variable, RealAlgebraicNumber<mpq_class>>& m,
        const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );
    */

    template boost::optional<std::vector<RealAlgebraicNumber<mpq_class>>> realRootsZ3(
        const UnivariatePolynomial<MultivariatePolynomial<mpq_class>>& p,
        const std::map<Variable, RealAlgebraicNumber<mpq_class>>& m,
        const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );
}

#endif