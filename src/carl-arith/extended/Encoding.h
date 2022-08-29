#pragma once

#include "VariableComparison.h"

#include <carl-arith/poly/umvpoly/functions/Substitution.h>

namespace carl {

template<typename Poly>
std::pair<std::vector<BasicConstraint<Poly>>, Variable> encode_as_constraints(const MultivariateRoot<Poly>& f, Assignment<typename VariableComparison<Poly>::RAN> ass) {
    ass.erase(f.var());
    auto roots = real_roots(to_univariate_polynomial(f.poly(), f.var()), ass);
    if (!roots.is_univariate() || roots.roots().size() < f.k()) return std::make_pair(std::vector<BasicConstraint<Poly>>(), Variable());
    Variable res_var;
    std::vector<BasicConstraint<Poly>> result;
    Variable previous;
    for (int i = 1; i <= roots.roots().size(); i++) {
        Variable var = fresh_real_variable();
        result.emplace_back(substitute(f.poly(), f.var(), Poly(var)), Relation::EQ);
        if (previous != Variable::NO_VARIABLE) {
            result.emplace_back(Poly(previous) - var, Relation::LESS);
        }
        previous = var;
        if (i == f.k()) res_var = var;
    }    
    return std::make_pair(result, res_var);
}

template<typename Poly>
std::pair<std::vector<BasicConstraint<Poly>>, BasicConstraint<Poly>> encode_as_constraints(const VariableComparison<Poly>& f, const Assignment<typename VariableComparison<Poly>::RAN>& ass) {
    auto c = as_constraint(f);
    if (c) return std::make_pair(std::vector<BasicConstraint<Poly>>(), *c);
    assert(std::holds_alternative<typename VariableComparison<Poly>::MR>(f.value()));
    auto mv = std::get<typename VariableComparison<Poly>::MR>(f.value());
    auto [result, var] = encode_as_constraints(mv, ass);
    if (result.empty()) return std::make_pair(std::vector<BasicConstraint<Poly>>(), BasicConstraint<Poly>(true));
    BasicConstraint<Poly> res_constr(Poly(f.var()) - var, f.negated() ? inverse(f.relation()) : f.relation());
    return std::make_pair(result, res_constr);
}

}