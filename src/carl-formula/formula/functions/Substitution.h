#pragma once

#include "../Formula.h"
#include "Visit.h"

namespace carl {

namespace helper {
	template<typename Pol>
	struct Substitutor {
		const std::map<Formula<Pol>,Formula<Pol>>& replacements;
		explicit Substitutor(const std::map<Formula<Pol>,Formula<Pol>>& repl): replacements(repl) {}
		Formula<Pol> operator()(const Formula<Pol>& formula) {
			auto it = replacements.find(formula);
			if (it == replacements.end()) return formula;
			return it->second;
		}
	};

	template<typename Pol>
	struct PolynomialSubstitutor {
		const std::map<Variable,typename Formula<Pol>::PolynomialType>& replacements;
		explicit PolynomialSubstitutor(const std::map<Variable,typename Formula<Pol>::PolynomialType>& repl): replacements(repl) {}
		Formula<Pol> operator()(const Formula<Pol>& formula) {
			if (formula.type() != FormulaType::CONSTRAINT) return formula;
			return Formula<Pol>(carl::substitute(formula.constraint().lhs(), replacements), formula.constraint().relation());
		}
	};

	template<typename Pol>
	struct BitvectorSubstitutor {
		const std::map<BVVariable,BVTerm>& replacements;
		explicit BitvectorSubstitutor(const std::map<BVVariable,BVTerm>& repl): replacements(repl) {}
		Formula<Pol> operator()(const Formula<Pol>& formula) {
			if (formula.type() != FormulaType::BITVECTOR) return formula;
			BVTerm lhs = formula.bv_constraint().lhs().substitute(replacements);
			BVTerm rhs = formula.bv_constraint().rhs().substitute(replacements);
			return Formula<Pol>(BVConstraint::create(formula.bv_constraint().relation(), lhs, rhs));
		}
	};

	template<typename Pol>
	struct UninterpretedSubstitutor {
		const std::map<UVariable,UFInstance>& replacements;
		explicit UninterpretedSubstitutor(const std::map<UVariable,UFInstance>& repl): replacements(repl) {}
		Formula<Pol> operator()(const Formula<Pol>& formula) {
			if (formula.type() != FormulaType::UEQ) return formula;

		}
	};
}

template<typename Pol, typename Source, typename Target>
Formula<Pol> substitute(const Formula<Pol>& formula, const Source& source, const Target& target) {
	std::map<Source,Target> tmp;
	tmp.emplace(source, target);
	return substitute(formula, tmp);
}
template<typename Pol>
Formula<Pol> substitute(const Formula<Pol>& formula, const std::map<Formula<Pol>,Formula<Pol>>& replacements) {
	helper::Substitutor<Pol> subs(replacements);
	return visit_result(formula, std::function<Formula<Pol>(Formula<Pol>)>(subs));
}
template<typename Pol>
Formula<Pol> substitute(const Formula<Pol>& formula, const std::map<Variable,typename Formula<Pol>::PolynomialType>& replacements) {
	helper::PolynomialSubstitutor<Pol> subs(replacements);
	return visit_result(formula, std::function<Formula<Pol>(Formula<Pol>)>(subs));
}
template<typename Pol>
Formula<Pol> substitute(const Formula<Pol>& formula, const std::map<BVVariable,BVTerm>& replacements) {
	helper::BitvectorSubstitutor<Pol> subs(replacements);
	return visit_result(formula, std::function<Formula<Pol>(Formula<Pol>)>(subs));
}
template<typename Pol>
Formula<Pol> substitute(const Formula<Pol>& formula, const std::map<UVariable,UFInstance>& replacements) {
	helper::UninterpretedSubstitutor<Pol> subs(replacements);
	return visit_result(formula, std::function<Formula<Pol>(Formula<Pol>)>(subs));
}

}