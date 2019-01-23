#pragma once

#include "../MultivariatePolynomial.h"

namespace carl {

template<typename C, typename O, typename P>
std::vector<std::pair<C,MultivariatePolynomial<C,O,P>>> SoSDecomposition(const MultivariatePolynomial<C,O,P>& p, bool not_trivial = false) {
	std::vector<std::pair<C,MultivariatePolynomial<C,O,P>>> result;
	if (carl::isNegative(p.lcoeff())) {
		return result;
	}
	if (!not_trivial) {
		for (auto term = p.rbegin(); term != p.rend(); ++term) {
			if (!carl::isNegative(term->coeff())) {
				assert(!carl::isZero(term->coeff()));
				if (term->isConstant()) {
					result.emplace_back(term->coeff(), MultivariatePolynomial<C,O,P>(constant_one<C>::get()));
				} else {
					auto resMonomial = term->monomial()->sqrt();
					if (resMonomial == nullptr) {
						break;
					}
					result.emplace_back(term->coeff(), MultivariatePolynomial<C,O,P>(resMonomial));
				}
			} else {
				break;
			}
		}
		result.clear();
	}
	// TODO: If cheap, find substitution of monomials to variables such that applied to this polynomial a quadratic form is achieved.
	// Only compute sos for quadratic forms.
	if (p.totalDegree() != 2) {
		return result;
	}
	auto rem = p;
	while (!rem.isConstant()) {
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Consider " << rem);
		assert(rem.totalDegree() <= 2);
		Variable var = (*rem.lterm().monomial())[0].first;
		// Complete the square for var
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Complete the square for " << var);
		auto varInfos = rem.template getVarInfo<true>(var);
		auto lcoeffIter = varInfos.coeffs().find(2);
		if (lcoeffIter == varInfos.coeffs().end()) {
			CARL_LOG_TRACE("carl.core.sos_decomposition", "Cannot construct sos due to line " << __LINE__);
			return {};
		}
		assert(lcoeffIter->second.isConstant());
		if (carl::isNegative(lcoeffIter->second.constantPart())) {
			CARL_LOG_TRACE("carl.core.sos_decomposition", "Cannot construct sos due to line " << __LINE__);
			return {};
		}
		assert(!carl::isZero(lcoeffIter->second.constantPart()));
		auto constCoeffIter = varInfos.coeffs().find(0);
		rem = constCoeffIter != varInfos.coeffs().end() ? constCoeffIter->second : MultivariatePolynomial<C,O,P>();
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Constant part is " << rem);
		auto coeffIter = varInfos.coeffs().find(1);
		if (coeffIter != varInfos.coeffs().end()) {
			MultivariatePolynomial<C,O,P> qr = coeffIter->second/(C(2)*lcoeffIter->second.constantPart());
			result.emplace_back(lcoeffIter->second.constantPart(), MultivariatePolynomial<C,O,P>(var)+qr);
			rem -= qr.pow(2)*lcoeffIter->second.constantPart();
		} else {
			result.emplace_back(lcoeffIter->second.constantPart(), MultivariatePolynomial<C,O,P>(var));
		}
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Add " << result.back().first << " * (" << result.back().second << ")^2");
	}
	if (carl::isNegative(rem.constantPart())) {
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Cannot construct sos due to line " << __LINE__);
		return {};
	} else if(!carl::isZero(rem.constantPart())) {
		result.emplace_back(rem.constantPart(), MultivariatePolynomial<C,O,P>(constant_one<C>::get()));
	}
	CARL_LOG_TRACE("carl.core.sos_decomposition", "Add " << result.back().first << " * (" << result.back().second << ")^2");
	return result;
}

}
