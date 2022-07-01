#pragma once

#include "BasicConstraint.h"

namespace carl {

/**
 * @return true, if this constraint is a bound.
 */
template<typename Pol>
bool is_bound(const BasicConstraint<Pol>& constr) {
    auto vars = variables(constr);
    if (vars.size() != 1 || constr.lhs().degree(*vars.begin()) != 1) return false;
    return constr.relation() != Relation::NEQ;
}

/**
 * @return true, if this constraint is a lower bound.
 */
template<typename Pol>
bool is_lower_bound(const BasicConstraint<Pol>& constr) {
    if (is_bound(constr)) {
        if (constr.relation() == Relation::EQ) return true;
        const typename Pol::NumberType& coeff = constr.lhs().lterm().coeff();
        if (coeff < 0)
            return (constr.relation() == Relation::LEQ || constr.relation() == Relation::LESS);
        else {
            assert(coeff > 0);
            return (constr.relation() == Relation::GEQ || constr.relation() == Relation::GREATER);
        }
    }
    return false;
}

/**
 * @return true, if this constraint is an upper bound.
 */
template<typename Pol>
bool is_upper_bound(const BasicConstraint<Pol>& constr) {
    if (is_bound(constr)) {
        if (constr.relation() == Relation::EQ) return true;
        const typename Pol::NumberType& coeff = constr.lhs().lterm().coeff();
        if (coeff > 0)
            return (constr.relation() == Relation::LEQ || constr.relation() == Relation::LESS);
        else {
            assert(coeff < 0);
            return (constr.relation() == Relation::GEQ || constr.relation() == Relation::GREATER);
        }
    }
    return false;
}

}