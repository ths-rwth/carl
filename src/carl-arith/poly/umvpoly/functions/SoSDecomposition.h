#pragma once

#include "Degree.h"
#include "Power.h"

#include "../MultivariatePolynomial.h"
#include "VarInfo.h"

namespace carl {

template<typename C, typename O, typename P>
std::vector<std::pair<C,MultivariatePolynomial<C,O,P>>> sos_decomposition(const MultivariatePolynomial<C,O,P>& p, bool not_trivial = false) {
	std::vector<std::pair<C,MultivariatePolynomial<C,O,P>>> result;
	if (carl::is_negative(p.lcoeff())) {
		return result;
	}
	if (!not_trivial) {
		for (auto term = p.rbegin(); term != p.rend(); ++term) {
			if (!carl::is_negative(term->coeff())) {
				assert(!carl::is_zero(term->coeff()));
				if (is_constant(*term)) {
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
	if (p.total_degree() != 2) {
		return result;
	}
	auto rem = p;
	while (!rem.is_constant()) {
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Consider " << rem);
		assert(rem.total_degree() <= 2);
		Variable var = (*rem.lterm().monomial())[0].first;
		// Complete the square for var
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Complete the square for " << var);
		auto varInfos = carl::var_info(rem, var, true);
		auto lcoeffIter = varInfos.coeffs().find(2);
		if (lcoeffIter == varInfos.coeffs().end()) {
			CARL_LOG_TRACE("carl.core.sos_decomposition", "Cannot construct sos due to line " << __LINE__);
			return {};
		}
		assert(lcoeffIter->second.is_constant());
		if (carl::is_negative(lcoeffIter->second.constant_part())) {
			CARL_LOG_TRACE("carl.core.sos_decomposition", "Cannot construct sos due to line " << __LINE__);
			return {};
		}
		assert(!carl::is_zero(lcoeffIter->second.constant_part()));
		auto constCoeffIter = varInfos.coeffs().find(0);
		rem = constCoeffIter != varInfos.coeffs().end() ? constCoeffIter->second : MultivariatePolynomial<C,O,P>();
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Constant part is " << rem);
		auto coeffIter = varInfos.coeffs().find(1);
		if (coeffIter != varInfos.coeffs().end()) {
			MultivariatePolynomial<C,O,P> qr = coeffIter->second/(C(2)*lcoeffIter->second.constant_part());
			result.emplace_back(lcoeffIter->second.constant_part(), MultivariatePolynomial<C,O,P>(var)+qr);
			rem -= carl::pow(qr, 2)*lcoeffIter->second.constant_part();
		} else {
			result.emplace_back(lcoeffIter->second.constant_part(), MultivariatePolynomial<C,O,P>(var));
		}
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Add " << result.back().first << " * (" << result.back().second << ")^2");
	}
	if (carl::is_negative(rem.constant_part())) {
		CARL_LOG_TRACE("carl.core.sos_decomposition", "Cannot construct sos due to line " << __LINE__);
		return {};
	} else if(!carl::is_zero(rem.constant_part())) {
		result.emplace_back(rem.constant_part(), MultivariatePolynomial<C,O,P>(constant_one<C>::get()));
	}
	CARL_LOG_TRACE("carl.core.sos_decomposition", "Add " << result.back().first << " * (" << result.back().second << ")^2");
	return result;
}

}
