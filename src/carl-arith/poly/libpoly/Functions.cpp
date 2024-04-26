#include "Functions.h"

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "CoCoAAdaptorLP.h"

namespace carl {

inline Factors<LPPolynomial> factorization(const LPPolynomial& p, const CoCoAAdaptorLP& adaptor, bool includeConstant) {
    #ifndef USE_COCOA
    CARL_LOG_FATAL("carl.poly", "factorization is not supported without libcocoa");
    #endif
    return adaptor.factorize(p, includeConstant);
}

/*
 * @brief wrapper for the factorization of LPPolynomials
 * @param LPPolynomial p
 * @return factorization(p)
 */
Factors<LPPolynomial> factorization(const LPPolynomial& p, bool includeConstant) {
    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(p.context());
    return factorization(p, adaptor, includeConstant);
}

inline std::vector<LPPolynomial> irreducible_factors(const LPPolynomial& p, const CoCoAAdaptorLP& adaptor, bool includeConstants) {
	#ifndef USE_COCOA
    CARL_LOG_FATAL("carl.poly", "factorization is not supported without libcocoa");
    #endif
    
    if (is_constant(p)) {
		if (includeConstants) {
			return {p};
		} else {
			return {};
		}
	} else if (p.total_degree() == 1) {
		return {p};
	}

    return adaptor.irreducible_factors(p, includeConstants);
}

std::vector<LPPolynomial> irreducible_factors(const LPPolynomial& p, bool includeConstants) {
    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(p.context());
    return irreducible_factors(p, adaptor, includeConstants);
}

std::vector<LPPolynomial> square_free_factors(const LPPolynomial& p) {
    lp_polynomial_t** factors = nullptr;
    std::size_t* multiplicities = nullptr;
    std::size_t size = 0;
    lp_polynomial_factor_square_free(p.get_internal(), &factors, &multiplicities, &size);
    std::vector<LPPolynomial> res;
    for (std::size_t i = 0; i < size; ++i) {
        res.emplace_back(factors[i], p.context());
    }
    free(factors);
    free(multiplicities);
    return res;
}

std::vector<LPPolynomial> content_free_factors(const LPPolynomial& p) {
    lp_polynomial_t** factors = nullptr;
    std::size_t* multiplicities = nullptr;
    std::size_t size = 0;
    lp_polynomial_factor_content_free(p.get_internal(), &factors, &multiplicities, &size);
    std::vector<LPPolynomial> res;
    for (std::size_t i = 0; i < size; ++i) {
        res.emplace_back(factors[i], p.context());
    }
    free(factors);
    free(multiplicities);
    return res;
}

std::vector<LPPolynomial> groebner_basis(const std::vector<LPPolynomial>& polys) {
    if (polys.size() <= 1) return polys;
    CoCoAAdaptorLP adaptor = CoCoAAdaptorLP(polys.at(0).context());
    return adaptor.GBasis(polys);
}

}

#endif