#pragma once

namespace carl {


/**
 * Recursively calls func on every subformula.
 * @param formula Formula to visit.
 * @param func Function to call.
 */
template<typename Pol, typename Visitor>
void visit(const Formula<Pol>& formula, /*std::function<void(const Formula<Pol>&)>&*/ Visitor func) {
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
template<typename Pol, typename Visitor>
Formula<Pol> visit_result(const Formula<Pol>& formula, /*std::function<Formula<Pol>(const Formula<Pol>&)>&*/ Visitor func) {
	Formula<Pol> newFormula = formula;
	switch (formula.getType()) {
		case AND:
		case OR:
		case IFF:
		case XOR: {
			Formulas<typename Formula<Pol>::PolynomialType> newSubformulas;
			bool changed = false;
			for (const auto& cur: formula.subformulas()) {
				Formula newCur = visit_result(cur, func);
				if (newCur != cur) changed = true;
				newSubformulas.push_back(newCur);
			}
			if (changed) {
				newFormula = Formula(formula.getType(), std::move(newSubformulas));
			}
			break;
		}
		case NOT: {
			Formula<Pol> cur = visit_result(formula.subformula(), func);
			if (cur != formula.subformula()) {
				newFormula = !cur;
			}
			break;
		}
		case IMPLIES: {
			Formula<Pol> prem = visit_result(formula.premise(), func);
			Formula<Pol> conc = visit_result(formula.conclusion(), func);
			if ((prem != formula.premise()) || (conc != formula.conclusion())) {
				newFormula = Formula(IMPLIES, {prem, conc});
			}
			break;
		}
		case ITE: {
			Formula<Pol> cond = visit_result(formula.condition(), func);
			Formula<Pol> fCase = visit_result(formula.firstCase(), func);
			Formula<Pol> sCase = visit_result(formula.secondCase(), func);
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
			Formula<Pol> sub = visit_result(formula.quantifiedFormula(), func);
			if (sub != formula.quantifiedFormula()) {
				newFormula = Formula<Pol>(formula.getType(), formula.quantifiedVariables(), sub);
			}
			break;
		}
	}
	return func(newFormula);
}


}