#pragma once

#include "../Formula.h"

namespace carl {

/**
 * Transforms this formula to its quantifier free equivalent.
 * The quantifiers are represented by the parameter variables. Each entry in variables contains all variables between two quantifier alternations.
 * The even entries (starting with 0) are quantified existentially, the odd entries are quantified universally.
 * @param variables Contains the quantified variables.
 * @param level Used for internal recursion.
 * @param negated Used for internal recursion.
 * @return The quantifier-free version of this formula.
 */
template<typename Poly>
Formula<Poly> toQF(QuantifiedVariables& variables, unsigned level = 0, bool negated = false) {
	switch (getType()) {
		case FormulaType::AND:
		case FormulaType::IFF:
		case FormulaType::OR:
		case FormulaType::XOR:
		{
			if (!negated) {
				Formulas<Pol> subs;
				for (auto& sub: subformulas()) {
					subs.push_back(sub.toQF(variables, level, false));
				}
				return Formula<Pol>( getType(), std::move(subs) );
			} else if (getType() == FormulaType::AND || getType() == FormulaType::OR) {
				Formulas<Pol> subs;
				for (auto& sub: subformulas()) {
					subs.push_back(sub.toQF(variables, level, true));
				}
				if (getType() == FormulaType::AND) return Formula<Pol>(FormulaType::OR, std::move(subs));
				else return Formula<Pol>(FormulaType::AND, std::move(subs));
			} else if (getType() == FormulaType::IFF) {
				Formulas<Pol> sub1;
				Formulas<Pol> sub2;
				for (auto& sub: subformulas()) {
					sub1.push_back(sub.toQF(variables, level, true));
					sub2.push_back(sub.toQF(variables, level, false));
				}
				return Formula<Pol>(FormulaType::AND, {Formula<Pol>(FormulaType::OR, std::move(sub1)), Formula<Pol>(FormulaType::OR, std::move(sub2))});
			} else if (getType() == FormulaType::XOR) {
				auto lhs = back().toQF(variables, level, false);
				auto rhs = connectPrecedingSubformulas().toQF(variables, level, true);
				return Formula<Pol>(FormulaType::IFF, {lhs, rhs});
			}
			assert(false);
		}
		case FormulaType::BOOL:
		case FormulaType::CONSTRAINT:
		case FormulaType::FALSE:
		case FormulaType::UEQ:
		case FormulaType::BITVECTOR:
		case FormulaType::TRUE:
		case FormulaType::VARCOMPARE:
		case FormulaType::VARASSIGN:
		{
			if (negated) return Formula<Pol>( NOT, *this );
			else return *this;
		}
		case FormulaType::EXISTS:
		case FormulaType::FORALL:
		{
			unsigned cur = 0;
			if ((level % 2 == (getType() == FormulaType::EXISTS ? (unsigned)0 : (unsigned)1)) ^ negated) cur = level;
			else cur = level+1;
			Variables vars(quantifiedVariables().begin(), quantifiedVariables().end());
			Formula<Pol> f = quantifiedFormula();
			for (auto it = vars.begin(); it != vars.end();) {
				if (it->type() == VariableType::VT_BOOL) {
					// Just leave boolean variables at the base level up to the SAT solver.
					if (cur > 0) {
						f = Formula<Pol>(
							(getType() == FormulaType::EXISTS ? FormulaType::OR : FormulaType::AND),
							{f.substitute({{*it, Formula<Pol>( FormulaType::TRUE )}}),
							f.substitute({{*it, Formula<Pol>( FormulaType::FALSE )}})}
						);
					}
					it = vars.erase(it);
				}
				else it++;
			}
			if (vars.size() > 0) {
				while (variables.size() <= cur) variables.emplace_back();
				variables[cur].insert(vars.begin(), vars.end());
			}
			return f.toQF(variables, cur, negated);
		}
		case FormulaType::IMPLIES:
			if (negated) return Formula<Pol>(FormulaType::AND, {premise().toQF(variables, level, false), conclusion().toQF(variables, level, true)});
			else return Formula<Pol>( FormulaType::IMPLIES, {premise().toQF(variables, level, false), conclusion().toQF(variables, level, false)});
		case FormulaType::ITE:
			return Formula<Pol>( FormulaType::ITE, {condition().toQF(variables, level, negated), firstCase().toQF(variables, level, negated), secondCase().toQF(variables, level, negated)});
		case FormulaType::NOT:
			return subformula().toQF(variables, level, !negated);
	}
	return Formula<Pol>( FormulaType::TRUE );
}

}