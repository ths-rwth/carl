#pragma once

#include "../../VarInfo.h"
#include "../MultivariatePolynomial.h"

namespace carl {

namespace detail {

template<typename CoeffType>
inline void var_info_term(VarInfo<CoeffType>& info, const typename CoeffType::TermType& term, const Variable var, bool collect_coeff) {
    if (term.has(var)) {
        exponent exp = term.monomial()->exponent_of_variable(var);
        info.increase_num_occurences();
        info.raise_max_degree(exp);
        info.lower_min_degree(exp);
        if (collect_coeff) {
            typename CoeffType::MonomType::Arg m = term.monomial()->drop_variable(var);
            if(m == nullptr) {
                info.update_coeff(exp, typename CoeffType::TermType(term.coeff()));
            } else {
                info.update_coeff(exp, typename CoeffType::TermType(term.coeff(), m));
            }
        }
    } else {
        if (collect_coeff) {
            info.update_coeff(0, term);
        }
    }
}

template<typename Coeff, typename Ordering, typename Policies>
inline void vars_info_term(VarsInfo<MultivariatePolynomial<Coeff,Ordering,Policies>>& infos, const typename MultivariatePolynomial<Coeff,Ordering,Policies>::TermType& term, bool collect_coeff) {
    for (const auto& ve : *term.monomial()) {
        auto& info = infos.var(ve.first);

        info.increase_num_occurences();
        info.raise_max_degree(ve.second);
        info.lower_min_degree(ve.second);
        if (collect_coeff) {
            typename MultivariatePolynomial<Coeff,Ordering,Policies>::MonomType::Arg m = term.monomial()->drop_variable(ve.first);
            if(m == nullptr) {
                info.update_coeff(ve.second, typename MultivariatePolynomial<Coeff,Ordering,Policies>::TermType(term.coeff()));
            } else {
                info.update_coeff(ve.second, typename MultivariatePolynomial<Coeff,Ordering,Policies>::TermType(term.coeff(), m));
            }
        }
    }
}

}

template<typename Coeff, typename Ordering, typename Policies>
inline VarInfo<MultivariatePolynomial<Coeff,Ordering,Policies>> var_info(const MultivariatePolynomial<Coeff,Ordering,Policies>& poly, const Variable var, bool collect_coeff = false) {
    VarInfo<MultivariatePolynomial<Coeff,Ordering,Policies>> res;
    for (const auto& term : poly) {
        detail::var_info_term(res, term, var, collect_coeff);
    }
    return res;
}

template<typename Coeff, typename Ordering, typename Policies>
inline VarsInfo<MultivariatePolynomial<Coeff,Ordering,Policies>> vars_info(const MultivariatePolynomial<Coeff,Ordering,Policies>& poly, bool collect_coeff = false) {
    //VarsInfo<MultivariatePolynomial<Coeff,Ordering,Policies>> res;
    //for (const auto& term : poly) {
    //    detail::vars_info_term(res, term, collect_coeff);
    //}
    //return res;
    VarsInfo<MultivariatePolynomial<Coeff,Ordering,Policies>> res;
    for (const auto& var : variables(poly)) {
        res.data().emplace(var, var_info(poly, var, collect_coeff));
    }
    return res;
}



}