#pragma once

#include "Power.h"

#include <carl-logging/carl-logging.h>
#include "../CoCoAAdaptor.h"
#include <carl-arith/converter/OldGinacConverter.h>
#include <carl-arith/core/Common.h>

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
			p *= carl::pow(f.first, f.second);
		}
		if (p == reference) return;
		if (p == -reference) {
			CARL_LOG_WARN("carl.core.factorize", "The factorization had an incorrect sign, correct it.");
			CARL_LOG_WARN("carl.core.factorize", reference << " -> " << factors);
			MultivariatePolynomial<C,O,P> factor(-1);
			auto it = std::find_if(factors.begin(), factors.end(), [](const auto& f){ return f.first.is_constant(); });
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
	if (p.total_degree() == 0) {
		if (includeConstants) {
			return helper::trivialFactorization(p);
		} else {
			return {};
		}
	} else if (p.total_degree() == 1) {
		return helper::trivialFactorization(p);
	}

	auto s = overloaded {
	#if defined USE_COCOA
		[includeConstants](const MultivariatePolynomial<mpq_class,O,P>& p){ CoCoAAdaptor<MultivariatePolynomial<mpq_class,O,P>> c({p}); return c.factorize(p, includeConstants); },
		[includeConstants](const MultivariatePolynomial<mpz_class,O,P>& p){ CoCoAAdaptor<MultivariatePolynomial<mpz_class,O,P>> c({p}); return c.factorize(p, includeConstants); }
	#else
		[](const MultivariatePolynomial<mpq_class,O,P>& p){ return helper::trivialFactorization(p); },
		[](const MultivariatePolynomial<mpz_class,O,P>& p){ return helper::trivialFactorization(p); }
	#endif
	#if defined USE_GINAC
		,
		[](const MultivariatePolynomial<cln::cl_RA,O,P>& p){ return ginacFactorization(p); },
		[](const MultivariatePolynomial<cln::cl_I,O,P>& p){ return ginacFactorization(p); }
	#endif
	};

	auto factors = s(p);
	helper::sanitizeFactors(p, factors);
	return factors;
}

template<typename C, typename O, typename P>
bool is_trivial(const Factors<MultivariatePolynomial<C,O,P>>& f) {
	return f.size() <= 1;
}
 

/**
 * Try to factorize a multivariate polynomial and return the irreducible factors (without multiplicities).
 * Uses CoCoALib and GiNaC, if available, depending on the coefficient type of the polynomial.
 */
template<typename C, typename O, typename P>
std::vector<MultivariatePolynomial<C,O,P>> irreducible_factors(const MultivariatePolynomial<C,O,P>& p, bool includeConstants = true) {
	if (p.total_degree() == 0) {
		if (includeConstants) {
			return {p};
		} else {
			return {};
		}
	} else if (p.total_degree() == 1) {
		return {p};
	}

	auto s = overloaded {
	#if defined USE_COCOA
		[includeConstants](const MultivariatePolynomial<mpq_class,O,P>& p){ CoCoAAdaptor<MultivariatePolynomial<mpq_class,O,P>> c({p}); return c.irreducible_factors(p, includeConstants); },
		[includeConstants](const MultivariatePolynomial<mpz_class,O,P>& p){ CoCoAAdaptor<MultivariatePolynomial<mpz_class,O,P>> c({p}); return c.irreducible_factors(p, includeConstants); }
	#else
		[includeConstants](const MultivariatePolynomial<mpq_class,O,P>& p){ return std::vector<MultivariatePolynomial<mpq_class,O,P>>({p}); },
		[includeConstants](const MultivariatePolynomial<mpz_class,O,P>& p){ return std::vector<MultivariatePolynomial<mpz_class,O,P>>({p}); }
	#endif
	#if defined USE_GINAC
		,
		[](const MultivariatePolynomial<cln::cl_RA,O,P>& p){ return std::vector<MultivariatePolynomial<cln::cl_RA,O,P>>({p}); },
		[](const MultivariatePolynomial<cln::cl_I,O,P>& p){ return std::vector<MultivariatePolynomial<cln::cl_I,O,P>>({p}); }
	#endif
	};
	return s(p);
}

}
