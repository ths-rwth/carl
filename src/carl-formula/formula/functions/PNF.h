#pragma once

#include "../Formula.h"
#include "Substitution.h"
#include "aux.h"

namespace carl {

enum class Quantifier {
	EXISTS,
	FORALL,
	FREE
};
inline std::ostream& operator<<(std::ostream& os, const Quantifier& type) {
	switch (type) {
	case Quantifier::EXISTS:
		os << "exists";
		return os;
	case Quantifier::FORALL:
		os << "forall";
		return os;
	case Quantifier::FREE:
		os << "free";
		return os;
	}
}

using QuantifierPrefix = std::vector<std::pair<Quantifier, carl::Variable>>;

template<typename Poly>
Formula<Poly> to_pnf(const Formula<Poly>& f, QuantifierPrefix& prefix, boost::container::flat_set<Variable>& used_vars, bool negated = false) {
	switch (f.type()) {
		case FormulaType::AND:
		case FormulaType::IFF:
		case FormulaType::OR:
		case FormulaType::XOR: {
			if (!negated) {
				Formulas<Poly> subs;
				for (auto& sub : f.subformulas()) {
					subs.push_back(to_pnf(sub, prefix, used_vars, false));
				}
				return Formula<Poly>(f.type(), std::move(subs));
			} else if (f.type() == FormulaType::AND || f.type() == FormulaType::OR) {
				Formulas<Poly> subs;
				for (auto& sub : f.subformulas()) {
					subs.push_back(to_pnf(sub, prefix, used_vars, true));
				}
				if (f.type() == FormulaType::AND) {
					return Formula<Poly>(FormulaType::OR, std::move(subs));
				} else {
					return Formula<Poly>(FormulaType::AND, std::move(subs));
				}
			} else if (f.type() == FormulaType::IFF) {
				Formulas<Poly> sub1;
				Formulas<Poly> sub2;
				for (auto& sub : f.subformulas()) {
					sub1.push_back(to_pnf(sub, prefix, used_vars, true));
					sub2.push_back(to_pnf(sub, prefix, used_vars, false));
				}
				return Formula<Poly>(FormulaType::AND, {Formula<Poly>(FormulaType::OR, std::move(sub1)), Formula<Poly>(FormulaType::OR, std::move(sub2))});
			} else if (f.type() == FormulaType::XOR) {
				auto lhs = to_pnf(f, prefix, used_vars, false);
				auto rhs = to_pnf(formula::aux::connectPrecedingSubformulas(f), prefix, used_vars, false);
				return Formula<Poly>(FormulaType::IFF, std::vector<Formula<Poly>>({lhs, rhs}));
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
		case FormulaType::VARASSIGN: {
			if (negated)
				return f.negated();
			else
				return f;
		}
		case FormulaType::EXISTS:
		case FormulaType::FORALL: {
			Quantifier q = ((f.type() == FormulaType::EXISTS) ^ negated) ? Quantifier::EXISTS : Quantifier::FORALL;
			Formula<Poly> sub = f.quantified_formula();
			for (auto v : f.quantified_variables()) {
				if (used_vars.contains(v)) {
					auto new_v = fresh_variable(v.type());
					std::stringstream ss; ss << v.name() << "_" << new_v.id();
					VariablePool::getInstance().set_name(new_v, ss.str());
					sub = substitute(sub, v, Poly(new_v));
					v = new_v;
				} else {
					used_vars.insert(v);
				}
				prefix.push_back(std::make_pair(q, v));
			}
			return to_pnf(sub, prefix, used_vars, negated);
		}
		case FormulaType::IMPLIES:
			if (negated) {
				return Formula<Poly>(FormulaType::AND, {to_pnf(f.premise(), prefix, used_vars, false), to_pnf(f.conclusion(), prefix, used_vars, true)});
			} else {
				return Formula<Poly>(FormulaType::IMPLIES, {to_pnf(f.premise(), prefix, used_vars, false), to_pnf(f.conclusion(), prefix, used_vars, false)});
			}
		case FormulaType::ITE:
			return Formula<Poly>(FormulaType::ITE, {to_pnf(f.condition(), prefix, used_vars, negated), to_pnf(f.first_case(), prefix, used_vars, negated), to_pnf(f.second_case(), prefix, used_vars, negated)});
		case FormulaType::NOT:
			return to_pnf(f.subformula(), prefix, used_vars, !negated);
		default:
			assert(false);
			return Formula<Poly>(FormulaType::FALSE);
	}
}

template<typename Poly>
void free_variables(const Formula<Poly>& f, boost::container::flat_set<Variable>& current_quantified_vars, boost::container::flat_set<Variable>& free_vars) {
	switch (f.type()) {
		case FormulaType::AND:
		case FormulaType::IFF:
		case FormulaType::OR:
		case FormulaType::XOR: {
			for (auto& sub : f.subformulas()) {
				free_variables(sub, current_quantified_vars, free_vars);
			}
			break;
		}
		case FormulaType::BOOL:
		case FormulaType::CONSTRAINT:
		case FormulaType::VARCOMPARE: 
		case FormulaType::UEQ:
		case FormulaType::BITVECTOR:{
			for (auto v : variables(f)) {
				if (!current_quantified_vars.contains(v)) {
					free_vars.insert(v);
				}
			}
			break;
		}
		case FormulaType::VARASSIGN: {
			assert(false);
			break;
		}
		case FormulaType::EXISTS:
		case FormulaType::FORALL: {
			auto old = current_quantified_vars;
			current_quantified_vars.insert(f.quantified_variables().begin(), f.quantified_variables().end());
			free_variables(f.quantified_formula(), current_quantified_vars, free_vars);
			current_quantified_vars = old;
			break;
		}
		case FormulaType::IMPLIES:
			free_variables(f.premise(), current_quantified_vars, free_vars);
			free_variables(f.conclusion(), current_quantified_vars, free_vars);
			break;
		case FormulaType::ITE:
			free_variables(f.condition(), current_quantified_vars, free_vars);
			free_variables(f.first_case(), current_quantified_vars, free_vars);
			free_variables(f.second_case(), current_quantified_vars, free_vars);
			break;
		case FormulaType::NOT:
			free_variables(f.subformula(), current_quantified_vars, free_vars);
			break;
		case FormulaType::FALSE:
		case FormulaType::TRUE: {
			break;
		}
		default:
			assert(false);
	}
}

template<typename Poly>
auto free_variables(const Formula<Poly>& f) {
	boost::container::flat_set<Variable> current_quantified_vars;
	boost::container::flat_set<Variable> free_vars;
	free_variables(f, current_quantified_vars, free_vars);
	return free_vars;
}

/**
 * Transforms this formula to its equivalent in prenex normal form.
 * @param negated Used for internal recursion.
 * @return A pair of the prefix and the matrix.
 */
template<typename Poly>
std::pair<QuantifierPrefix, Formula<Poly>> to_pnf(const Formula<Poly>& f) {
	QuantifierPrefix p;
	boost::container::flat_set<Variable> used_vars = free_variables(f);
	auto m = to_pnf(f, p, used_vars, false);
	return std::make_pair(p, m);
}

template<typename Poly>
inline Formula<Poly> to_formula(const QuantifierPrefix& prefix, const Formula<Poly>& matrix) {
	Formula<Poly> res = matrix;
	for (auto p = prefix.rbegin(); p != prefix.rend(); p++) {
		assert(p->first != Quantifier::FREE);
		res = Formula<Poly>(p->first == Quantifier::EXISTS ? FormulaType::EXISTS : FormulaType::FORALL, std::vector<Variable>({p->second}), res);
	}
	return res;
}

} // namespace carl