#pragma once

#include "../Formula.h"

namespace carl {

/**
 * This class provides a generic visitor for the above Formula class.
 */
template<typename Formula>
struct FormulaVisitor {
	/**
	 * Recursively calls func on every subformula.
	 * @param formula Formula to visit.
	 * @param func Function to call.
	 */
	void visit(const Formula& formula, const std::function<void(Formula)>& func) {
		switch (formula.getType()) {
			case AND:
			case OR:
			case IFF:
			case XOR:
			case IMPLIES:
			case ITE:
				for (const auto& cur: formula.subformulas()) visit(cur, func);
				break;
			case NOT:
				visit(formula.subformula(), func);
				break;
			case BOOL:
			case CONSTRAINT:
			case VARCOMPARE:
			case VARASSIGN:
			case BITVECTOR:
			case TRUE:
			case FALSE:
			case UEQ:
				break;
			case EXISTS:
			case FORALL:
				visit(formula.quantifiedFormula(), func);
				break;
		}
		func(formula);
	}
	/**
	 * Recursively calls func on every subformula and return a new formula.
	 * On every call of func, the passed formula is replaced by the result.
	 * @param formula Formula to visit.
	 * @param func Function to call.
	 * @return New formula.
	 */
	Formula visitResult(const Formula& formula, const std::function<Formula(Formula)>& func) {
		Formula newFormula = formula;
		switch (formula.getType()) {
			case AND:
			case OR:
			case IFF:
			case XOR: {
				Formulas<typename Formula::PolynomialType> newSubformulas;
				bool changed = false;
				for (const auto& cur: formula.subformulas()) {
					Formula newCur = visitResult(cur, func);
					if (newCur != cur) changed = true;
					newSubformulas.push_back(newCur);
				}
				if (changed) {
					newFormula = Formula(formula.getType(), std::move(newSubformulas));
				}
				break;
			}
			case NOT: {
				Formula cur = visitResult(formula.subformula(), func);
				if (cur != formula.subformula()) {
					newFormula = !cur;
				}
				break;
			}
			case IMPLIES: {
				Formula prem = visitResult(formula.premise(), func);
				Formula conc = visitResult(formula.conclusion(), func);
				if ((prem != formula.premise()) || (conc != formula.conclusion())) {
					newFormula = Formula(IMPLIES, {prem, conc});
				}
				break;
			}
			case ITE: {
				Formula cond = visitResult(formula.condition(), func);
				Formula fCase = visitResult(formula.firstCase(), func);
				Formula sCase = visitResult(formula.secondCase(), func);
				if ((cond != formula.condition()) || (fCase != formula.firstCase()) || (sCase != formula.secondCase())) {
					newFormula = Formula(ITE, {cond, fCase, sCase});
				}
				break;
			}
			case BOOL:
			case CONSTRAINT:
			case VARCOMPARE:
			case VARASSIGN:
			case BITVECTOR:
			case TRUE:
			case FALSE:
			case UEQ:
				break;
			case EXISTS:
			case FORALL: {
				Formula sub = visitResult(formula.quantifiedFormula(), func);
				if (sub != formula.quantifiedFormula()) {
					newFormula = Formula(formula.getType(), formula.quantifiedVariables(), sub);
				}
				break;
			}
		}
		return func(newFormula);
	}
};

template<typename Formula>
struct FormulaSubstitutor {
private:
	FormulaVisitor<Formula> visitor;

	struct Substitutor {
		const std::map<Formula,Formula>& replacements;
		explicit Substitutor(const std::map<Formula,Formula>& repl): replacements(repl) {}
		Formula operator()(const Formula& formula) {
			auto it = replacements.find(formula);
			if (it == replacements.end()) return formula;
			return it->second;
		}
	};

	struct PolynomialSubstitutor {
		const std::map<Variable,typename Formula::PolynomialType>& replacements;
		explicit PolynomialSubstitutor(const std::map<Variable,typename Formula::PolynomialType>& repl): replacements(repl) {}
		Formula operator()(const Formula& formula) {
			if (formula.getType() != FormulaType::CONSTRAINT) return formula;
			return Formula(carl::substitute(formula.constraint().lhs(), replacements), formula.constraint().relation());
		}
	};

	struct BitvectorSubstitutor {
		const std::map<BVVariable,BVTerm>& replacements;
		explicit BitvectorSubstitutor(const std::map<BVVariable,BVTerm>& repl): replacements(repl) {}
		Formula operator()(const Formula& formula) {
			if (formula.getType() != FormulaType::BITVECTOR) return formula;
			BVTerm lhs = formula.bvConstraint().lhs().substitute(replacements);
			BVTerm rhs = formula.bvConstraint().rhs().substitute(replacements);
			return Formula(BVConstraint::create(formula.bvConstraint().relation(), lhs, rhs));
		}
	};

	struct UninterpretedSubstitutor {
		const std::map<UVariable,UFInstance>& replacements;
		explicit UninterpretedSubstitutor(const std::map<UVariable,UFInstance>& repl): replacements(repl) {}
		Formula operator()(const Formula& formula) {
			if (formula.getType() != FormulaType::UEQ) return formula;

		}
	};
public:
	template<typename Source, typename Target>
	Formula substitute(const Formula& formula, const Source& source, const Target& target) {
		std::map<Source,Target> tmp;
		tmp.emplace(source, target);
		return substitute(formula, tmp);
	}

	Formula substitute(const Formula& formula, const std::map<Formula,Formula>& replacements) {
		Substitutor subs(replacements);
		return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
	}
	Formula substitute(const Formula& formula, const std::map<Variable,typename Formula::PolynomialType>& replacements) {
		PolynomialSubstitutor subs(replacements);
		return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
	}
	Formula substitute(const Formula& formula, const std::map<BVVariable,BVTerm>& replacements) {
		BitvectorSubstitutor subs(replacements);
		return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
	}
	Formula substitute(const Formula& formula, const std::map<UVariable,UFInstance>& replacements) {
		UninterpretedSubstitutor subs(replacements);
		return visitor.visitResult(formula, std::function<Formula(Formula)>(subs));
	}
};


}