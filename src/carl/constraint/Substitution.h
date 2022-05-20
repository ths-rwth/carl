#pragma once

#include "BasicConstraint.h"

#include <optional>

namespace carl {

/**
 * If this constraint represents a substitution (equation, where at least one variable occurs only linearly),
 * this method detects a (there could be various possibilities) corresponding substitution variable and term.
 * @param _substitutionVariable Is set to the substitution variable, if this constraint represents a substitution.
 * @param _substitutionTerm Is set to the substitution term, if this constraint represents a substitution.
 * @return true, if this constraints represents a substitution;
 *         false, otherwise.
 */
template<typename Pol>
std::optional<std::pair<Variable, Pol>> get_substitution(const BasicConstraint<Pol>& c, bool _negated = false, Variable _exclude = carl::Variable::NO_VARIABLE, std::optional<VariablesInformation<true, Pol>> var_info = std::nullopt) {
	if (var_info == std::nullopt) {
		var_info = c.lhs().template getVarInfo<true>();
	}
	if ((!_negated && c.relation() != Relation::EQ) || (_negated && c.relation() != Relation::NEQ))
		return std::nullopt;
	for (const auto& e : *var_info) {
		if (e.first == _exclude) continue;
		if (e.second.maxDegree() == 1) {
			auto d = e.second.coeffs().find(1);
			assert(d != e.second.coeffs().end());
			if (d->second.is_constant() && (e.first.type() != carl::VariableType::VT_INT || carl::is_one(carl::abs(d->second.constant_part())))) {
				return std::make_pair(e.first, (Pol(e.first) * d->second - c.lhs()) / d->second.constant_part());
			}
		}
	}
	return std::nullopt;
}

template<typename Pol>
std::optional<std::pair<Variable, typename Pol::NumberType>> get_assignment(const BasicConstraint<Pol>& c) {
	if (c.relation() != Relation::EQ) return std::nullopt;
	if (c.lhs().nr_terms() > 2) return std::nullopt;
	if (c.lhs().nr_terms() == 0) return std::nullopt;
	if (!c.lhs().lterm().is_single_variable()) return std::nullopt;
	if (c.lhs().nr_terms() == 1) {
		return std::make_pair(c.lhs().lterm().single_variable(),0);
	}
	assert(c.lhs().nr_terms() == 2);
	if (!c.lhs().trailingTerm().is_constant()) return std::nullopt;
	return std::make_pair(c.lhs().lterm().single_variable(), -c.lhs().trailingTerm().coeff() / c.lhs().lterm().coeff());
}

}