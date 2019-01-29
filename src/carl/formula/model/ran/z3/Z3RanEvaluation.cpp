#include "Z3RanEvaluation.h"

#ifdef RAN_USE_Z3

#include "../../../../converter/Z3Converter.h"

#include "Z3RanContent.h"
#include "../RealAlgebraicNumber_Z3.h"

namespace carl {
/*
    template<typename Number>
    RealAlgebraicNumber<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, RealAlgebraicNumber<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(polynomial);
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : evalMap) {
            assert(pair.second.isZ3Ran()); // all trivial assignments should have already been plugged in ...
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.getZ3Ran().content();
            map.set(var, val);
        }
        z3().polyMan().eval(poly.get(), map, res);
        return RealAlgebraicNumber<Number>(Z3Ran<Number>(std::move(res)));
    }

    template RealAlgebraicNumber<mpq_class> evaluateZ3(const MultivariatePolynomial<mpq_class>& polynomial, const std::map<Variable, RealAlgebraicNumber<mpq_class>>& evalMap);
*/

    template<typename Number>
    ran::Z3Content<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, ran::Z3Content<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(polynomial);
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : evalMap) {
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.z3_ran().content();
            map.set(var, val);
        }
        z3().polyMan().eval(poly.get(), map, res);
        return ran::Z3Content<Number>(Z3Ran<Number>(std::move(res)));
    }

    template ran::Z3Content<mpq_class> evaluateZ3(const MultivariatePolynomial<mpq_class>& polynomial, const std::map<Variable, ran::Z3Content<mpq_class>>& evalMap);

    template<typename Number, typename Poly>
    bool evaluateZ3(const Constraint<Poly>& constraint, const std::map<Variable, ran::Z3Content<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(constraint.lhs());
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : evalMap) {
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.z3_ran().content();
            map.set(var, val);
        }
        int rs = z3().anumMan().eval_sign_at(poly, map);
        return evaluate(rs, constraint.relation());
    }

    template bool evaluateZ3(const Constraint<MultivariatePolynomial<mpq_class>>& constraint, const std::map<Variable, ran::Z3Content<mpq_class>>& evalMap);

}

#endif