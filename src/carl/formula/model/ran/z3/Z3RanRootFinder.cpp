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
    std::vector<RealAlgebraicNumber<Number>> realRootsZ3(
        const UnivariatePolynomial<Coeff>& p,
        const std::map<Variable, RealAlgebraicNumber<Number>>& m,
        const Interval<Number>& interval
    ) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(p);

		nlsat::assignment map(z3().anumMan());
        for(auto const &pair : m) {
			polynomial::var var = z3().toZ3(pair.first);
            if (pair.second.isZ3Ran()) {
                const algebraic_numbers::anum& val = pair.second.getZ3Ran().content();
                map.set(var, val);
			} else if (pair.second.isInterval()) {
				polynomial::polynomial_ref poly = z3().toZ3IntCoeff(pair.second.getIRPolynomial());
				mpq lower = z3().toZ3MPQ(pair.second.lower());
				mpq upper = z3().toZ3MPQ(pair.second.upper());
				algebraic_numbers::anum root;
				for (unsigned i = 1; i <= pair.second.getIRPolynomial().degree(); ++i) {
					z3().anumMan().mk_root(poly, i, root);
					if (z3().anumMan().gt(root, lower) && z3().anumMan().lt(root, upper)) {
						break;
					}
				}
				assert(z3().anumMan().gt(root, lower) && z3().anumMan().lt(root, upper));
				map.set(var, root);
            } else {
                assert(pair.second.isNumeric());
                mpq num = z3().toZ3MPQ(pair.second.value());
                anum alnum;
                z3().anumMan().set(alnum, num);
                map.set(var, alnum);
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
    template std::vector<RealAlgebraicNumber<mpq_class>> realRootsZ3(
        const UnivariatePolynomial<mpq_class>& p,
        const std::map<Variable, RealAlgebraicNumber<mpq_class>>& m,
        const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );
    */

    template std::vector<RealAlgebraicNumber<mpq_class>> realRootsZ3(
        const UnivariatePolynomial<MultivariatePolynomial<mpq_class>>& p,
        const std::map<Variable, RealAlgebraicNumber<mpq_class>>& m,
        const Interval<mpq_class>& interval = Interval<mpq_class>::unboundedInterval()
    );
}

#endif