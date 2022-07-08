#pragma once

#include <carl-arith/vs/SqrtEx.h>
#include <optional>
#include <vector>

namespace carl::vs {

/**
 * A square root expression with side conditions.
 */
template<typename Poly>
struct zero {
	SqrtEx<Poly> sqrt_ex;
	Constraints<Poly> side_condition;
};
template<typename Poly>
std::ostream& operator<<(std::ostream& out, const zero<Poly>& z) {
    out << z.sqrt_ex << " (if " << z.side_condition << ")";
    return out;
}

/**
 * Gathers zeros with side conditions from the given constraint in the given variable.
 */
template<typename Poly>
static bool gather_zeros(const Constraint<Poly>& constraint, const Variable& eliminationVar, std::vector<zero<Poly>>& results) {
	using Rational = typename Poly::NumberType;
    
    if (!constraint.variables().has(eliminationVar)) {
		return true;
	}

	std::vector<Poly> factors;
	Constraints<Poly> sideConditions;

	auto& factorization = constraint.lhs_factorization();
	if (!carl::is_trivial(factorization)) {
		for (const auto& iter : factorization) {
			if (carl::variables(iter.first).has(eliminationVar)) {
				factors.push_back(iter.first);
			} else {
				Constraint<Poly> cons = Constraint<Poly>(iter.first, carl::Relation::NEQ);
				if (cons != Constraint<Poly>(true)) {
					assert(cons != Constraint<Poly>(false));
					sideConditions.insert(cons);
				}
			}
		}
	} else {
		factors.push_back(constraint.lhs());
	}

	for (const auto& factor : factors) {
		auto varInfo = carl::var_info(factor,eliminationVar,true);
		const auto& coeffs = varInfo.coeffs();
		assert(!coeffs.empty());

		switch (coeffs.rbegin()->first) {
		case 0: {
			assert(false);
			break;
		}
		case 1: // degree = 1
		{
			Poly constantCoeff;
			auto iter = coeffs.find(0);
			if (iter != coeffs.end()) constantCoeff = iter->second;
			// Create state ({b!=0} + oldConditions, [x -> -c/b]):
			Constraint<Poly> cons = Constraint<Poly>(coeffs.rbegin()->second, carl::Relation::NEQ);
			if (cons != Constraint<Poly>(false)) {
				Constraints<Poly> sideCond = sideConditions;
				if (cons != Constraint<Poly>(true)) {
					sideCond.insert(cons);
				}
				SqrtEx<Poly> sqEx(-constantCoeff, Poly(), coeffs.rbegin()->second, Poly());
				results.push_back({std::move(sqEx), std::move(sideCond)});
			}
			break;
		}
		case 2: // degree = 2
		{
			Poly constantCoeff;
			auto iter = coeffs.find(0);
			if (iter != coeffs.end()) constantCoeff = iter->second;
			Poly linearCoeff;
			iter = coeffs.find(1);
			if (iter != coeffs.end()) linearCoeff = iter->second;
			Poly radicand = carl::pow(linearCoeff, 2) - Rational(4) * coeffs.rbegin()->second * constantCoeff;
			Constraint<Poly> cons11 = Constraint<Poly>(coeffs.rbegin()->second, carl::Relation::EQ);
			if (cons11 != Constraint<Poly>(false)) {
				// Create state ({a==0, b!=0} + oldConditions, [x -> -c/b]):
				Constraint<Poly> cons12 = Constraint<Poly>(linearCoeff, carl::Relation::NEQ);
				if (cons12 != Constraint<Poly>(false)) {
					Constraints<Poly> sideCond = sideConditions;
					if (cons11 != Constraint<Poly>(true))
						sideCond.insert(cons11);
					if (cons12 != Constraint<Poly>(true))
						sideCond.insert(cons12);
					SqrtEx<Poly> sqEx(-constantCoeff, Poly(), linearCoeff, Poly());
					results.push_back({std::move(sqEx), std::move(sideCond)});
				}
			}
			Constraint<Poly> cons21 = Constraint<Poly>(radicand, carl::Relation::GEQ);
			if (cons21 != Constraint<Poly>(false)) {
				Constraint<Poly> cons22 = Constraint<Poly>(coeffs.rbegin()->second, carl::Relation::NEQ);
				if (cons22 != Constraint<Poly>(false)) {
					Constraints<Poly> sideCond = sideConditions;
					if (cons21 != Constraint<Poly>(true)) {
						sideCond.insert(cons21);
					}
					if (cons22 != Constraint<Poly>(true)) {
						sideCond.insert(cons22);
					}

					// Create ({a!=0, b^2-4ac>=0} + oldConditions, [x -> (-b-sqrt(b^2-4ac))/2a]):
					SqrtEx<Poly> sqExA(-linearCoeff, Poly(-1), Rational(2) * coeffs.rbegin()->second, radicand);
					results.push_back({std::move(sqExA), sideCond});

					// Create ({a!=0, b^2-4ac>=0} + oldConditions, [x -> (-b+sqrt(b^2-4ac))/2a]):
					SqrtEx<Poly> sqExB(-linearCoeff, Poly(1), Rational(2) * coeffs.rbegin()->second, radicand);
					results.push_back({std::move(sqExB), std::move(sideCond)});
				}
			}
			break;
		}
		//degree > 2 (> 3)
		default: {
			// degree to high
			return false;
			break;
		}
		}
	}
	return true;
}

template<typename Poly>
static bool gather_zeros(const VariableComparison<Poly>& varcomp, const Variable& eliminationVar, std::vector<zero<Poly>>& results) {
	using Rational = typename Poly::NumberType;

	if (varcomp.var() != eliminationVar && !carl::variables(varcomp).has(eliminationVar) ) {
		return true;
	}
	auto as_constr = carl::as_constraint(varcomp);
	if (as_constr) {
		return gather_zeros(Constraint<Poly>(*as_constr), eliminationVar, results);
	}
	if (std::holds_alternative<MultivariateRoot<Poly>>(varcomp.value())) {
		return false;
	}
	assert(varcomp.var() == eliminationVar);

	const auto& ran = std::get<IntRepRealAlgebraicNumber<Rational>>(varcomp.value());
	if (ran.is_numeric()) {
		results.push_back({SqrtEx<Poly>(Poly(Rational(ran.value()))), Constraints<Poly>()});
		return true;
	}
	else {
		for (const auto& factor : carl::irreducible_factors(carl::defining_polynomial(varcomp), false)) {
			assert(factor.degree(eliminationVar) > 0);
			if (factor.degree(eliminationVar) > 2) continue;

			auto roots = real_roots(carl::to_univariate_polynomial(factor, eliminationVar));
			assert(roots.is_univariate());
			auto it = std::find(roots.roots().begin(), roots.roots().end(), ran);
			if (it != roots.roots().end()) {
				size_t idx = std::distance(roots.roots().begin(), it);

				auto varInfo = carl::var_info(factor,eliminationVar,true);
				const auto& coeffs = varInfo.coeffs();
				assert(!coeffs.empty());

				if (coeffs.rbegin()->first == 1) {
					assert(idx == 0);
					Poly constantCoeff;
					auto iter = coeffs.find(0);
					if (iter != coeffs.end()) constantCoeff = iter->second;
					// b!=0
					assert(!carl::is_zero(coeffs.rbegin()->second));
					// Create state [x -> -c/b]
					SqrtEx<Poly> sqEx(-constantCoeff, Poly(), coeffs.rbegin()->second, Poly());
					results.push_back({std::move(sqEx), {}});
				} else {
					assert(coeffs.rbegin()->first == 2);

					Poly constantCoeff;
					auto iter = coeffs.find(0);
					if (iter != coeffs.end()) constantCoeff = iter->second;
					Poly linearCoeff;
					iter = coeffs.find(1);
					if (iter != coeffs.end()) linearCoeff = iter->second;
					Poly radicand = carl::pow(linearCoeff, 2) - Rational(4) * coeffs.rbegin()->second * constantCoeff;

					// a!=0, b^2-4ac>=0
					assert(Constraint<Poly>(radicand, carl::Relation::GEQ) == Constraint<Poly>(true));
					assert(Constraint<Poly>(coeffs.rbegin()->second, carl::Relation::NEQ) == Constraint<Poly>(true));

					if (idx == 0) {
						// Create[x -> (-b-sqrt(b^2-4ac))/2a]
						SqrtEx<Poly> sqExA(-linearCoeff, Poly(-1), Rational(2) * coeffs.rbegin()->second, radicand);
						results.push_back({std::move(sqExA), {}});
					} else {
						assert(idx == 1);
						// Create [x -> (-b+sqrt(b^2-4ac))/2a]
						SqrtEx<Poly> sqExB(-linearCoeff, Poly(1), Rational(2) * coeffs.rbegin()->second, radicand);
						results.push_back({std::move(sqExB), {}});
					}
				}
				return true;
			}
		}
	}
	return false;
}
}