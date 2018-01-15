#pragma once

#include "../logging.h"
#include "../../converter/CoCoAAdaptor.h"
#include "../../numbers/FunctionSelector.h"

namespace carl {

template<typename C, typename O, typename P>
class MultivariatePolynomial;

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> coprimePart(const MultivariatePolynomial<C,O,P>& p, const MultivariatePolynomial<C,O,P>& q) {
	if (p.isConstant() || q.isConstant()) {
		return p;
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
		[](const auto& p, const auto& q){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p, q}); return c.makeCoprimeWith(p, q); },
		[](const auto& p, const auto& q){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p, q}); return c.makeCoprimeWith(p, q); }
	#else
		[](const auto& p, const auto& q){ return p; },
		[](const auto& p, const auto& q){ return p; }
	#endif
	#if defined USE_GINAC
		,
		[](const auto& p, const auto& q){ return p; },
		[](const auto& p, const auto& q){ return p; }
	#endif
	);
	return s(p, q);
}

}
