#pragma once

#include "../Formula.h"
#include "ConstraintBounds.h"
#include "Negations.h"
#include "aux.h"

namespace carl {
namespace formula_to_cnf {

/**
 * Constructs the equivalent of
 *   (iff lhs (and *rhs_and)))
 * The result is the list
 *   (=> lhs (and *rhs_and))
 *   (=> rhs !lhs) (for each rhs in rhs_and)
 */
template<typename Poly>
std::vector<Formula<Poly>> construct_iff(const Formula<Poly>& lhs, const std::vector<Formula<Poly>>& rhs_and) {
	std::vector<Formula<Poly>> res;
	Formulas<Poly> subs = { lhs };
	for (const auto& sub: rhs_and) {
		subs.emplace_back(!sub);
		res.emplace_back(FormulaType::OR, sub, !lhs);
	}
	res.emplace_back(FormulaType::OR, std::move(subs));
	return res;
}

template<typename Poly>
using TseitinConstraints = std::vector<Formula<Poly>>;
template<typename Poly>
using ConstraintBounds = FastMap<Poly, std::map<typename Poly::NumberType, std::pair<Relation,Formula<Poly>>>>;

/**
 * Converts an OR to cnf.
 * 
 */
template<typename Poly>
Formula<Poly> to_cnf_or(const Formula<Poly>& f, bool keep_constraints, bool simplify_combinations, bool tseitin_equivalence, TseitinConstraints<Poly>& tseitin) {
	// Checks for immediate tautologies among constraints
	ConstraintBounds<Poly> constraint_bounds;
	// Resulting subformulas
	Formulas<Poly> subformulas;
	// Queue of subformulas to process
	std::vector<Formula<Poly>> subformula_queue = { f };
	while (!subformula_queue.empty()) {
		auto current = subformula_queue.back();
		CARL_LOG_DEBUG("carl.formula.cnf", "Processing " << current << " from " << subformula_queue);
		subformula_queue.pop_back();

		switch (current.type()) {
			case FormulaType::TRUE:
				return Formula<Poly>(FormulaType::TRUE);
			case FormulaType::FALSE:
				break;
			case FormulaType::BITVECTOR:
			case FormulaType::BOOL:
			case FormulaType::UEQ:
			case FormulaType::VARASSIGN:
			case FormulaType::VARCOMPARE:
				subformulas.emplace_back(current);
				break;
			case FormulaType::CONSTRAINT:
				// Try simplification with ConstraintBounds
				if (simplify_combinations) {
					if (addConstraintBound(constraint_bounds, current, false).is_false()) {
						CARL_LOG_DEBUG("carl.formula.cnf", "Adding " << current << " to constraint bounds yielded a tautology");
						return Formula<Poly>(FormulaType::TRUE);
					}
				} else {
					subformulas.emplace_back(current);
				}
				break;
			case FormulaType::NOT: {
				// Resolve negation
				auto resolved = resolve_negation(current, keep_constraints);
				if (resolved.is_literal()) {
					subformulas.emplace_back(resolved);
				} else {
					subformula_queue.emplace_back(resolved);
				}
				break;
			}
			case FormulaType::IMPLIES:
				// (=> A B) -> (not A), B
				subformula_queue.emplace_back(!current.premise());
				subformula_queue.emplace_back(current.conclusion());
				break;
			case FormulaType::ITE:
				// (ite C T E) -> (and (=> C T) (=> (not C) E)) -> (and (or (not C) T) (or C E))
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::AND, {
					Formula<Poly>(FormulaType::OR, {
						!current.condition(), current.first_case()
					}),
					Formula<Poly>(FormulaType::OR, {
						current.condition(), current.second_case()
					})
				}));
				break;
			case FormulaType::IFF: {
				// (iff A ...) -> (and A ...), (and (not A) ...)
				Formulas<Poly> subA;
				Formulas<Poly> subB;
				for (const auto& sub: current.subformulas()) {
					subA.emplace_back(sub);
					subB.emplace_back(!sub);
				}
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::AND, std::move(subA)));
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::AND, std::move(subB)));
				break;
			}
			case FormulaType::XOR: {
				// (xor A B) -> (and A (not B)), (and (not A) B)
				auto lhs = formula::aux::connectPrecedingSubformulas(current);
				const auto& rhs = current.subformulas().back();
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::AND, { lhs, !rhs }));
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::AND, { !lhs, rhs }));
				break;
			}
			case FormulaType::OR:
				// Simply add subformulas to the queue
				for (const auto& sub: current.subformulas()) {
					subformula_queue.emplace_back(sub);
				}
				break;
			case FormulaType::AND: {
				// Replace by a fresh tseitin variable.
				auto tseitinVar = FormulaPool<Poly>::getInstance().createTseitinVar(current);
				if (tseitin_equivalence) {
					tseitin.emplace_back(Formula<Poly>(FormulaType::IFF, { tseitinVar, current }));
				} else {
					tseitin.emplace_back(Formula<Poly>(FormulaType::IMPLIES, { tseitinVar, current }));
				}
				subformulas.emplace_back(tseitinVar);
				break;
			}
			case FormulaType::EXISTS:
			case FormulaType::AUX_EXISTS:
			case FormulaType::FORALL:
				CARL_LOG_ERROR("carl.formula.cnf", "Cannot transform quantified formula to CNF");
				assert(false);
				break;
		}
	}
	if (simplify_combinations && swapConstraintBounds(constraint_bounds, subformulas, false)) {
		return Formula<Poly>(FormulaType::TRUE);
	} else if (subformulas.empty()) {
		tseitin.clear();
		return Formula<Poly>(FormulaType::FALSE);
	} else if (subformulas.size() == 1) {
		return subformulas.front();
	}
	return Formula<Poly>(FormulaType::OR, std::move(subformulas));
}

}

/**
 * Converts the given formula to CNF.
 * @param f Formula to convert.
 * @param keep_constraints Indicates whether to keep constraints or allow to change them in resolve_negation().
 * @param simplify_combinations Indicates whether we attempt to simplify combinations of constraints with ConstraintBounds.
 * @param tseitin_equivalence Indicates whether we use implications or equivalences for tseitin variables.
 * @return The formula in CNF.
 */
template<typename Poly>
Formula<Poly> to_cnf(const Formula<Poly>& f, bool keep_constraints = true, bool simplify_combinations = false, bool tseitin_equivalence = true) {
	if (!simplify_combinations && f.property_holds(PROP_IS_IN_CNF)) {
		if (keep_constraints) {
			return f;
		} else if (f.type() == FormulaType::NOT) {
			assert(f.is_literal());
			return resolve_negation(f,keep_constraints);
		}
	} else if (f.is_atom()) {
		return f;
	}

	// Checks for immediate conflicts among constraints
	formula_to_cnf::ConstraintBounds<Poly> constraint_bounds;
	// Resulting subformulas
	Formulas<Poly> subformulas;
	// Queue of subformulas to process
	std::vector<Formula<Poly>> subformula_queue = { f };
	while (!subformula_queue.empty()) {
		auto current = subformula_queue.back();
		CARL_LOG_DEBUG("carl.formula.cnf", "Processing " << current << " from " << subformula_queue);
		subformula_queue.pop_back();

		switch (current.type()) {
			case FormulaType::TRUE:
				break;
			case FormulaType::FALSE:
				return Formula<Poly>(FormulaType::FALSE);
			case FormulaType::BITVECTOR:
			case FormulaType::BOOL:
			case FormulaType::UEQ:
			case FormulaType::VARASSIGN:
			case FormulaType::VARCOMPARE:
				subformulas.emplace_back(current);
				break;
			case FormulaType::CONSTRAINT:
				// Try simplification with ConstraintBounds
				if (simplify_combinations) {
					if (addConstraintBound(constraint_bounds, current, true).is_false()) {
						CARL_LOG_DEBUG("carl.formula.cnf", "Adding " << current << " to constraint bounds yielded a conflict");
						return Formula<Poly>(FormulaType::FALSE);
					}
				} else {
					subformulas.emplace_back(current);
				}
				break;
			case FormulaType::NOT: {
				// Resolve negation
				auto resolved = resolve_negation(current, keep_constraints);
				if (resolved.is_literal()) {
					subformulas.emplace_back(resolved);
				} else {
					subformula_queue.emplace_back(resolved);
				}
				break;
			}
			case FormulaType::IMPLIES:
				// (=> A B) -> (or (not A) B)
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, {
					!current.premise(), current.conclusion()
				}));
				break;
			case FormulaType::ITE:
				// (ite C T E) -> (=> C T), (=> (not C) E) -> (or (not C) T), (or C E)
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, {
					!current.condition(), current.first_case()
				}));
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, {
					current.condition(), current.second_case()
				}));
				break;
			case FormulaType::IFF:
				if (current.subformulas().size() == 2) {
					const auto& lhs = current.subformulas().front();
					const auto& rhs = current.subformulas().back();
					if (lhs.type() == FormulaType::AND) {
						auto tmp = formula_to_cnf::construct_iff(rhs, lhs.subformulas());
						subformula_queue.insert(subformula_queue.end(), tmp.begin(), tmp.end());
					} else if (rhs.type() == FormulaType::AND) {
						auto tmp = formula_to_cnf::construct_iff(lhs, rhs.subformulas());
						subformula_queue.insert(subformula_queue.end(), tmp.begin(), tmp.end());
					} else {
						// (iff A B) -> (or !A B), (or A !B)
						subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, { !lhs, rhs }));
						subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, { lhs, !rhs }));
					}
				} else {
					// (iff A ...) -> (or (and A ...) (and (not A) ...))
					Formulas<Poly> subA;
					Formulas<Poly> subB;
					for (const auto& sub: current.subformulas()) {
						subA.emplace_back(sub);
						subB.emplace_back(!sub);
					}
					subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, {
						Formula<Poly>(FormulaType::AND, std::move(subA)),
						Formula<Poly>(FormulaType::AND, std::move(subB))
					}));
				}
				break;
			case FormulaType::XOR: {
				// (xor A B) -> (or A B), (or (not A) (not B))
				auto lhs = formula::aux::connectPrecedingSubformulas(current);
				const auto& rhs = current.subformulas().back();
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, { lhs, rhs }));
				subformula_queue.emplace_back(Formula<Poly>(FormulaType::OR, { !lhs, !rhs }));
				break;
			}
			case FormulaType::AND:
				// Simply add subformulas to the queue
				for (const auto& sub: current.subformulas()) {
					subformula_queue.emplace_back(sub);
				}
				break;
			case FormulaType::OR: {
				// Call to_cnf_or() to obtain a clause of literals res and the newly created tseitin variables defined in tseitin.
				formula_to_cnf::TseitinConstraints<Poly> tseitin;
				auto res = formula_to_cnf::to_cnf_or(current, keep_constraints, simplify_combinations, tseitin_equivalence, tseitin);
				if (res.is_false()) {
					return Formula<Poly>(FormulaType::FALSE);
				}
				subformula_queue.insert(subformula_queue.end(), tseitin.begin(), tseitin.end());
				subformulas.emplace_back(res);
				break;
			}
			case FormulaType::EXISTS:
			case FormulaType::FORALL:
				CARL_LOG_ERROR("carl.formula.cnf", "Cannot transform quantified formula to CNF");
				assert(false);
				break;
		}
	}
	if (simplify_combinations && swapConstraintBounds(constraint_bounds, subformulas, true)) {
		return Formula<Poly>(FormulaType::FALSE);
	} else if (subformulas.empty()) {
		return Formula<Poly>(FormulaType::TRUE);
	} else if (subformulas.size() == 1) {
		return subformulas.front();
	}
	return Formula<Poly>(FormulaType::AND, std::move(subformulas));
}

}