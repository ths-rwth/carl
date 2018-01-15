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
	
	template<typename C, typename O, typename P>
	Factors<MultivariatePolynomial<C,O,P>> trivialFactorization(const MultivariatePolynomial<C,O,P>& p) {
		return { std::make_pair(p, 1) };
	}
}

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
		[includeConstants](const auto& p){ return trivialFactorization(p); },
		[includeConstants](const auto& p){ return trivialFactorization(p); }
	#endif
	#if defined USE_GINAC
		,
		[includeConstants](const auto& p){ return ginacFactorization(p); },
		[includeConstants](const auto& p){ return ginacFactorization(p); }
	#endif
	);
	return s(p);
}

}
