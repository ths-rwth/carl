#include "../../../../converter/Z3Converter.h"

#ifdef USE_Z3_RANS

#include "Z3RanEvaluation.h"
#include "Z3RanContent.h"

namespace carl {

    template<typename Number>
    RealAlgebraicNumber<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, RealAlgebraicNumber<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3(polynomial);
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : evalMap) {
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.getZ3Ran().content();
            map.set(var, val);
        }
        z3().polyMan().eval(poly.get(), map, res);
        return RealAlgebraicNumber<Number>(Z3Ran<Number>(std::move(res)));
    }

    template RealAlgebraicNumber<mpq_class> evaluateZ3(const MultivariatePolynomial<mpq_class>& polynomial, const std::map<Variable, RealAlgebraicNumber<mpq_class>>& evalMap);

}

#endif