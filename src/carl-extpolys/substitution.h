#pragma once

#include "FactorizedPolynomial.h"

namespace carl {

/**
 * Replace the given variable by the given value.
 * @return A new factorized polynomial resulting from this substitution.
 */
template<typename P>
FactorizedPolynomial<P> substitute(const FactorizedPolynomial<P>& p, Variable var, const FactorizedPolynomial<P>& value) {
	std::map<Variable, FactorizedPolynomial<P>> varFPolMap;
	varFPolMap.insert(std::make_pair(var, value));
	std::map<Variable, P> varPolMap;
	varPolMap.insert(std::make_pair(var, value.polynomial()));
	return carl::substitute(p, varFPolMap, varPolMap);
}

/**
 * Replace all variables by a value given in their map.
 * @return A new factorized polynomial without the variables in map.
 */
template<typename P>
FactorizedPolynomial<P> substitute(const FactorizedPolynomial<P>& p, const std::map<Variable, FactorizedPolynomial<P>>& substitutions) {
	std::map<Variable, P> varPolMap;
	for (const auto& varFPolPair : substitutions) {
		varPolMap.insert(varPolMap.end(), std::make_pair(varFPolPair.first, varFPolPair.second.polynomial()));
	}
	return carl::substitute(p, substitutions, varPolMap);
}

/**
 * Replace all variables by a value given in their map.
 * @return A new factorized polynomial without the variables in map.
 */
template<typename P>
FactorizedPolynomial<P> substitute(const FactorizedPolynomial<P>& p, const std::map<Variable, FactorizedPolynomial<P>>& substitutions, const std::map<Variable, P>& substitutionsAsP) {
	if (!existsFactorization(p)) {
		// Contains no variables but only coefficients
		return p;
	}
	if (p.factorizedTrivially()) {
		// Only has one factor
		// Substitute in copy
		P subResult = carl::substitute(p.polynomial(), substitutionsAsP);
		if (subResult.is_constant()) {
			FactorizedPolynomial<P> result(subResult.constant_part() * p.coefficient());
			assert(computePolynomial(result) == substitute(computePolynomial(p),substitutionsAsP));
			return std::move(result);
		} else {
			FactorizedPolynomial<P> result(std::move(subResult), p.pCache());
			result *= p.coefficient();
			assert(computePolynomial(result) == substitute(computePolynomial(p),substitutionsAsP));
			return std::move(result);
		}
	} else {
		P resultCoeff = p.coefficient();
		Factorization<P> resultFactorization;
		// Substitute in all factors
		for (const auto& factor : p.factorization()) {
			FactorizedPolynomial<P> subResult = substitute(factor.first,substitutions);
			if (subResult.is_zero()) {
				return FactorizedPolynomial<P>(constant_zero<P>::get());
			}
			if (subResult.is_constant()) {
				resultCoeff *= carl::pow(subResult.constant_part(), factor.second);
			} else {
				// Add substituted polynomial into factorization
				resultFactorization.insert(std::pair<FactorizedPolynomial<P>, carl::exponent>(subResult, factor.second));
			}
		}
		FactorizedPolynomial<P> result(std::move(resultFactorization), resultCoeff, p.pCache());
		assert(computePolynomial(result) == substitute(computePolynomial(p),substitutionsAsP));
		return std::move(result);
	}
}

/**
 * Replace all variables by a value given in their map.
 * @return A new factorized polynomial without the variables in map.
 */
template<typename P, typename Subs>
FactorizedPolynomial<P> substitute(const FactorizedPolynomial<P>& p, const std::map<Variable, Subs>& substitutions) {
	if (!existsFactorization(p)) {
		// Contains no variables but only coefficients
		return p;
	}
	if (p.factorizedTrivially()) {
		// Only has one factor
		P subResult = carl::substitute(p.polynomial(), substitutions);
		if (subResult.is_constant()) {
			FactorizedPolynomial<P> result(subResult.constant_part() * p.coefficient());
			assert(computePolynomial(result) == carl::substitute(computePolynomial(p), substitutions));
			return result;
		} else {
			FactorizedPolynomial<P> result(std::move(subResult), p.pCache());
			result *= p.coefficient();
			assert(computePolynomial(result) == carl::substitute(computePolynomial(p), substitutions));
			return result;
		}
	} else {
		auto resultCoeff = p.coefficient();
		Factorization<P> resultFactorization;
		// Substitute in all factors
		for (const auto& factor : p.factorization()) {
			FactorizedPolynomial<P> subResult = carl::substitute(factor.first, substitutions);
			if (subResult.is_zero())
				return FactorizedPolynomial<P>(constant_zero<typename P::CoeffType>::get());
			if (subResult.is_constant()) {
				resultCoeff *= carl::pow(subResult.constant_part(), factor.second);
			} else {
				// Add substituted polynomial into factorization
				resultFactorization.insert(std::pair<FactorizedPolynomial<P>, carl::exponent>(subResult, factor.second));
			}
		}
		FactorizedPolynomial<P> result(std::move(resultFactorization), resultCoeff, p.pCache());
		assert(computePolynomial(result) == carl::substitute(computePolynomial(p), substitutions));
		return result;
	}
}


}