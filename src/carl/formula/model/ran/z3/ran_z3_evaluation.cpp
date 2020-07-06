#include "ran_z3_evaluation.h"

#ifdef RAN_USE_Z3

#include "../../../../converter/Z3Converter.h"

#include "Z3RanContent.h"

namespace carl {
    template<typename Number>
    real_algebraic_number_z3<Number> evaluate(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, real_algebraic_number_z3<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(polynomial);
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : evalMap) {
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.content();
            map.set(var, val);
        }
        z3().polyMan().eval(poly.get(), map, res);
        return real_algebraic_number_z3<Number>(std::move(res));
    }

    template real_algebraic_number_z3<mpq_class> evaluate(const MultivariatePolynomial<mpq_class>& polynomial, const std::map<Variable, real_algebraic_number_z3<mpq_class>>& evalMap);

    template<typename Number, typename Poly>
    bool evaluate(const Constraint<Poly>& constraint, const std::map<Variable, real_algebraic_number_z3<Number>>& evalMap) {
        polynomial::polynomial_ref poly = z3().toZ3IntCoeff(constraint.lhs());
        algebraic_numbers::anum res;
        nlsat::assignment map(z3().anumMan()); // map frees its elements automatically
        for(auto const &pair : evalMap) {
            polynomial::var var = z3().toZ3(pair.first);
            const algebraic_numbers::anum& val = pair.second.content();
            map.set(var, val);
        }
        int rs = z3().anumMan().eval_sign_at(poly, map);
        return evaluate(rs, constraint.relation());
    }

    template bool evaluate(const Constraint<MultivariatePolynomial<mpq_class>>& constraint, const std::map<Variable, real_algebraic_number_z3<mpq_class>>& evalMap);

}

#endif