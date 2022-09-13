#pragma once

#include "VariableComparison.h"

#include <carl-arith/poly/umvpoly/functions/Substitution.h>

namespace carl {

template<typename Poly>
void encode_as_constraints_simple(const MultivariateRoot<Poly>& f, Assignment<typename VariableComparison<Poly>::RAN> ass, Variable var, std::vector<BasicConstraint<Poly>>& out) {
    ass.erase(f.var());
    auto roots = real_roots(to_univariate_polynomial(f.poly(), f.var()), ass);
    if (!roots.is_univariate() || roots.roots().size() < f.k()) return;
    Variable previous;
    for (int i = 1; i <= f.k(); i++) {
        Variable tmp_var;
        if (i == f.k()) tmp_var = var;
        else tmp_var = fresh_real_variable();
        out.emplace_back(substitute(f.poly(), f.var(), Poly(tmp_var)), Relation::EQ);
        if (previous != Variable::NO_VARIABLE) {
            out.emplace_back(Poly(previous) - tmp_var, Relation::LESS);
        }
        previous = tmp_var;
    }
}

template<typename Poly>
void encode_as_constraints_thom(const MultivariateRoot<Poly>& f, Assignment<typename VariableComparison<Poly>::RAN> ass, Variable var, std::vector<BasicConstraint<Poly>>& out) {
    ass.erase(f.var());
    auto roots = real_roots(to_univariate_polynomial(f.poly(), f.var()), ass);
    if (!roots.is_univariate() || roots.roots().size() < f.k()) return;
    auto poly = substitute(f.poly(), f.var(), Poly(var));
    ass.emplace(var, roots.roots()[f.k()-1]);
    while (!poly.is_constant()) {
        auto sign = sgn(evaluate(poly, ass));
        Relation rel = Relation::EQ;
        if (sign == Sign::POSITIVE) rel = Relation::GREATER;
        else if (sign == Sign::NEGATIVE) rel = Relation::LESS;
        out.emplace_back(poly, rel);
        poly = derivative(poly, var);
    }
}

template<typename Poly>
using EncodingCache = std::map<MultivariateRoot<Poly>, std::pair<std::vector<BasicConstraint<Poly>>, Variable>>;

template<typename Poly>
std::pair<std::vector<BasicConstraint<Poly>>, Variable> encode_as_constraints(const MultivariateRoot<Poly>& f, Assignment<typename VariableComparison<Poly>::RAN> ass, EncodingCache<Poly> cache) {
    if (cache.find(f) == cache.end()) {
        Variable var = fresh_real_variable();
        std::vector<BasicConstraint<Poly>> result;
        encode_as_constraints_simple(f, ass, var, result);
        encode_as_constraints_thom(f, ass, var, result);
        if (!result.empty()) {
            cache.emplace(f, std::make_pair(result, var));
        } else {
            cache.emplace(f, std::make_pair(result, Variable()));
        }
    }
    return cache.at(f);
}

template<typename Poly>
std::pair<std::vector<BasicConstraint<Poly>>, BasicConstraint<Poly>> encode_as_constraints(const VariableComparison<Poly>& f, const Assignment<typename VariableComparison<Poly>::RAN>& ass, EncodingCache<Poly> cache) {
    auto c = as_constraint(f);
    if (c) return std::make_pair(std::vector<BasicConstraint<Poly>>(), *c);
    assert(std::holds_alternative<typename VariableComparison<Poly>::MR>(f.value()));
    auto mv = std::get<typename VariableComparison<Poly>::MR>(f.value());
    auto [result, var] = encode_as_constraints(mv, ass, cache);
    if (result.empty()) return std::make_pair(std::vector<BasicConstraint<Poly>>(), BasicConstraint<Poly>(true));
    BasicConstraint<Poly> res_constr(Poly(f.var()) - var, f.negated() ? inverse(f.relation()) : f.relation());
    return std::make_pair(result, res_constr);
}

}