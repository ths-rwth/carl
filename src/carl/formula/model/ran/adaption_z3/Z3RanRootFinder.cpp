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
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(polynomial);

		CARL_LOG_WARN("carl.z3ran", "Roots of " << poly);

        anum_vector roots;
        z3().anumMan().isolate_roots(poly, roots);

        std::vector<RealAlgebraicNumber<Number>> res;
        for (unsigned i = 0; i < roots.size(); i++) {
            Z3Ran<Number> zr(std::move(roots[i]));
            if (zr.containedIn(interval)) {
                res.push_back(RealAlgebraicNumber<Number>(zr)); // std::move(zr)));
            }
        }

        /* values have been moved...
        for (unsigned i = 0; i < roots.size(); i++) {
            z3().free(roots[i]);
        }*/
        
        return res;
    }

    template<typename Coeff, typename Number>
    boost::optional<std::vector<RealAlgebraicNumber<Number>>> realRootsZ3(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, RealAlgebraicNumber<Number>>& m,
        const Interval<Number>& interval
    ) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(p);

		CARL_LOG_WARN("carl.z3ran", "Roots of " << poly);

        nlsat::assignment map(z3().anumMan());
        for(auto const &pair : m) {
			CARL_LOG_WARN("carl.z3ran", pair.first << " -> " << pair.second);
            polynomial::var var = z3().toZ3(pair.first);
            if (pair.second.isZ3Ran()) {
                const algebraic_numbers::anum& val = pair.second.getZ3Ran().content();
                map.set(var, val);
				{
					std::stringstream ss;
					z3().anumMan().display_root(ss, val);
					CARL_LOG_WARN("carl.z3ran", var << " -> " << ss.str());
				}
            } else {
                assert(pair.second.isNumeric());
                mpq num = z3().toZ3MPQ(pair.second.value());
                anum alnum;
                z3().anumMan().set(alnum, num);
                map.set(var, alnum);
				{
					std::stringstream ss;
					z3().anumMan().display_root(ss, alnum);
					CARL_LOG_WARN("carl.z3ran", var << " -> " << ss.str());
				}
                z3().free(num);
                z3().free(alnum);
            }
        }

        anum_vector roots;
        z3().anumMan().isolate_roots(poly, map, roots);

        std::vector<RealAlgebraicNumber<Number>> res;
        for (unsigned i = 0; i < roots.size(); i++) {
            Z3Ran<Number> zr(std::move(roots[i]));
            if (zr.containedIn(interval)) {
                res.push_back(RealAlgebraicNumber<Number>(zr)); // std::move(zr)));
            }
        }

        /* values have been moved...
        for (unsigned i = 0; i < roots.size(); i++) {
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