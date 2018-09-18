#pragma once

#include "../logging.h"
#include "../../converter/CoCoAAdaptor.h"
#include "../../converter/OldGinacConverter.h"
#include "../../numbers/FunctionSelector.h"
#include "../../util/Common.h"

namespace carl {

template<typename C, typename O, typename P>
class MultivariatePolynomial;

namespace helper {
	/**
	 * Returns a factors datastructure containing only the full polynomial as single factor.
	 */
	template<typename C, typename O, typename P>
	Factors<MultivariatePolynomial<C,O,P>> trivialFactorization(const MultivariatePolynomial<C,O,P>& p) {
		return { std::make_pair(p, 1) };
	}
	
	template<typename C, typename O, typename P>
	void sanitizeFactors(const MultivariatePolynomial<C,O,P>& reference, Factors<MultivariatePolynomial<C,O,P>>& factors) {
		MultivariatePolynomial<C,O,P> p(1);
		for (const auto& f: factors) {
			p *= f.first.pow(f.second);
		}
		if (p == reference) return;
		if (p == -reference) {
			CARL_LOG_WARN("carl.core.factorize", "The factorization had an incorrect sign, correct it.");
			CARL_LOG_WARN("carl.core.factorize", reference << " -> " << factors);
			MultivariatePolynomial<C,O,P> factor(-1);
			auto it = std::find_if(factors.begin(), factors.end(), [](const auto& f){ return f.first.isConstant(); });
			if (it != factors.end()) {
				assert(it->second == 1);
				factor *= it->first;
				factors.erase(it);
			}
			factors.emplace(factor, 1);
			return;
		}
		CARL_LOG_WARN("carl.core.factorize", "The factorization was incorrect, return trivial factorization.");
		CARL_LOG_WARN("carl.core.factorize", reference << " -> " << factors);
		factors = trivialFactorization(reference);
	}
}

/**
 * Try to factorize a multivariate polynomial..
 * Uses CoCoALib and GiNaC, if available, depending on the coefficient type of the polynomial.
 */
template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factorization(const MultivariatePolynomial<C,O,P>& p, bool includeConstants = true) {
	if (p.totalDegree() <= 1) {
		return helper::trivialFactorization(p);
	}
	using TypeSelector = carl::function_selector::NaryTypeSelector;

	using types = carl::function_selector::wrap_types<
		mpz_class,mpq_class
#if defined USE_GINAC
		,cln::cl_I,cln::cl_RA
#endif
	>;

	auto s = carl::createFunctionSelector<TypeSelector, types>(
	#if defined USE_COCOA
		[includeConstants](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.factorize(p, includeConstants); },
		[includeConstants](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.factorize(p, includeConstants); }
	#else
		[includeConstants](const auto& p){ return helper::trivialFactorization(p); },
		[includeConstants](const auto& p){ return helper::trivialFactorization(p); }
	#endif
	#if defined USE_GINAC
		,
		[includeConstants](const auto& p){ return ginacFactorization(p); },
		[includeConstants](const auto& p){ return ginacFactorization(p); }
	#endif
	);
	auto factors = s(p);
	helper::sanitizeFactors(p, factors);
	return factors;
}

/**
 * Try to factorize a multivariate polynomial and return the irreducible factors (without multiplicities).
 * Uses CoCoALib and GiNaC, if available, depending on the coefficient type of the polynomial.
 */
template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C,O,P>> irreducibleFactors(const MultivariatePolynomial<C,O,P>& p, bool includeConstants = true) {
	if (p.totalDegree() <= 1) {
		return {p};
	}
	using TypeSelector = carl::function_selector::NaryTypeSelector;

	using types = carl::function_selector::wrap_types<
		mpz_class,mpq_class
#if defined USE_GINAC
		,cln::cl_I,cln::cl_RA
#endif
	>;

	auto s = carl::createFunctionSelector<TypeSelector, types>(
	#if defined USE_COCOA
		[includeConstants](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.irreducibleFactors(p, includeConstants); },
		[includeConstants](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.irreducibleFactors(p, includeConstants); }
	#else
		[includeConstants](const auto& p){ return std::vector<MultivariatePolynomial<C,O,P>>({p}); },
		[includeConstants](const auto& p){ return std::vector<MultivariatePolynomial<C,O,P>>({p}); }
	#endif
	#if defined USE_GINAC
		,
		[includeConstants](const auto& p){ return std::vector<MultivariatePolynomial<C,O,P>>({p}); },
		[includeConstants](const auto& p){ return std::vector<MultivariatePolynomial<C,O,P>>({p}); }
	#endif
	);
	return s(p);
}

}
