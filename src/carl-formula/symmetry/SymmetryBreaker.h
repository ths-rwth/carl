#pragma once

#include "../formula/Formula.h"

#include <vector>

namespace carl {
namespace formula {
namespace symmetry {

template<typename Poly>
Formula<Poly> createComparison(Variable x, Variable y, Relation rel) {
	assert(x.type() == y.type());
	switch (x.type()) {
		case VariableType::VT_BOOL:
			switch (rel) {
				case Relation::EQ:
					return Formula<Poly>(FormulaType::IFF, Formula<Poly>(x), Formula<Poly>(y));
				case Relation::LEQ:
					return Formula<Poly>(FormulaType::IMPLIES, Formula<Poly>(x), Formula<Poly>(y));
				case Relation::GEQ:
					return Formula<Poly>(FormulaType::IMPLIES, Formula<Poly>(y), Formula<Poly>(x));
				default:
					assert(false);
			}
		case VariableType::VT_INT:
		case VariableType::VT_REAL:
			return Formula<Poly>(Constraint<Poly>(Poly(x) - y, rel));
		default:
			CARL_LOG_INFO("carl.formula.symmetry", "Tried to break symmetry on unsupported variable type " << x.type());
	}
	return Formula<Poly>(FormulaType::TRUE);
}

/**
 * Creates symmetry breaking constraints from the passed symmetries in the spirit of @cite Crawford1996.
 */
template<typename Poly>
Formula<Poly> lexLeaderConstraint(const Symmetry& vars) {
	constexpr bool eliminateTrue = true;
	Formulas<Poly> eq;
	Formulas<Poly> res;
	std::set<std::pair<Variable,Variable>> inEq;
	for (const auto& v: vars) {
		if (v.first == v.second) continue;
		if (eliminateTrue && inEq.find(v) != inEq.end()) continue;
		Formula<Poly> cur = createComparison<Poly>(v.first, v.second, Relation::LEQ);
		res.emplace_back(Formula<Poly>(FormulaType::IMPLIES, Formula<Poly>(FormulaType::AND, eq), cur));
		eq.emplace_back(createComparison<Poly>(v.first, v.second, Relation::EQ));
		if (eliminateTrue) inEq.emplace(v.second, v.first);
	}
	return Formula<Poly>(FormulaType::AND, std::move(res));
}

}
}
}
