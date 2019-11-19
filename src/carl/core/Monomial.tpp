#pragma once

#include "../numbers/numbers.h"

namespace carl
{
	template<typename Coefficient>
	Coefficient Monomial::substitute(const std::map<Variable, Coefficient>& substitutions) const {
		CARL_LOG_FUNC("carl.core.monomial", *this << ", " << substitutions);
		Coefficient res = carl::constant_one<Coefficient>::get();
		for (const auto& ve : mExponents) {
			auto it = substitutions.find(ve.first);
			if (it == substitutions.end()) {
				res *= carl::pow(ve.first, ve.second);
			} else {
				res *= carl::pow(it->second, ve.second);
			}
		}
		CARL_LOG_TRACE("carl.core.monomial", "Result: " << res);
		return res;
	}

	template<typename Coefficient>
	Coefficient Monomial::evaluate(const std::map<Variable, Coefficient>& substitutions) const {
		CARL_LOG_FUNC("carl.core.monomial", *this << ", " << substitutions);
		Coefficient res = carl::constant_one<Coefficient>::get();
		for (const auto& ve : mExponents) {
			auto it = substitutions.find(ve.first);
			assert(it != substitutions.end());
			res *= carl::pow(it->second, ve.second);
		}
		CARL_LOG_TRACE("carl.core.monomial", "Result: " << res);
		return res;
	}
}
