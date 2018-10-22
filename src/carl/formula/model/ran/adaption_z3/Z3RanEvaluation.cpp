#include "Z3RanEvaluation.h"
#include "Z3RanContent.h"

#include "../../../../converter/Z3Converter.h"

namespace carl {

    template<typename Number>
    RealAlgebraicNumber<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, RealAlgebraicNumber<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3(polynomial);
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan());
        for(auto const &pair : evalMap) {
            polynomial::var var = z3().toZ3(pair.first);
            algebraic_numbers::anum& val = pair.second.getZ3Ran().content();
            map.set(var, val);
        }
        z3().polyMan().eval(poly.get(), map, res);
        return RealAlgebraicNumber<Number>(Z3Ran<Number>(res));
    }

    template RealAlgebraicNumber<mpq_class> evaluateZ3(const MultivariatePolynomial<mpq_class>& polynomial, const std::map<Variable, RealAlgebraicNumber<mpq_class>>& evalMap);

}