#pragma once

#include "../logging.h"
#include "../../converter/CoCoAAdaptor.h"
#include "../../numbers/FunctionSelector.h"

namespace carl {

template<typename C, typename O, typename P>
class MultivariatePolynomial;
template<typename C>
class UnivariatePolynomial;

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> squareFreePart(const MultivariatePolynomial<C,O,P>& p) {
	using TypeSelector = carl::function_selector::NaryTypeSelector;

	using types = carl::function_selector::wrap_types<
		mpz_class,mpq_class
#if defined USE_GINAC
		,cln::cl_I,cln::cl_RA
#endif
	>;

	auto s = carl::createFunctionSelector<TypeSelector, types>(
	#if defined USE_COCOA
		[](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.squareFreePart(p); },
		[](const auto& p){ CoCoAAdaptor<MultivariatePolynomial<C,O,P>> c({p}); return c.squareFreePart(p); }
	#else
		[](const auto& p){ return p; },
		[](const auto& p){ return p; }
	#endif
	#if defined USE_GINAC
		,
		[](const auto& p){ return p; },
		[](const auto& p){ return p; }
	#endif
	);
	return s(p);
}

}
